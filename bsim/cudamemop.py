
import os
import subprocess
import ctypes
from ctypes import *
from typing import List


class CUDAMemOp(object):
    def __init__(self, types: List):
        self.types = types

    def so(self):
        if self.compile_():
            memcpy_so = cdll.LoadLibrary('%s/c_so/cudamemop.so' % os.path.dirname(__file__))

            for i in self.types:
                getattr(memcpy_so, 'gpu_malloc_%s' % i).restype = POINTER(getattr(ctypes, 'c_%s' % i))

            return memcpy_so
        else:
            raise EnvironmentError('Compile file connection.data.so failed')
        return None

    def compile_(self):
        self._compile_h()
        self._compile_cu()

        # TODO nvcc path
        return subprocess.call('/usr/local/cuda/bin/nvcc -I/usr/local/cuda/include/ -shared '
                               '--compiler-options "-Wall -Wno-switch -Wfatal-errors -Ofast -fPIC -shared" '
                               ' %s/c_code/cudamemop.cu -o %s/c_so/cudamemop.so' %
                               (os.path.dirname(__file__), os.path.dirname(__file__)),
                               shell=True) == 0

    def _compile_h(self):
        h_file = open("%s/c_code/cudamemop.h" % os.path.dirname(__file__), mode="w+")

        h_file.write("\n\n")
        h_file.write("#ifndef CUDAMEMCPY_H\n")
        h_file.write("#define CUDAMEMCPY_H\n")
        h_file.write("\n\n")

        h_file.write('extern "C" {\n')

        for i in self.types:
            h_file.write("\t%s* gpu_malloc_%s(int size);\n" % (i, i))
            h_file.write("\tvoid cpu2gpu_%s(%s *cpu, %s *gpu, int size);\n" % (i, i, i))
            h_file.write("\tvoid gpu2cpu_%s(%s *gpu, %s *cpu, int size);\n" % (i, i, i))
            h_file.write("\n")

        h_file.write("}\n")
        h_file.write("\n")

        h_file.write("#endif /* CUDAMEMCPY_H */\n")

        h_file.close()
        return

    def _compile_cu(self):
        cu_file = open('%s/c_code/cudamemop.cu' % os.path.dirname(__file__), mode="w+")

        cu_file.write("\n\n")
        cu_file.write('#include "mem_op.h"\n')
        cu_file.write('#include "cudamemop.h"\n')
        cu_file.write("\n\n")

        for i in self.types:

            cu_file.write("%s* gpu_malloc_%s(int size)\n" % (i, i))
            cu_file.write("{\n")
            cu_file.write("\treturn gpuMalloc<%s>(size);\n" % i)
            cu_file.write("}\n")
            cu_file.write("\n")

            cu_file.write("void cpu2gpu_%s(%s *cpu, %s *gpu, int size)\n" % (i, i, i))
            cu_file.write("{\n")
            cu_file.write("\tcopyToGPU<%s>(gpu, cpu, size);\n" % i)
            cu_file.write("}\n")
            cu_file.write("\n")

            cu_file.write("void gpu2cpu_%s(%s *gpu, %s *cpu, int size)\n" % (i, i, i))
            cu_file.write("{\n")
            cu_file.write("\tcopyFromGPU<%s>(cpu, gpu, size);\n" % i)
            cu_file.write("}\n")
            cu_file.write("\n")

        cu_file.write("\n")
        cu_file.close()
        return
