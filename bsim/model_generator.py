
from generator import *

C_TYPE_SORT = {
    'char' : 0,
    'unsigned char' : 1,
    'short' : 2,
    'unsigned short' : 3,
    'int' : 4,
    'unsigned int' : 5,
    'float' : 6,
    'long' : 7,
    'unsigned long': 8,
    'long long' : 9,
    'unsigned long long': 10,
    'double' : 11,
    'long double' : 12
}

def mycap(v):
    return v[0].upper() + v[1:]

def myhash(v):
    if v in C_TYPE_SORT:
        return C_TYPE_SORT[v]
    else:
        return len(C_TYPE_SORT) + abs(hash(v))

class Data(object):
    def __init__(self, name, parameters, path='./', pre='', post='Data',
            headers=[], cu_headers=[]):
        self.name = mycap(name);
        self.classname = "{}{}{}".format(pre, name, post)
        self.path = path
        self.headers = headers
        self.cu_headers = cu_headers
        self.parameters = {k:parameters[k] for k in sorted(parameters.keys(), key= lambda x:myhash(x), reverse = False)}

    def generate(self):
        self.generate_h()
        self.generate_c()
        self.generate_cu()
        self.generate_mpi()

    def generate_h(self):
        h = HGenerator("{}/{}.h".format(self.path, self.classname))

        h.include_std("stdio.h")
        h.blank_line()
        h.include("../../net/Connection.h")
        h.blank_line()
        for i in self.headers:
            h.include(i)
        # h.include("../../utils/macros.h")
        h.blank_line()

        h.struct(self.classname, 0)
        for k in self.parameters:
            for v in self.parameters[k]:
                h.line("{} *p{}".format(k, mycap(v)))
            h.blank_line()
        h.struct_end()
        h.blank_line()

        h.blank_line()
        h.func("size_t get{}Size()".format(self.name))
        h.func("void *malloc{}()".format(self.name))
        h.func("void *alloc{}(int num)".format(self.name))
        h.func("int alloc{}Para(void *pCPU, int num)".format(self.name))
        h.func("int free{}(void *pCPU)".format(self.name))
        h.func("int free{}Para(void *pCPU)".format(self.name))
        h.func("int save{}(void *pCPU, int num, FILE *f)".format(self.name))
        h.func("void *load{}(int num, FILE *f)".format(self.name))
        h.func("bool isEqual{}(void *p1, void *p2, int num)".format(self.name))
        h.blank_line()

        h.func("void *cudaMalloc{}()".format(self.name))
        h.func("void *cudaAlloc{}(void *pCPU, int num)".format(self.name))
        h.func("void *cudaAlloc{}Para(void *pCPU, int num)".format(self.name))
        h.func("int cudaFree{}(void *pGPU)".format(self.name))
        h.func("int cudaFree{}Para(void *pGPU)".format(self.name))
        h.func("int cudaFetch{}(void *pCPU, void *pGPU, int num)".format(self.name))
        h.func("int cuda{}ParaToGPU(void *pCPU, void *pGPU, int num)".format(self.name))
        h.func("int cuda{}ParaFromGPU(void *pCPU, void *pGPU, int num)".format(self.name))
        h.func("void cudaUpdate{}(Connection *conn, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize)".format(self.name))
        h.blank_line()

        h.func("int mpiSend{}(void *data, int rank, int offset, int size)".format(self.name))
        h.func("int mpiRecv{}(void **data, int rank, int size)".format(self.name))
        h.blank_line()

        h.close()
        return 0

    def generate_c(self):
        c = CGenerator("{}/{}.cpp".format(self.path, self.classname))
        c.include_std("stdlib.h")
        c.include_std("string.h")
        c.blank_line()
        c.include("../../utils/utils.h")
        c.blank_line()
        c.blank_line()
        c.include("{}.h".format(self.classname))
        c.blank_line()

        c.func_start("size_t get{}Size()".format(self.name))
        c.func_end("sizeof({})".format(self.classname))
        c.blank_line()

        c.func_start("void *malloc{}()".format(self.name))
        c.malloc("p", "{}".format(self.classname), 1)
        c.func_end("(void*)p")
        c.blank_line()

        c.func_start("int alloc{}Para(void *pCPU, int num)".format(self.name))
        c.line("{} *p = ({}*)pCPU".format(self.classname, self.classname))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("p->p{} = ({}*)malloc(num*sizeof({}))".format(mycap(p), t, t))
                c.line("memset(p->p{}, 0, num*sizeof({}))".format(mycap(p), t))
            c.blank_line()
        c.func_end(0)
        c.blank_line()

        c.func_start("void *alloc{}(int num)".format(self.name))
        c.line("void *p = malloc{}()".format(self.name))
        c.line("alloc{}Para(p, num)".format(self.name, self.name, self.name))
        c.func_end("p")
        c.blank_line()

        c.func_start("int free{}Para(void *pCPU)".format(self.name))
        c.line("{} *p = ({}*)pCPU".format(self.classname, self.classname))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.free("p->p{}".format(mycap(p)))
            c.blank_line()
        c.func_end("0")
        c.blank_line()

        c.func_start("int free{}(void *pCPU)".format(self.name))
        c.line("{} *p = ({}*)pCPU".format(self.classname, self.classname))
        c.blank_line()
        c.line("free{}Para(p)".format(self.name))
        c.free("p")
        c.func_end("0")
        c.blank_line()

        c.func_start("int save{}(void *pCPU, int num, FILE *f)".format(self.name))
        c.blank_line()
        c.line("{} *p = ({}*)pCPU".format(self.classname, self.classname))
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("fwrite(p->p{}, sizeof({}), num, f)".format(mycap(p), t))
            c.blank_line()
        c.func_end("0")
        c.blank_line()

        c.func_start("void *load{}(int num, FILE *f)".format(self.name))
        c.line("{} *p = ({}*)alloc{}(num)".format(self.classname, self.classname, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("fread(p->p{}, sizeof({}), num, f)".format(mycap(p), t))
            c.blank_line()
        c.func_end("p")
        c.blank_line()

        c.func_start("bool isEqual{}(void *p1, void *p2, int num)".format(self.name))
        c.line("{} *t1 = ({}*)p1".format(self.classname, self.classname))
        c.line("{} *t2 = ({}*)p2".format(self.classname, self.classname))
        c.blank_line()
        c.line("bool ret = true")
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("ret = ret && isEqualArray(t1->p{}, t2->p{}, num)".format(mycap(p), mycap(p)))
            c.blank_line()
        c.func_end("ret")
        c.blank_line()

        return 0

    def generate_cu(self):
        cu = CUDAGenerator("{}/{}.cu".format(self.path, self.classname)) 
        cu.include_std("stdlib.h")
        cu.include_std("string.h")
        for i in self.cu_headers:
            cu.include(i)
        cu.include("{}.h".format(self.classname))
        cu.blank_line()

        cu.func_start("void *cudaMalloc{}()".format(self.name))
        cu.line("void *ret = NULL")
        cu.malloc_gpu("ret", "{}".format(self.classname), 1)
        cu.func_end("ret")
        cu.blank_line()

        cu.func_start("void *cudaAlloc{}(void *pCPU, int num)".format(self.name))
        cu.line("void *ret = cudaMalloc{}()".format(self.name))
        cu.line("void *tmp = cudaAlloc{}Para(pCPU, num)".format(self.name))
        cu.cpu_to_gpu(gpu="ret", cpu="tmp", type_="{}".format(self.classname), num=1)
        cu.free("tmp")
        cu.func_end("ret")
        cu.blank_line()

        cu.func_start("void *cudaAlloc{}Para(void *pCPU, int num)".format(self.name))
        cu.line("{} *p = ({}*)pCPU".format(self.classname, self.classname))
        cu.malloc("ret", "{}".format(self.classname), 1)
        cu.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                cu.to_gpu("ret->p{}".format(mycap(p)),
                          cpu="p->p{}".format(mycap(p)),
                          type_=t, num="num");
            cu.blank_line()
        cu.func_end("ret")
        cu.blank_line()

        cu.func_start("int cudaFetch{}(void *pCPU, void *pGPU, int num)".format(self.name))
        cu.malloc("pTmp", "{}".format(self.classname), 1)
        cu.gpu_to_cpu(cpu="pTmp", gpu="pGPU", type_="{}".format(self.classname), num=1)
        cu.blank_line()
        cu.line("cuda{}ParaFromGPU(pCPU, pTmp, num)".format(self.name))
        cu.func_end(0)
        cu.blank_line()

        cu.func_start("int cuda{}ParaToGPU(void *pCPU, void *pGPU, int num)".format(self.name))
        cu.line("{} *pC = ({}*)pCPU".format(self.classname, self.classname))
        cu.line("{} *pG = ({}*)pGPU".format(self.classname, self.classname))
        cu.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                cu.cpu_to_gpu(cpu="pC->p{}".format(mycap(p)), gpu="pG->p{}".format(mycap(p)), type_=t, num="num")
            cu.blank_line()
        cu.func_end(0)
        cu.blank_line()

        cu.func_start("int cuda{}ParaFromGPU(void *pCPU, void *pGPU, int num)".format(self.name))
        cu.line("{} *pC = ({}*)pCPU".format(self.classname, self.classname))
        cu.line("{} *pG = ({}*)pGPU".format(self.classname, self.classname))
        cu.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                cu.gpu_to_cpu(cpu="pC->p{}".format(mycap(p)), gpu="pG->p{}".format(mycap(p)), type_=t, num="num")
            cu.blank_line()
        cu.func_end(0)
        cu.blank_line()

        cu.func_start("int cudaFree{}(void *pGPU)".format(self.name))
        cu.from_gpu("tmp", gpu="pGPU", type_="{}".format(self.classname), num=1)
        cu.line("cudaFree{}Para(tmp)".format(self.name))
        cu.free("tmp")
        cu.free_gpu("pGPU")
        cu.func_end(0)
        cu.blank_line()

        cu.func_start("int cudaFree{}Para(void *pGPU)".format(self.name))
        cu.line("{} *p = ({}*)pGPU".format(self.classname, self.classname))
        for t in self.parameters:
            for p in self.parameters[t]:
                cu.free_gpu("p->p{}".format(mycap(p)))
            cu.blank_line()
        cu.func_end(0)
        cu.blank_line()

        cu.close()
        return 0

    def generate_mpi(self):
        c = CGenerator("{}/{}.mpi.cpp".format(self.path, self.classname))
        c.include("mpi.h")
        c.blank_line()
        c.include("{}.h".format(self.classname))
        c.blank_line()

        c.func_start("int mpiSend{}(void *data, int rank, int offset, int size)".format(self.name))
        c.func_end("0")
        c.blank_line()

        c.func_start("int mpiRecv{}(void **data, int rank, int size)".format(self.name))
        c.func_end("0")
        c.blank_line()

        return 0

if __name__ == '__main__':
    parameters_old = {
                    'refracStep' : 'int',
                    'refracTime' : 'int',
                    'v_m' : 'real',
                    'ci' : 'real',
                    'ce' : 'real',
                    'c_i' : 'real',
                    'c_e' : 'real',
                    'v_tmp' : 'real',
                    'i_i' : 'real',
                    'i_e' : 'real',
                    'v_i' : 'real',
                    'v_e' : 'real',
                    'v_thresh' : 'real',
                    'v_reset' : 'real',
                    'c_m' : 'real',
                }
    parameters = {}
    for k,v in parameters_old.items():
        parameters.setdefault(v, []).append(k)

    lif = Data('LIF', parameters, 
            path='../src/neuron/lif/', pre='', post='Data', 
            headers=['../../utils/type.h', '../../utils/BlockSize.h'], 
            cu_headers=['../../third_party/cuda/helper_cuda.h'])
    lif.generate()

    static = Data('Static', {'int':['dst'], 'real':['weight']},
            path='../src/synapse/static/', pre='', post='Data',
            headers=['../../utils/type.h', '../../utils/BlockSize.h'], 
            cu_headers=['../../third_party/cuda/helper_cuda.h'])
    static.generate()

