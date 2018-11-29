
from ctypes import *

import subprocess


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

    def to_c(self):
        assert len(self.delay_start) == len(self.delay_num) and \
               len(self.rev_delay_start) == len(self.rev_delay_num) and \
               len(self.delay_start) == len(self.rev_delay_start)
        c = CConnection()
        c.n_length = len(self.delay_start)
        c.s_length = len(self.delay_num)
        c.delay_start = (c_int * len(self.delay_start))(*(self.delay_start))
        c.delay_num = (c_int * len(self.delay_num))(*(self.delay_num))

        c.rev_delay_start = (c_int * len(self.rev_delay_start))(*(self.rev_delay_start))
        c.rev_delay_num = (c_int * len(self.rev_delay_num))(*(self.rev_delay_num))
        c.rev_map2sid = (c_int * len(self.rev_map2sid))(*(self.rev_map2sid))

        return c

    def to_gpu(self):

        if self.compile_():
            connection_data_so = cdll.LoadLibrary('./c_so/connection.data.so')
            c_data = self.to_c()
            return connection_data_so.to_gpu_connection(c_data)
        else:
            raise EnvironmentError('Compile file connection.data.so failed')

        return None

    def compile_(self):
        self._compile_h()
        self._compile_data_cu()

        return subprocess.call('nvcc -I/usr/local/cuda/include/ -shared'
                               '--compiler-options "-Wall -Wfatal-errors -Ofast -fPIC" -c '
                               './c_code/connection.data.cu -o ./c_so/connection.data.so',
                               shell=True) == 0

    @staticmethod
    def _compile_h():
        h_file = open("./c_code/connection.h")

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
        h_file.write("\tCConnection * to_gpu_connection(CConnection *cpum);\n")
        h_file.write("}\n")
        h_file.write("\n")

        h_file.close()

    @staticmethod
    def _compile_data_cu():
        c_file = open('./c_code/connection.data.cu')

        c_file.write("\n\n")
        c_file.write('#include <stdlib.h>\n')
        c_file.write("\n")
        c_file.write('#include "../gpu_utils/mem_op.h"\n')
        c_file.write('#include "connection.h"\n')
        c_file.write("\n\n")

        c_file.write('extern "C" {\n')
        c_file.write("\t%CConnection * to_gpu_connection(CConnection *cpu, int num)\n")
        c_file.write("\t{\n")
        c_file.write('\t\tCConnection * gpu = malloc(sizeof(CConnection);\n')
        c_file.write('\t\tgpu->delay_start = copyToGPU<int>(cpu->delay_start, cpu->n_length);\n')
        c_file.write('\t\tgpu->delay_num = copyToGPU<int>(cpu->delay_num, cpu->n_length);\n')
        c_file.write('\t\tgpu->rev_delay_start = copyToGPU<int>(rev_cpu->delay_start, cpu->n_length);\n')
        c_file.write('\t\tgpu->rev_delay_num = copyToGPU<int>(rev_cpu->delay_num, cpu->n_length);\n')
        c_file.write('\t\tgpu->rev_map2sid = copyToGPU<int>(rev_cpu->delay_num, cpu->s_length);\n')
        c_file.write('\t\tret = copyToGPU<CConnection>(gpu, 1);\n')
        c_file.write('\n\t\treturn ret;\n')
        c_file.write("\t}\n")
        c_file.write("}\n")
        c_file.write("\n")

        c_file.close()
        return







