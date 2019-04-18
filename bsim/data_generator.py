
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
    def __init__(self, name, parameters, path='./'):
        self.name = mycap(name);
        self.path = path;
        self.parameters = {k:parameters[k] for k in sorted(parameters.keys(), key= lambda x:myhash(x), reverse = False)}

    def generate_h(self):
        h = HGenerator("{}/{}Data.h".format(self.path, self.name))

        h.include_std("stdio.h")
        h.blank_line()
        h.include("../../utils/type.h")
        # h.include("../../utils/macros.h")
        h.blank_line()

        h.struct("{}Data".format(self.name), 0)
        for k in self.parameters:
            for v in self.parameters[k]:
                h.line("{} *p{}".format(k, mycap(v)))
            h.blank_line(1)
        h.close_brace()
        h.blank_line(1)

        # h.line("DATA_FUNC_DEFINE({})".format(self.name), 0)
        
        h.blank_line(1)
        h.func("void *malloc{}()".format(self.name))
        h.func("void *alloc{}(int n)".format(self.name))
        h.func("int free{}(void *pCpu, int n)".format(self.name))
        h.func("int alloc{}Para(void *pCpu, int n)".format(self.name))
        h.func("int free{}Para(void *pCpu, int n)".format(self.name))
        h.func("int save{}(void *pCpu, int n, FILE *f)".format(self.name))
        h.func("int load{}(int n, FILE *f)".format(self.name))
        h.blank_line()

        h.func("void* cudaAlloc{}(void *pCpu, int num)".format(self.name))
        h.func("int cuda{}ToGPU(void *pCpu, void *pGPU, int num)".format(self.name))
        h.func("void cudaUpdate{}(void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize)".format(self.name))
        h.func("int cudaFree{}(void *pGpu)".format(self.name))
        h.blank_line()

        h.func("int mpiSend{}(void *data, int rank, int offset, int size)".format(self.name))
        h.func("int mpiRecv{}(void *data, int rank, int offset, int size)".format(self.name))
        h.blank_line()

        h.close()
        return 0

    def generate_c(self):
        c = CGenerator("{}/{}Data.cpp".format(self.path, self.name))
        c.include_std("stdlib.h")
        c.blank_line()
        # c.include("mpi.h")
        # c.include("../../utils/macros.h")
        # c.include("../../utils/TagPool.h")
        c.include("{}Data.h".format(self.name))
        c.blank_line()

        c.func_start("void *malloc{}()".format(self.name))
        c.line("void *p = malloc(sizeof({}Data))".format(self.name))
        c.func_end("p")
        c.blank_line()

        c.func_start("void *alloc{}(int n)".format(self.name))
        c.line("void *p = malloc{}()".format(self.name, self.name, self.name))
        c.line("alloc{}Para(p, n)".format(self.name, self.name, self.name))
        c.func_end("p")
        c.blank_line()

        c.func_start("int free{}(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("free(p->p{})".format(mycap(p)))
            c.blank_line()
        c.line("free(p)")
        c.func_end("0")
        c.blank_line()

        c.func_start("int alloc{}Para(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("p->p{} = ({}*)malloc(n*sizeof({}))".format(mycap(p), t, t))
            c.blank_line()
        c.func_end(0)
        c.blank_line()

        c.func_start("int free{}Para(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("free(p->p{})".format(mycap(p)))
            c.blank_line()
        c.func_end("0")
        c.blank_line()

        c.func_start("int save{}(void *pCpu, int n, FILE *f)".format(self.name))
        c.blank_line()
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("fwrite(p->p{}, sizeof({}), n, f)".format(mycap(p), t))
            c.blank_line()
        c.func_end("0")
        c.blank_line()

        c.func_start("int load{}(int n, FILE *f)".format(self.name))
        c.line("{}Data *p = ({}Data*)malloc(sizeof({}Data))".format(self.name, self.name, self.name))
        c.blank_line(1)
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("fread(p->p{}, sizeof({}), n, f)".format(mycap(p), t))
            c.blank_line(1)
        c.func_end("p")
        c.blank_line()

        return 0

    def generate_cu(self):
        cu = CuGenerator("{}/{}Data.h".format(self.path, self.name))

        h.blank_line()
        h.if_define("{}DATA_H ".format(self.name))
        h.blank_line()

        h.include("../../utils/type.h")
        h.include("../../utils/macros.h")
        h.blank_line()

        c.func_start("int alloc{}Para(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("p->p{} = ({}*)malloc(n*sizeof({}))".format(mycap(p), t, t))
            c.blank_line()
        c.func_end(0)
        c.blank_line()

        h.struct("{}Data".format(self.name), 0)
        h.blank_line(1)
        for k in sorted(self.parameters.keys(), key = lambda k:C_TYPE_SORT.get(k, 1000), reverse = False):
            for v in self.parameters[k]:
                h.line("{} *p{}".format(k, mycap(v)))
            h.blank_line(1)
        h.close_brace()
        h.blank_line(1)


        h.line("DATA_FUNC_DEFINE({})".format(self.name), 0)
        h.blank_line(1)

        h.end_if_define("{}DATA_H".format(self.name))

        h.close()
        return 0

    def generate_mpi(self):
        c = CGenerator("{}/{}Data.cpp".format(self.path, self.name))
        c.include("mpi.h")
        c.blank_line()
        # c.include("../../utils/macros.h")
        # c.include("../../utils/TagPool.h")
        c.include("{}Data.h".format(self.name))
        c.blank_line()

        c.func_start("void *malloc{}()".format(self.name))
        c.line("void *p = malloc(sizeof({}Data))".format(self.name))
        c.func_end("p")
        c.blank_line()

        c.func_start("void *alloc{}(int n)".format(self.name))
        c.line("void *p = malloc{}()".format(self.name, self.name, self.name))
        c.line("alloc{}Para(p, n)".format(self.name, self.name, self.name))
        c.func_end("p")
        c.blank_line()

        c.func_start("int free{}(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("free(p->p{})".format(mycap(p)))
            c.blank_line()
        c.line("free(p)")
        c.func_end("0")
        c.blank_line()

        c.func_start("int alloc{}Para(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("p->p{} = ({}*)malloc(n*sizeof({}))".format(mycap(p), t, t))
            c.blank_line()
        c.func_end(0)
        c.blank_line()

        c.func_start("int free{}Para(void *pCpu, int n)".format(self.name))
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        c.blank_line()
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("free(p->p{})".format(mycap(p)))
            c.blank_line()
        c.func_end("0")
        c.blank_line()

        c.func_start("int save{}(void *pCpu, int n, FILE *f)".format(self.name))
        c.blank_line()
        c.line("{}Data *p = ({}Data*)pCpu".format(self.name, self.name))
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("fwrite(p->p{}, sizeof({}), n, f)".format(mycap(p), t))
            c.blank_line()
        c.func_end("0")
        c.blank_line()

        c.func_start("int load{}(int n, FILE *f)".format(self.name))
        c.line("{}Data *p = ({}Data*)malloc(sizeof({}Data))".format(self.name, self.name, self.name))
        c.blank_line(1)
        for t in self.parameters:
            for p in self.parameters[t]:
                c.line("fread(p->p{}, sizeof({}), n, f)".format(mycap(p), t))
            c.blank_line(1)
        c.func_end("p")
        c.blank_line()

        return 0

if __name__ == '__main__':
    parameters_old = {
                    'refracStep' : 'int',
                    'refracTime' : 'int',
                    'Vm' : 'real',
                    'Ci' : 'real',
                    'Ce' : 'real',
                    'C_i' : 'real',
                    'C_e' : 'real',
                    'Vtmp' : 'real',
                    'Ii' : 'real',
                    'Ie' : 'real',
                    'Vthresh' : 'real',
                    'Vreset' : 'real',
                    'Cm' : 'real',
                }
    parameters = {}
    for k,v in parameters_old.items():
        parameters.setdefault(v, []).append(k)
    LIF = Data('LIF', parameters)

    LIF.generate_h()
    LIF.generate_c()

