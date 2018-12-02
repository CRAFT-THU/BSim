
class CGenerator(object):
    def __init__(self, filename: str= ''):
        self.file = open(filename, "w+")

    def line(self, line, tab: int=1):
        self.file.write('%s%s;\n' % (tab*'\t', line))

    def line_no_end(self, line: str='', tab: int=1):
        self.file.write('%s%s\n' % (tab*'\t', line))

    def if_define(self, name: str=''):
        self.line_no_end('#ifdef %' % name.replace('.', '_').upper())
        self.line_no_end('#define %' % name.replace('.', '_').upper())

    def end_if_define(self, name: str=''):
        self.line_no_end('#endif // %' % name.replace('.', '_').upper())

    def include(self, filename: str='', tab: int=0):
        self.line_no_end('#include "%s"\n' % filename, tab=tab)

    def include_std(self, filename: str='', tab: int=0):
        self.line_no_end('#include <%s>\n' % filename, tab=tab)

    def open_brace(self, tab: int=1):
        self.line_no_end("{\n", tab=tab)

    def close_brace(self, tab: int=1):
        self.line_no_end("}\n", tab=tab)

    def blank_line(self):
        self.line_no_end('\n')

    def malloc(self, ret: str="", type_: str="", num='1', tab: int=1):
        self.line(
            line='%s * %s = static_cast<%s*>(malloc(sizeof(%s)*%s))' %
                 (type_, ret, type_, type_, num),
            tab=tab
        )


class CUDAGenerator(CGenerator):
    def __init__(self, filename: str= ''):
        self.file = open(filename, "w+")

    def malloc_gpu(self, ret: str="", type_: str="", num='1', tab: int=1):
        self.line(
            line='checkCudaErrors(cudaMalloc((void**)&(%s), sizeof(%s)*%s))' %
                 (ret, type_, num),
            tab=tab
        )

    def to_gpu(self, ret: str, cpu: str, type_:str= 'double', num='1', tab: int=1):
        self.malloc_gpu(ret=ret, type_=type_, num=num, tab=tab)
        self.cpu_to_gpu(gpu=ret, cpu=cpu, type_=type_, num=num, tab=tab)

    def from_gpu(self, ret: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.malloc(ret=ret, type_=type_, num=num, tab=tab)
        self.gpu_to_cpu(gpu=gpu, cpu=ret, type_=type_, num=num, tab=tab)

    def cpu_to_gpu(self, cpu: str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.line(
            line='checkCudaErrors(cudaMemcpy(%s, %s, sizeof(%s)*%s, cudaMemcpyHostToDevice))' %
                 (gpu, cpu, type_, num),
            tab=tab
        )

    def gpu_to_cpu(self, cpu:str, gpu: str, type_:str= 'double', num='1', tab: int=1):
        self.line(
            line='checkCudaErrors(cudaMemcpy(%s, %s, sizeof(%s)*%s, cudaMemcpyDeviceToHost))' %
                 (cpu, gpu, type_, num),
            tab=tab
        )
