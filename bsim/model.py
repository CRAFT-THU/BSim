import os
from abc import ABC, abstractmethod

from bsim import pkg_dir
from bsim.generator import CGenerator, PyGenerator, CUDAGenerator


class Model(ABC):
    def __init__(self):
        pass

    @abstractmethod
    def generate_compute_cu(self, debug=False):
        pass

    def generate_h(self):
        h_gen = CGenerator("{}/code_gen/{}.h".format(pkg_dir, self.name.lower()))

        h_gen.blank_line(2)
        h_gen.if_define("{}_H ".format(self.name.upper()))
        h_gen.blank_line(2)

        h_gen.struct(self.name.capitalize(), 0)

        for i in self.parameters['special']:
            h_gen.line("int *p_%s" % i)

        for i in self.parameters['constant']:
            h_gen.line("float *p_%s" % i)

        for i in self.parameters['variable']:
            h_gen.line("float *p_%s" % i)

        h_gen.line("}", 0)
        h_gen.blank_line(1)

        h_gen.line_no_end('extern "C" {', 0)
        h_gen.line("void update_{}({} *data, int num, int start_id, int t)"
                   .format(self.name.lower(), self.name.capitalize()))
        h_gen.line("{} * to_gpu_{}({} *cpu, int num)"
                   .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()))
        h_gen.line("{} * from_gpu_{}({} *gpu, int num)"
                   .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()))
        h_gen.close_brace()
        h_gen.blank_line(1)

        h_gen.end_if_define("{}_H".format(self.name.upper()))

        h_gen.close()
        return

    def generate_py(self):
        py_gen = PyGenerator('{}/code_gen/{}.py'.format(pkg_dir, self.name.lower()))

        py_gen.blank_line()
        py_gen.import_("*", "ctypes")
        py_gen.blank_line(2)
        py_gen.class_(self.name.capitalize(), "Structure")
        py_gen.line("_fields_ = [")

        for i in list(self.parameters['special']):
            py_gen.line('("p_{}", POINTER(c_int)),'.format(i), 2)

        for i in list(self.parameters['constant']):
            py_gen.line('("p_{}", POINTER(c_float)),'.format(i), 2)

        for i in self.parameters['variable']:
            py_gen.line('("p_{}", POINTER(c_float)),'.format(i), 2)

        py_gen.line("]")
        py_gen.blank_line()

        py_gen.close()
        return

    def generate_data_cu(self, debug=False):
        cu_gen = CUDAGenerator('{}/code_gen/{}.data.cu'.format(pkg_dir, self.name.lower()))

        cu_gen.blank_line(2)
        cu_gen.include_std('stdlib.h')
        cu_gen.blank_line()
        cu_gen.include("../c_code/helper_cuda.h")
        cu_gen.include("{}.h".format(self.name.lower()))
        cu_gen.blank_line(2)

        cu_gen.line_no_end("{} * to_gpu_{}({} *cpu, int num)"
                           .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()), 0)
        cu_gen.open_brace()
        cu_gen.malloc(ret='gpu', type_=self.name.capitalize())

        for i in self.parameters['special']:
            cu_gen.to_gpu(ret='gpu->p_{}'.format(i), cpu='cpu->p_{}'.format(i), num='num', type_='int')

        for i in self.parameters['constant']:
            cu_gen.to_gpu(ret='gpu->p_{}'.format(i), cpu='cpu->p_{}'.format(i), num='num', type_='float')

        for i in self.parameters['variable']:
            cu_gen.to_gpu(ret='gpu->p_{}'.format(i), cpu='cpu->p_{}'.format(i), num='num', type_='float')

        cu_gen.line('{} * ret = NULL'.format(self.name.capitalize()))
        cu_gen.to_gpu(ret='ret', cpu='gpu', num='1', type_=self.name.capitalize())

        cu_gen.line('return ret')
        cu_gen.close_brace()
        cu_gen.blank_line()

        cu_gen.line_no_end("{} * from_gpu_{}({} *gpu, int num)"
                           .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()), 0)
        cu_gen.open_brace()
        cu_gen.from_gpu(ret='cpu', gpu='gpu', type_=self.name.capitalize())

        cu_gen.line('return cpu')
        cu_gen.close_brace()
        cu_gen.blank_line()

        cu_gen.close()
        return




