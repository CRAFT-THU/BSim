import os
from ctypes import *

import subprocess

from bsim import utils
from bsim.cudamemop import cudamemops, CUDAMemOp


class CConnection(Structure):
    _fields_ = [
        ("delay_start", POINTER(c_int)),
        ("delay_num", POINTER(c_int)),
        ("rev_delay_start", POINTER(c_int)),
        ("rev_delay_num", POINTER(c_int)),
        ("rev_map2sid", POINTER(c_int)),
        ("n_length", c_int),
        ("s_length", c_int)
    ]


class Connection(object):
    def __init__(self):
        self.delay_start = []
        self.delay_num = []
        self.rev_delay_start = []
        self.rev_delay_num = []
        self.rev_map2sid = []
        self.dir = os.path.dirname(__file__)
        self._so = None

    def so(self):
        if not self._so:
            self.compile_()
        return self._so

    def to_c(self):
        assert len(self.delay_start) == len(self.delay_num) and \
               len(self.rev_delay_start) == len(self.rev_delay_num) and \
               len(self.delay_start) == len(self.rev_delay_start)
        c = CConnection()
        c.n_length = len(self.delay_start)
        c.s_length = len(self.rev_map2sid)
        c.delay_start = (c_int * len(self.delay_start))(*(self.delay_start))
        c.delay_num = (c_int * len(self.delay_num))(*(self.delay_num))

        c.rev_delay_start = (c_int * len(self.rev_delay_start))(*(self.rev_delay_start))
        c.rev_delay_num = (c_int * len(self.rev_delay_num))(*(self.rev_delay_num))
        c.rev_map2sid = (c_int * len(self.rev_map2sid))(*(self.rev_map2sid))

        return c

    def to_gpu(self):
        c_data = self.to_c()
        return self.so().to_gpu_connection(pointer(c_data))

    def from_gpu(self, gpu: POINTER(CConnection), only_struct=True):
        cpu = self.so().from_gpu_connection(gpu)
        c = cast(cpu, POINTER(CConnection*1)).contents[0]

        if not only_struct:
            c.delay_start = cast(
                cudamemops.from_gpu_int(c.delay_start, c.n_length),
                POINTER(c_int * c.n_length)
            ).content
            c.delay_num = cast(
                cudamemops.from_gpu_int(c.delay_num, c.n_length),
                POINTER(c_int * c.n_length)
            ).content
            c.rev_delay_start = cast(
                cudamemops.from_gpu_int(c.rev_delay_start, c.n_length),
                POINTER(c_int * c.n_length)
            ).content
            c.rev_delay_num = cast(
                cudamemops.from_gpu_int(c.rev_delay_num, c.n_length),
                POINTER(c_int * c.n_length)
            ).content
            c.rev_map2sid = cast(
                cudamemops.from_gpu_int(c.rev_map2sid, c.n_length),
                POINTER(c_int * c.n_length)
            ).content

        return c

    def compile_(self):
        self._compile_h()
        self._compile_data_cu()

        if subprocess.call('/usr/local/cuda/bin/nvcc -I/usr/local/cuda/include/ -shared '
                               '--compiler-options "-Wall -Wfatal-errors -Ofast -fPIC -shared" '
                               '%s/c_code/connection.data.cu -o %s/c_so/connection.data.so' %
                               (self.dir , self.dir),
                               shell=True) == 0:
            self._so = cdll.LoadLibrary('%s/c_so/connection.data.so' % self.dir)
            self._so.to_gpu_connection.restype = POINTER(CConnection)
            self._so.from_gpu_connection.restype = POINTER(CConnection)
        else:
            self._so = None
            raise EnvironmentError('Compile file connection.data.so failed')

    def _compile_h(self):
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

    def _compile_data_cu(self):
        cu_file = open('%s/c_code/connection.data.cu' % self.dir, mode="w+")

        cu_file.write("\n\n")
        cu_file.write('#include <stdlib.h>\n')
        cu_file.write("\n")
        cu_file.write('#include "helper_cuda.h"\n')
        cu_file.write('#include "connection.h"\n')
        cu_file.write("\n\n")

        cu_file.write("CConnection * to_gpu_connection(CConnection *cpu)\n")
        cu_file.write("{\n")
        cu_file.write('\tCConnection * gpu = (CConnection*)malloc(sizeof(CConnection));\n')

        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->delay_start', cpu='cpu->delay_start', type_='int', num='cpu->n_length'))
        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->delay_num', cpu='cpu->delay_num', type_='int', num='cpu->n_length'))

        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->rev_delay_start', cpu='cpu->rev_delay_start', type_='int', num='cpu->n_length'))
        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->rev_delay_num', cpu='cpu->rev_delay_num', type_='int', num='cpu->n_length'))

        cu_file.write(CUDAMemOp.to_gpu(ret='gpu->rev_map2sid', cpu='cpu->rev_map2sid', type_='int', num='cpu->s_length'))

        cu_file.write(utils.code_line('CConnection * ret = NULL'))
        cu_file.write(CUDAMemOp.to_gpu(ret='ret', cpu='gpu', type_='CConnection'))
        cu_file.write('\n\treturn ret;\n')
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.write("CConnection * from_gpu_connection(CConnection *gpu)\n")
        cu_file.write("{\n")
        cu_file.write(CUDAMemOp.from_gpu(gpu='gpu', ret='ret', type_='CConnection'))
        cu_file.write('\n\treturn ret;\n')
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.close()
        return







