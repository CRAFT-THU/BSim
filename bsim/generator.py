import subprocess


class BaseGenerator(object):
    def __init__(self, filename: str= ''):
        self.file = open(filename, "w+")

    def line_no_end(self, line: str='', tab: int=1):
        self.file.write('{}{}\n'.format(tab * '\t', line))

    def blank_line(self, num: int=1):
        self.file.write('\n'*num)

    def close(self):
        self.file.close()


class PyGenerator(BaseGenerator):
    def __init__(self, filename: str= ''):
        self.file = open(filename, "w+")

    def line(self, line: str='', tab: int=1):
        self.line_no_end(line, tab=tab)

    def import_(self, name: str='', package: str=''):
        if not package == '':
            self.file.write('from {} import {}\n'.format(package, name))
        else:
            self.file.write('import {}\n'.format(name))

    def class_(self, name: str='', parent: str='', tab: int=0):
        self.line('class {}({}):'.format(name, parent), tab=tab)


class CGenerator(BaseGenerator):
    def __init__(self, filename: str= ''):
        self.file = open(filename, "w+")

    def line(self, line, tab: int=1):
        self.line_no_end('{};'.format(line), tab=tab)

    def struct(self, name: str='', tab: int=0):
        self.line_no_end('struct {} {{'.format(name), tab=tab)

    def if_define(self, name: str='', tab: int=0):
        self.line_no_end('#ifndef {}'.format(name.replace('.', '_').upper()), tab=tab)
        self.line_no_end('#define {}'.format(name.replace('.', '_').upper()), tab=tab)

    def end_if_define(self, name: str='', tab: int=0):
        self.line_no_end('#endif // {}'.format(name.replace('.', '_').upper()), tab=tab)

    def include(self, filename: str='', tab: int=0):
        self.line_no_end('#include "{}"'.format(filename), tab=tab)

    def include_std(self, filename: str='', tab: int=0):
        self.line_no_end('#include <{}>'.format(filename), tab=tab)

    def open_brace(self, tab: int=0):
        self.line_no_end("{", tab=tab)

    def close_brace(self, tab: int=0):
        self.line_no_end("}", tab=tab)

    def block(self, line: str=0):
        self.line_no_end(line, tab=0)

    def malloc(self, ret: str="", type_: str="", num='1', tab: int=1):
        self.line(
            line='{} * {} = static_cast<{}*>(malloc(sizeof({})*{}))'
                .format(type_, ret, type_, type_, num),
            tab=tab
        )

    @staticmethod
    def compile_(src: str='a.cpp', output: str='a.so'):
        return subprocess.call(
            '/usr/bin/g++ -Wall -Wno-switch -Wfatal-errors -Ofast -fPIC -shared '
            '{} -o {}'.format(src, output),
            shell=True) == 0


class CUDAGenerator(CGenerator):
    def __init__(self, filename: str= ''):
        self.file = open(filename, "w+")

    def cu_line(self, line: str='', tab: int=1):
        self.line(line='checkCudaErrors({})'.format(line), tab=tab)

    def malloc_gpu(self, ret: str="", type_: str="", num='1', tab: int=1):
        self.cu_line(
            line='cudaMalloc((void**)&({}), sizeof({})*{})'
                .format(ret, type_, num),
            tab=tab
        )
        self.cu_line(
            line='cudaMemset({}, 10, sizeof({})*{})'
                .format(ret, type_, num),
            tab=tab
        )

    def malloc_symbol(self, symbol:str, gpu: str, type_: str="int", num='1', tab: int=1):
        self.malloc_gpu(ret=gpu, type_=type_, num=num, tab=tab)
        self.cu_line(
            line='cudaMemcpyToSymbol({}, &{}, sizeof({}))'.format(symbol, gpu, gpu),
            tab=tab
        )

    def to_gpu(self, ret: str, cpu: str, type_:str= 'double', num='1', tab: int=1):
        self.malloc_gpu(ret=ret, type_=type_, num=num, tab=tab)
        self.cpu_to_gpu(gpu=ret, cpu=cpu, type_=type_, num=num, tab=tab)

    def from_gpu(self, ret: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.malloc(ret=ret, type_=type_, num=num, tab=tab)
        self.gpu_to_cpu(gpu=gpu, cpu=ret, type_=type_, num=num, tab=tab)

    def cpu_to_gpu(self, cpu: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.cu_line(
            line='cudaMemcpy({}, {}, sizeof({})*{}, cudaMemcpyHostToDevice)'
                .format(gpu, cpu, type_, num),
            tab=tab
        )

    def gpu_to_cpu(self, cpu:str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.cu_line(
            line='cudaMemcpy({}, {}, sizeof({})*{}, cudaMemcpyDeviceToHost)'
                .format(cpu, gpu, type_, num),
            tab=tab
        )

    @staticmethod
    def compile_(src: str='a.cu', output: str='a.so'):
        cmd = '{} {} {} -o {}'.format(
            '/usr/local/cuda/bin/nvcc -I/usr/local/cuda/include/ -shared -rdc=true',
            '--compiler-options "-Wall -Wno-switch -Wfatal-errors -Ofast -fPIC -shared"',
            src, output
        )
        # print('{}\n'.format(cmd))
        return subprocess.call(cmd, shell=True) == 0
