import os
from ctypes import *

import subprocess

import importlib

from bsim import utils
from bsim.cudamemop import cudamemops, CUDAMemOp


#class CConnection(Structure):
#    _fields_ = [
#        ("delay_start", POINTER(c_int)),
#        ("delay_num", POINTER(c_int)),
#        ("rev_delay_start", POINTER(c_int)),
#        ("rev_delay_num", POINTER(c_int)),
#        ("rev_map2sid", POINTER(c_int)),
#        ("n_length", c_int),
#        ("s_length", c_int)
#    ]


class Connection(object):
    def __init__(self, debug = False):
        self.delay_start = []
        self.delay_num = []
        self.rev_delay_start = []
        self.rev_delay_num = []
        self.rev_map2sid = []

        self.dir = os.path.dirname(__file__)
        self.debug = debug
        self._so = None
        self.cconnection = None

    def so(self):
        if not self._so:
            self.compile_()
        return self._so

    def to_c(self):
        assert len(self.delay_start) == len(self.delay_num) and \
               len(self.rev_delay_start) == len(self.rev_delay_num) and \
               len(self.delay_start) == len(self.rev_delay_start)

        self._generate_py()

        self.cconnection = importlib.import_module(
            'bsim.py_code.cconnection_%s_%s' % (len(self.delay_start), len(self.rev_map2sid))
            ).CConnection
        c = self.cconnection()
        c.n_length = len(self.delay_start)
        c.s_length = len(self.rev_map2sid)
        c.delay_start = pointer((c_int * len(self.delay_start))(*(self.delay_start)))
        c.delay_num = pointer((c_int * len(self.delay_num))(*(self.delay_num)))

        c.rev_delay_start = pointer((c_int * len(self.rev_delay_start))(*(self.rev_delay_start)))
        c.rev_delay_num = pointer((c_int * len(self.rev_delay_num))(*(self.rev_delay_num)))
        c.rev_map2sid = pointer((c_int * len(self.rev_map2sid))(*(self.rev_map2sid)))

        return c

    def to_gpu(self):
        c_data = self.to_c()
        gpu_data = self.so().to_gpu_connection(pointer(c_data))

        if self.debug:
            print("\nPython GPU Pointer: %s" % hex(cast(gpu_data, c_void_p).value))

        return gpu_data

    def from_gpu(self, gpu, only_struct=True):
        """
        :param gpu:  must be of type POINTER(self.cconnection)
        :param only_struct:
        :return:
        """
        cpu = self.so().from_gpu_connection(gpu)
        c = cast(cpu, POINTER(self.cconnection*1)).contents[0]

        if self.debug:
            print("\nPython CPU Pointer: %s" % hex(cast(cpu, c_void_p).value))
            print("Python CPU n_lenght: %s s_lenght: %s\n" % (int(c.n_length), int(c.s_length)))

        if not only_struct:
            c.delay_start = cast(cudamemops.from_gpu_int(c.delay_start, c.n_length), POINTER(c_int*c.n_length))
            c.delay_num = cast(cudamemops.from_gpu_int(c.delay_num, c.n_length), POINTER(c_int*c.n_length))
            c.rev_delay_start = cast(cudamemops.from_gpu_int(c.rev_delay_start, c.n_length), POINTER(c_int*c.n_length))
            c.rev_delay_num = cast(cudamemops.from_gpu_int(c.rev_delay_num, c.n_length), POINTER(c_int*c.n_length))
            c.rev_map2sid = cast(cudamemops.from_gpu_int(c.rev_map2sid, c.s_length), POINTER(c_int*c.s_length))

        return c

    def compile_(self):
        self._generate_h()
        self._generate_data_cu()

        if subprocess.call('/usr/local/cuda/bin/nvcc -I/usr/local/cuda/include/ -shared '
                               '--compiler-options "-Wall -Wfatal-errors -Ofast -fPIC -shared" '
                               '%s/c_code/connection.data.cu -o %s/c_so/connection.data.so' %
                               (self.dir , self.dir),
                               shell=True) == 0:
            self._so = cdll.LoadLibrary('%s/c_so/connection.data.so' % self.dir)
            self._so.to_gpu_connection.restype = POINTER(self.cconnection)
            self._so.from_gpu_connection.restype = POINTER(self.cconnection)
        else:
            self._so = None
            raise EnvironmentError('Compile file connection.data.so failed')

    def _generate_h(self):
        h_file = open("%s/c_code/connection.h" % self.dir, mode="w+")

        h_file.write("\n\n")
        h_file.write("#ifndef CONNECTION_H\n ")
        h_file.write("#define CONNECTION_H\n")
        h_file.write("\n\n")

        h_file.write("struct CConnection {\n")
        h_file.write("\tint *delay_start;\n")
        h_file.write("\tint *delay_num;\n")
        h_file.write("\tint *rev_delay_start;\n")
        h_file.write("\tint *rev_delay_num;\n")
        h_file.write("\tint *rev_map2sid;\n")
        h_file.write("\tint n_length;\n")
        h_file.write("\tint s_length;\n")

        h_file.write("};\n")
        h_file.write("\n")

        h_file.write('extern "C" {\n')
        h_file.write("\tCConnection * to_gpu_connection(CConnection *cpu);\n")
        h_file.write("\tCConnection * from_gpu_connection(CConnection *gpu);\n")
        h_file.write("}\n")
        h_file.write("\n")

        h_file.write("#endif /* CONNECTION_H */\n")

        h_file.close()

    def _generate_data_cu(self):
        cu_file = open('%s/c_code/connection.data.cu' % self.dir, mode="w+")

        cu_file.write("\n\n")
        if self.debug:
            cu_file.write(utils.code_line_no_end(line='#include <stdio.h>', tab=0))
        cu_file.write('#include <stdlib.h>\n')
        cu_file.write("\n")
        cu_file.write('#include "helper_cuda.h"\n')
        cu_file.write('#include "connection.h"\n')
        cu_file.write("\n\n")

        cu_file.write("CConnection * to_gpu_connection(CConnection *cpu)\n")
        cu_file.write("{\n")
        cu_file.write('\tCConnection * gpu = (CConnection*)malloc(sizeof(CConnection));\n')

        cu_file.write(utils.code_line('gpu->n_length = cpu->n_length'))
        cu_file.write(utils.code_line('gpu->s_length = cpu->s_length'))

        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->delay_start', cpu='cpu->delay_start', type_='int', num='cpu->n_length'))
        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->delay_num', cpu='cpu->delay_num', type_='int', num='cpu->n_length'))

        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->rev_delay_start', cpu='cpu->rev_delay_start', type_='int', num='cpu->n_length'))
        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->rev_delay_num', cpu='cpu->rev_delay_num', type_='int', num='cpu->n_length'))

        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->rev_map2sid', cpu='cpu->rev_map2sid', type_='int', num='cpu->s_length'))

        cu_file.write(utils.code_line('CConnection * ret = NULL'))
        cu_file.write(CUDAMemOp.to_gpu(ret='ret', cpu='gpu', type_='CConnection'))

        if self.debug:
            cu_file.write(utils.code_line(line=r'printf("GPU CConnection Pointer: %p\n", ret)'))
            cu_file.write(utils.code_line(line=r'printf("GPU n_length: %d  s_length: %d\n", gpu->n_length, gpu->s_length)'))

        cu_file.write('\n\treturn ret;\n')
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.write("CConnection * from_gpu_connection(CConnection *gpu)\n")
        cu_file.write("{\n")
        cu_file.write(CUDAMemOp.from_gpu(gpu='gpu', ret='ret', type_='CConnection'))

        if self.debug:
            cu_file.write(utils.code_line(line=r'printf("CPU CConnection Pointer: %p\n", ret)'))
            cu_file.write(utils.code_line(line=r'printf("CPU n_length: %d s_length: %d\n", ret->n_length, ret->s_length)'))

        cu_file.write('\n\treturn ret;\n')
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.close()
        return

    def _generate_py(self):
        py_file = open('%s/py_code/cconnection_%s_%s.py' % (self.dir, len(self.delay_start), len(self.rev_map2sid)), mode="w+")

        py_file.write("from ctypes import *\n\n\n")
        py_file.write("class CConnection(Structure):\n")
        py_file.write("\t_fields_ = [\n")
        py_file.write('\t\t("delay_start", POINTER(c_int * %s)),\n' % len(self.delay_start))
        py_file.write('\t\t("delay_num", POINTER(c_int * %s)),\n' % len(self.delay_num))
        py_file.write('\t\t("rev_delay_start", POINTER(c_int * %s)),\n' % len(self.rev_delay_start))
        py_file.write('\t\t("rev_delay_num", POINTER(c_int * %s)),\n' % len(self.rev_delay_num))
        py_file.write('\t\t("rev_map2sid", POINTER(c_int * %s)),\n' % len(self.rev_map2sid))
        py_file.write('\t\t("n_length", c_int),\n')
        py_file.write('\t\t("s_length", c_int)\n')
        py_file.write("\t]\n")
        py_file.write('\n')

        py_file.close()
        return







