#!/usr/bin/python

import os

def generate_h_file(paras, type_name, type_type, path_name):
    obj_type = "G" + type_name + type_type

    filename = os.path.join(path_name, obj_type + ".h")
    f = open(filename, "w+")

    f.write("#ifndef "+ obj_type.upper() + "_H\n")
    f.write("#define "+ obj_type.upper() + "_H\n")
    f.write("\n")
    f.write('#include "../base/type.h"\n')
    f.write('#include "../utils/macros.h"\n')
    f.write('\n')

    f.write("struct " + obj_type + " {\n")
    for t in paras:
        for para in paras[t]:
            f.write("\t" + t + " *p_" + para + ";\n")

    if type_type == "Synapses":
        f.write("\tint *pDst;\n")


    f.write('};\n')
    f.write('\n')
    
    f.write(type_type.upper()[:-1] + '_GPU_FUNC_DEFINE(' + type_name + ')\n')
    f.write('\n')

    f.write("#endif /* " + obj_type.upper() + "_H */\n")

    f.close()


def generate_cpp_file(paras, type_name, type_type, path_name):
    obj_type = "G" + type_name + type_type

    filename = os.path.join(path_name, obj_type + ".cpp")
    f = open(filename, "w+")

    f.write("\n")
    f.write('#include <stdlib.h>\n')
    f.write('#include "mpi.h"\n')
    f.write('#include "../utils/macros.h"\n')
    f.write('#include "../utils/TagPool.h"\n')
    f.write('#include "' + obj_type +'.h"\n')

    f.write('\n' +  type_type.upper()[:-1] + '_GPU_FUNC_BASIC(' + type_name + ')\n\n')

    f.write("int alloc" + type_name + "(void *pCpu, int N)\n")
    f.write("{\n")
    f.write("\t" + obj_type + " *p = " + "(" + obj_type + "*)pCpu;\n")

    for t in paras:
        for para in paras[t]:
            f.write("\tp->p_" + para + " = (" + t + "*)malloc(N*sizeof(" + t + "));\n")
        
    if type_type == "Synapses":
        f.write("\tp->pDst = (int *)malloc(N*sizeof(int));\n")

    f.write("\treturn 0;\n")
    f.write("}\n\n")

    f.write("int free" + type_name + "(void *pCpu)\n")
    f.write("{\n")
    f.write("\t" + obj_type + " *pCpu" + type_type + " = " + "(" + obj_type + "*)pCpu;\n")

    for t in paras:
        for para in paras[t]:
            f.write("\t" + "free(" + "pCpu" +  type_type + "->p_" + para + ");\n")

    if type_type == "Synapses":
        f.write("\t" + "free(" + "pCpu" +  type_type + "->pDst);\n")
        

    f.write("\treturn 0;\n")
    f.write("}\n\n")

    f.write("void mpiSend" + type_name + "(void *data, int rank, int offset, int size)\n")
    f.write("{\n")
    f.write("}\n\n")

    f.write("void mpiRecv" + type_name + "(void **data, int rank, int rankSize)\n")
    f.write("{\n")
    f.write("}\n\n")

    f.close()
    

def generate_cu_file(paras, type_name, type_type, path_name):
    obj_type = "G" + type_name + type_type

    filename = os.path.join(path_name, obj_type + ".cu")
    f = open(filename, "w+")

    f.write("\n")
    f.write('#include "../third_party/cuda/helper_cuda.h"\n')
    f.write('#include "../gpu_utils/mem_op.h"\n')
    f.write('#include "' + obj_type +'.h"\n')
    f.write("\n")

    f.write("int cudaAlloc" + type_name + "(void *pCpu, void *pGpu, int num)\n")
    f.write("{\n")
    f.write("\t" + obj_type + " *pGpu" + type_type + " = " + "(" + obj_type + "*)pGpu;\n")
    f.write("\t" + obj_type + " *p = " + "(" + obj_type + "*)pCpu;\n")

    for t in paras:
        for para in paras[t]:
            f.write("\t" + "pGpu" +  type_type + "->p_" + para + " = copyToGPU<" + t + ">(p->p_" + para + ", num);\n")

    if type_type == "Synapses":
        f.write("\t" + "pGpu" +  type_type + "->pDst = copyToGPU<int>(p->pDst, num);\n")
        

    f.write("\treturn 0;\n")
    f.write("}\n\n")

    f.write("int cudaFree" + type_name + "(void *pGpu)\n")
    f.write("{\n")
    f.write("\t" + obj_type + " *pGpu" + type_type + " = " + "(" + obj_type + "*)pGpu;\n")

    for t in paras:
        for para in paras[t]:
            f.write("\t" + "gpuFree(" + "pGpu" +  type_type + "->p_" + para + ");\n")
        
    if type_type == "Synapses":
        f.write("\t" + "gpuFree(" + "pGpu" +  type_type + "->pDst);\n")

    f.write("\treturn 0;\n")
    f.write("}\n\n")

    f.close()

