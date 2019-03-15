
import os
import ctypes
from ctypes import *
from typing import List

from bsim import pkg_dir
from bsim.generator import CGenerator, CUDAGenerator


class CUDAMemOp(object):
    def __init__(self, types: List):
        self.types = types
        self._so = None

    def so(self):
        if not self._so:
            self.compile_()

        return self._so

    def compile_(self):
        self._generate_h()
        self._generate_cu()

        # TODO nvcc path
        if CUDAGenerator.compile_(
                src='{}/code_gen/cudamemop.cu'.format(pkg_dir),
                output='{}/so_gen/cudamemop.so'.format(pkg_dir)):
            self._so = cdll.LoadLibrary('{}/so_gen/cudamemop.so'.format(pkg_dir))
            for i in self.types:
                getattr(self._so, 'gpu_malloc_{}'.format(i)).restype = POINTER(getattr(ctypes, 'c_%s' % i))
                getattr(self._so, 'to_gpu_{}'.format(i)).restype = POINTER(getattr(ctypes, 'c_%s' % i))
                getattr(self._so, 'from_gpu_{}'.format(i)).restype = POINTER(getattr(ctypes, 'c_%s' % i))
        else:
            self._so = None
            raise EnvironmentError('Compile file connection.data.so failed')

    def _generate_h(self):
        h_gen = CGenerator("{}/code_gen/cudamemop.h".format(pkg_dir))

        h_gen.blank_line(2)
        h_gen.if_define("cudamemop.h")
        h_gen.blank_line(2)

        h_gen.line_no_end('extern "C" {', tab=0)
        for i in self.types:
            h_gen.line("{}* gpu_malloc_{}(int size)".format(i, i))
            h_gen.line("void cpu2gpu_{}({} *cpu, {} *gpu, int size)".format(i, i, i))
            h_gen.line("void gpu2cpu_{}({} *gpu, {} *cpu, int size)".format(i, i, i))
            h_gen.line("{}* to_gpu_{}({} *cpu, int size)".format(i, i, i))
            h_gen.line("{}* from_gpu_{}({} *gpu, int size)".format(i, i, i))
        h_gen.close_brace()

        h_gen.blank_line()
        h_gen.end_if_define("cudamemop.h")
        return

    def _generate_cu(self):
        cu_gen = CUDAGenerator('{}/code_gen/cudamemop.cu'.format(pkg_dir))

        cu_gen.blank_line(2)
        cu_gen.include("../c_code/helper_cuda.h")
        cu_gen.include("cudamemop.h")
        cu_gen.blank_line(2)

        for i in self.types:
            cu_gen.line_no_end("{}* gpu_malloc_{}(int size)".format(i, i), 0)
            cu_gen.open_brace(0)
            cu_gen.line('{} * gpu = NULL'.format(i))
            cu_gen.malloc_gpu(ret='gpu', type_=str(i), num='size')
            cu_gen.line(line='return gpu')
            cu_gen.close_brace(0)
            cu_gen.blank_line()

            cu_gen.line_no_end("void cpu2gpu_{}({} *cpu, {} *gpu, int size)".format(i, i, i), 0)
            cu_gen.open_brace(0)
            cu_gen.cpu_to_gpu(gpu='gpu', cpu='cpu', type_=str(i), num='size')
            cu_gen.close_brace(0)
            cu_gen.blank_line()

            cu_gen.line_no_end("void gpu2cpu_{}({} *gpu, {} *cpu, int size)".format(i, i, i), 0)
            cu_gen.open_brace(0)
            cu_gen.gpu_to_cpu(gpu='gpu', cpu='cpu', type_=str(i), num='size')
            cu_gen.close_brace(0)
            cu_gen.blank_line()

            cu_gen.line_no_end("{}* to_gpu_{}({} *cpu, int size)".format(i, i, i), 0)
            cu_gen.open_brace(0)
            cu_gen.line(line='{} * gpu = NULL'.format(i))
            cu_gen.to_gpu(ret='gpu', cpu='cpu', type_=str(i), num='size')
            cu_gen.line(line='return gpu', tab=1)
            cu_gen.close_brace(0)
            cu_gen.blank_line()

            cu_gen.line_no_end("{}* from_gpu_{}({} *gpu, int size)".format(i, i, i), 0)
            cu_gen.open_brace(0)
            cu_gen.from_gpu(gpu='gpu', ret='cpu', type_=str(i), num='size')
            cu_gen.line(line='return cpu', tab=1)
            cu_gen.close_brace(0)
            cu_gen.blank_line()

        cu_gen.blank_line()
        cu_gen.close()

        return

cudamemops = CUDAMemOp(['int', 'float', 'double']).so()
