
import os
import subprocess
import ctypes
from ctypes import *
from typing import List

from bsim import utils


class CUDAMemOp(object):
    def __init__(self, types: List):
        self.types = types
        self._so = None

    def so(self):
        if not self._so:
            self.compile_()

        return self._so

    def compile_(self):
        self._compile_h()
        self._compile_cu()

        # TODO nvcc path
        if subprocess.call('/usr/local/cuda/bin/nvcc -I/usr/local/cuda/include/ -shared '
                               '--compiler-options "-Wall -Wno-switch -Wfatal-errors -Ofast -fPIC -shared" '
                               ' %s/c_code/cudamemop.cu -o %s/c_so/cudamemop.so' %
                               (os.path.dirname(__file__), os.path.dirname(__file__)),
                               shell=True) == 0:
            self._so = cdll.LoadLibrary('%s/c_so/cudamemop.so' % os.path.dirname(__file__))
            for i in self.types:
                getattr(self._so, 'gpu_malloc_%s' % i).restype = POINTER(getattr(ctypes, 'c_%s' % i))
                getattr(self._so, 'to_gpu_%s' % i).restype = POINTER(getattr(ctypes, 'c_%s' % i))
                getattr(self._so, 'from_gpu_%s' % i).restype = POINTER(getattr(ctypes, 'c_%s' % i))
        else:
            self._so = None
            raise EnvironmentError('Compile file connection.data.so failed')

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
            h_file.write("\t%s* to_gpu_%s(%s *cpu, int size);\n" % (i, i, i))
            h_file.write("\t%s* from_gpu_%s(%s *gpu, int size);\n" % (i, i, i))
            h_file.write("\n")

        h_file.write("}\n")
        h_file.write("\n")

        h_file.write("#endif /* CUDAMEMCPY_H */\n")

        h_file.close()
        return

    def _compile_cu(self):
        cu_file = open('%s/c_code/cudamemop.cu' % os.path.dirname(__file__), mode="w+")

        cu_file.write("\n\n")
        cu_file.write('#include "helper_cuda.h"\n')
        cu_file.write('#include "cudamemop.h"\n')
        cu_file.write("\n\n")

        for i in self.types:

            cu_file.write("%s* gpu_malloc_%s(int size)\n" % (i, i))
            cu_file.write("{\n")
            cu_file.write(utils.code_line(line='%s * gpu = NULL' % i))
            cu_file.write(self.malloc(ret='gpu', type_=str(i), num='size'))
            cu_file.write(utils.code_line(line='return gpu'))
            cu_file.write("}\n")
            cu_file.write("\n")

            cu_file.write("void cpu2gpu_%s(%s *cpu, %s *gpu, int size)\n" % (i, i, i))
            cu_file.write("{\n")
            cu_file.write(self.cpu_to_gpu(gpu='gpu', cpu='cpu', type_=str(i), num='size'))
            cu_file.write("}\n")
            cu_file.write("\n")

            cu_file.write("void gpu2cpu_%s(%s *gpu, %s *cpu, int size)\n" % (i, i, i))
            cu_file.write("{\n")
            cu_file.write(self.gpu_to_cpu(gpu='gpu', cpu='cpu', type_=str(i), num='size'))
            cu_file.write("}\n")
            cu_file.write("\n")

            cu_file.write("%s* to_gpu_%s(%s *cpu, int size)\n" % (i, i, i))
            cu_file.write("{\n")
            cu_file.write(utils.code_line(line='%s * gpu = NULL' % i))
            cu_file.write(self.to_gpu(ret='gpu', cpu='cpu', type_=str(i), num='size'))
            cu_file.write(utils.code_line(line='return gpu', tab=1))
            cu_file.write("}\n")
            cu_file.write("\n")

            cu_file.write("%s* from_gpu_%s(%s *gpu, int size)\n" % (i, i, i))
            cu_file.write("{\n")
            cu_file.write(self.from_gpu(gpu='gpu', ret='cpu', type_=str(i), num='size'))
            cu_file.write(utils.code_line(line='return cpu', tab=1))
            cu_file.write("}\n")
            cu_file.write("\n")

        cu_file.write("\n")
        cu_file.close()
        return

    @staticmethod
    def malloc(ret: str="", type_: str="", num='1', tab: int=1):
        return utils.code_line(
                   line='checkCudaErrors(cudaMalloc((void**)&(%s), sizeof(%s)*%s))' %
                        (ret, type_, num),
                   tab = tab
               )

    @classmethod
    def to_gpu(cls, ret: str, cpu: str, type_:str= 'double', num='1', tab: int=1):
        return cls.malloc(ret=ret, type_=type_, num=num, tab=tab) +\
               cls.cpu_to_gpu(gpu=ret, cpu=cpu, type_=type_, num=num, tab=tab)

    @classmethod
    def from_gpu(cls, ret: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        return utils.malloc(ret=ret, type_=type_, num=num, tab=tab) + \
               cls.gpu_to_cpu(gpu=gpu, cpu=ret, type_=type_, num=num, tab=tab)

    @staticmethod
    def cpu_to_gpu(cpu: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        return utils.code_line(
            line='checkCudaErrors(cudaMemcpy(%s, %s, sizeof(%s)*%s, cudaMemcpyHostToDevice))' %
                 (gpu, cpu, type_, num),
            tab=tab
        )

    @staticmethod
    def gpu_to_cpu(cpu:str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        return utils.code_line(
            line='checkCudaErrors(cudaMemcpy(%s, %s, sizeof(%s)*%s, cudaMemcpyDeviceToHost))' %
                 (cpu, gpu, type_, num),
            tab=tab
        )


cudamemops = CUDAMemOp(['int', 'float', 'double']).so()
