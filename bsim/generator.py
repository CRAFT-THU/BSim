
import os
import subprocess

TAB = '\t'

class BaseGenerator(object):
    def __init__(self, filename: str = ''):
        self.file = open(filename, "w+")
        self.tab = 0

    def backspace(self, step: int = 1):
        self.file.seek(0, os.SEEK_END)
        self.file.seek(self.file.tell()-step, os.SEEK_SET)
        self.file.write('')

    def print_(self, line: str = '', tab: int = 0):
        tab = max(tab, self.tab) if tab > 0 else tab
        self.file.write('{}{}'.format(tab * TAB, line))

    def line_no_end(self, line: str = '', tab: int = 1):
        tab = max(tab, self.tab) if tab > 0 else tab
        self.file.write('{}{}\n'.format(tab * TAB, line))

    def blank_line(self, num: int = 1):
        self.file.write('\n'*num)

    def close(self):
        self.file.close()


class PyGenerator(BaseGenerator):
    def __init__(self, filename: str= ''):
        super(PyGenerator, self).__init__(filename)

    def line(self, line: str='', tab: int=1):
        self.line_no_end(line, tab=tab)

    def import_(self, name: str='', package: str=''):
        if not package == '':
            self.file.line_no_end('from {} import {}\n'.format(package, name),
                                 tab=0)
        else:
            self.file.line_no_end('import {}\n'.format(name), tab=0)

    def class_(self, name: str='', parent: str='', tab: int=0):
        self.line('class {}({}):'.format(name, parent), tab=tab)


class CGenerator(BaseGenerator):
    def __init__(self, filename: str= ''):
        super(CGenerator, self).__init__(filename)

    def line(self, line:str, tab: int=1):
        if not isinstance(line, str):
            line = str(line)
        if (len(line) < 1) or (line[-1] in [':', '{', '(', ';']):
            self.line_no_end('{}'.format(line), tab)
        else:
            self.line_no_end('{};'.format(line), tab)

    def func(self, line, tab: int=0):
        self.line(line, tab=tab)

    def if_start(self, line, tab: int=1):
        self.print_("if ({}) ".format(line), tab=tab)
        self.open_brace(tab=0)

    def if_end(self, tab: int=1):
        self.close_brace(tab=abs(tab))

    def for_start(self, line, tab: int=1):
        self.print_("for ({}) ".format(line), tab=tab)
        self.open_brace(tab=0)

    def for_end(self, tab: int=1):
        self.close_brace(tab=abs(tab))

    def func_start(self, line, tab: int=0):
        self.line_no_end(line, tab=tab)
        self.open_brace(tab=tab)

    def func_end(self, line='', tab: int=1):
        if not isinstance(line, str):
            line = str(line)
        if len(line) > 0:
            self.line('return {}'.format(line), tab=tab)
        self.close_brace(tab=abs(tab-1))
        self.blank_line()

    def struct(self, name: str='', father: str='', tab: int=0):
        if len(father) < 1:
            self.line_no_end('struct {} '.format(name, father), tab=tab)
            self.open_brace(tab=tab)
        else:
            self.line_no_end('struct {} : {} '.format(name, father), tab=tab)
            self.open_brace(tab=tab)

    def struct_end(self, name: str='', tab: int=0):
        self.line('};', tab=abs(tab))

    def if_define(self, name: str='', tab: int=0):
        self.line_no_end('#ifndef {}'.format(name.replace('.', '_').upper()), tab=tab)
        self.line_no_end('#define {}'.format(name.replace('.', '_').upper()), tab=tab)

    def end_if_define(self, name: str='', tab: int=0):
        self.line_no_end('#endif /* {} */'.format(name.replace('.', '_').upper()), tab=tab)

    def include(self, filename: str='', tab: int=0):
        self.line_no_end('#include "{}"'.format(filename), tab=tab)

    def include_std(self, filename: str='', tab: int=0):
        self.line_no_end('#include <{}>'.format(filename), tab=tab)

    def open_brace(self, tab: int=0):
        self.line_no_end("{", tab=tab)
        self.tab += 1

    def close_brace(self, tab: int=0):
        self.tab -= 1
        self.line_no_end("}", tab=tab)

    def block(self, line: str=0):
        self.line_no_end(line, tab=0)

    def malloc(self, ret: str="", type_: str="", num='1', tab: int=1):
        self.line(line='{} *{} = ({}*)malloc(sizeof({})*{})'.format(type_, ret, type_, type_, str(num)), tab=tab)
        self.line(line='memset({}, 0, sizeof({})*{})'.format(ret, type_, str(num)), tab=tab)

    def free(self, pointer: str="", tab: int=1):
        self.line(line='free({})'.format(pointer), tab=tab)
        self.line(line="{} = NULL".format(pointer), tab=tab)

    @staticmethod
    def compile_(src: str='a.cpp', output: str='a.so'):
        return subprocess.call(
            '/usr/bin/g++ -Wall -Wno-switch -Wfatal-errors -Ofast -fPIC -shared '
            '{} -o {}'.format(src, output),
            shell=True) == 0

class HGenerator(CGenerator):
    def __init__(self, filename: str= ''):
        super(HGenerator, self).__init__(filename)
        _, self.name = os.path.split(filename)
        self.blank_line()
        self.if_define(self.name.upper().replace('.', '_'))
        self.blank_line()

    def close(self):
        self.end_if_define(self.name.upper().replace('.', '_'))
        self.file.close()



class CUDAGenerator(CGenerator):
    def __init__(self, filename: str= ''):
        super(CUDAGenerator, self).__init__(filename)

    def sync(self, tab: int=1):
        self.line("__syncthreads()", tab=tab)
        self.blank_line()

    def cu_line(self, line: str='', tab: int=1):
        self.line(line='checkCudaErrors({})'.format(line), tab=tab)

    def malloc_gpu(self, ret: str="", type_: str="", num='1', tab: int=1):
        self.cu_line(
            line='cudaMalloc((void**)&({}), sizeof({})*{})'
                .format(ret, type_, str(num)),
            tab=tab
        )
        self.cu_line(
            # TODO: memset to 0
            line='cudaMemset({}, 0, sizeof({})*{})'.format(ret, type_, str(num)),
            # line='cudaMemset({}, 10, sizeof({})*{}/2)'.format(ret, type_, num),
            tab=tab
        )

    def malloc_symbol(self, symbol:str, gpu: str, type_: str="int", num='1', tab: int=1):
        self.malloc_gpu(ret=gpu, type_=type_, num=num, tab=tab)
        self.cu_line(
            line='cudaMemcpyToSymbol({}, &{}, sizeof({}))'.format(symbol, gpu, gpu),
            tab=tab
        )

    def free_gpu(self, pointer: str="", tab: int=1):
        self.line(line='cudaFree({})'.format(pointer), tab=tab)
        self.line(line="{} = NULL".format(pointer), tab=tab)

    def to_gpu(self, ret: str, cpu: str, type_:str= 'double', num='1', tab: int=1):
        self.malloc_gpu(ret=ret, type_=type_, num=str(num), tab=tab)
        self.cpu_to_gpu(gpu=ret, cpu=cpu, type_=type_, num=str(num), tab=tab)

    def from_gpu(self, ret: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.malloc(ret=ret, type_=type_, num=num, tab=tab)
        self.gpu_to_cpu(gpu=gpu, cpu=ret, type_=type_, num=num, tab=tab)

    def cpu_to_gpu(self, cpu: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.cu_line(
            line='cudaMemcpy({}, {}, sizeof({})*{}, cudaMemcpyHostToDevice)'
                .format(gpu, cpu, type_, str(num)),
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
            '/usr/local/cuda/bin/nvcc -G -g -I/usr/local/cuda/include/ -arch=sm_60 -shared -rdc=true',
            '--compiler-options "-g -Wall -Wno-unused-function -Wno-switch -Wfatal-errors -Ofast -fPIC -shared"',
            src, output
        )
        # print('{}\n'.format(cmd))
        return subprocess.call(cmd, shell=True) == 0

