
from bsim.model import BaseModel
from bsim.model_compiler import compile_


class NeuronModel(BaseModel):
    def __init__(self, computation: str='', threshold: str='v > vt', reset: str='v = vr', refract: bool=True, name: str=''):
        """
        Create NeuronModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.threshold = threshold
        self.reset = reset
        self.refract = refract

        self.expressions, self.parameters = compile_({
            'computation': computation,
        })

    def _compile_h(self):
        h_file = open("../model/%s.h" % self.name)

        h_file.write("\n\n")
        h_file.write("#ifndef %s_H\n " % self.name.upper())
        h_file.write("#define %s_H\n" % self.name.upper())
        h_file.write("\n\n")
        h_file.write('#include "../utils/BlockSize.h"')
        h_file.write("\n\n")

        h_file.write("struct %s {\n" % self.name.capitalize())
        if self.refract:
            h_file.write("\tint *p_refract_step;\n")

        for i in self.parameters['variable']:
            h_file.write("\tfloat *p_%s\n;" % i)
        h_file.write("};\n")
        h_file.write("\n")

        h_file.write('extern "C" {\n')
        h_file.write("\t void update_%s(%s *data, int num, int start_id, BlockSize *size);" % (self.name.lower(), self.name.capitalize()))
        h_file.write("}\n")
        h_file.write("\n")

        h_file.write('extern "C" {\n')
        h_file.write("\t%s * to_gpu_%s(%s *cpu, int num);\n" %
                     (self.name.lower(), self.name.capitalize(), self.name.capitalize()))
        h_file.write("}\n")
        h_file.write("\n")

        h_file.write("__global void find_%s_gpu(%s *data, int num, int start_id);\n" %
                     (self.name.lower(), self.name))
        h_file.write("\n")

        h_file.write("__global void update_%s_gpu(%s *data, int num, int start_id);\n" %
                     (self.name.lower(), self.name))
        h_file.write("\n")

        h_file.write("#endif /* " + self.name.upper() + "_H */\n")

        h_file.close()
        return

    def _compile_py(self):
        py_file = open('../model/%s.py' % self.name )

        py_file.write("from ctypes import *\n\n\n")
        py_file.write("class " + self.name.capitalize() + "(Structure):\n")
        py_file.write("\t_fields_ = [\n")
        if self.refract:
            py_file.write('("p_refract_step", POINTER(c_int),\n')

        for i in self.parameters['variable'][:-1]:
            py_file.write('("p_%s"), POINTER(c_float),\n'% i)
        py_file.write('("p_%s"), POINTER(c_float)\n' % self.parameters['variable'][-1])
        py_file.write("]\n")
        py_file.write('\n')

        py_file.close()
        return

    def _compile_c(self):
        c_file = open('../model/%s.cpp' % self.name )

        c_file.write("\n\n")
        c_file.write('#include <stdlib.h>\n')
        c_file.write("\n")
        c_file.write('#include "../gpu_utils/mem_op.h"\n')
        c_file.write("#include " + self.name + ".h\n")
        c_file.write("\n\n")

        c_file.write('extern "C" {\n')
        c_file.write("\t%s * to_gpu_%s(%s *cpu, int num)\n" %
                     (self.name.lower(), self.name.capitalize(), self.name.capitalize()))
        c_file.write("\t{\n")
        c_file.write('\t\t%s * ret = malloc(sizeof(%s);\n', (self.name.capitalize(), self.name.capitalize()))
        if self.refract:
            c_file.write('\t\tgpu->p_refract_step = copyToGPU<real>(cpu->p_refract_step, num);\n')

        for i in self.parameters['variable']:
            c_file.write('\t\tgpu->%s = copyToGPU<real>(cpu->%s, num);\n', (i, i))

        c_file.write('\n\t\treturn ret;\n')
        c_file.write("\t}\n")
        c_file.write("}\n")
        c_file.write("\n")

        c_file.close()
        return

    def _compile_cu(self):
        cu_file = open('../model/%s.cu' % self.name)

        cu_file.write("\n")
        cu_file.write('#include "../../gpu_utils/runtime.h"\n')
        cu_file.write('\n')
        cu_file.write('#include "%s.h"\n' % self.name)
        cu_file.write('\n\n')

        cu_file.write('extern "C" {\n')
        cu_file.write("\tvoid update_%s(%s *data, int num, int start_id, BlockSize *size)\n" %
                      (self.name.lower(), self.name.capitalize()))
        cu_file.write("\t{\n")
        cu_file.write("\t\tfind_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id);\n" %
                      (self.name.lower(), self.name.capitalize()))
        cu_file.write("\t\tupdate_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id)\n;" %
                      (self.name.lower(), self.name.capitalize()))
        cu_file.write("\t}\n")
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.write("__global void find_%s_gpu(%s *data, int num, int start_id)\n" %
                      (self.name.lower(), self.name))
        cu_file.write("{\n")

        cu_file.write("\t__shared__ int active_table_t[MAXBLOCKSIZE];\n")
        cu_file.write("\t__shared__ volatile int active_cnt;\n")
        cu_file.write("\n")

        cu_file.write("\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\tactive_cnt = 0;\n")
        cu_file.write("\t}\n")
        cu_file.write("\t__syncthreads();\n")
        cu_file.write("\n")

        cu_file.write("\tint tid = blockIdx.x * blockDim.x + threadIdx.x;\n")
        cu_file.write("\tfor (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {\n")
        cu_file.write("\t\tint test_loc = 0;\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tbool actived = data->p_refrac_step[idx] <= 0;\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tif (actived) {\n")
        cu_file.write("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);\n")
        cu_file.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\t\tactive_table_t[test_loc] = idx;\n")
        cu_file.write("\t\t\t\tactived = false;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t} else {\n")
        cu_file.write("\t\t\tgNeuronInput[start_id + idx] = 0;\n")
        cu_file.write("\t\t\tgNeuronInput_I[start_id + idx] = 0;\n")
        cu_file.write("\t\t\tdata->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tif (active_cnt >= MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\tcommit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tactive_cnt = 0;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tif (actived) {\n")
        cu_file.write("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);\n")
        cu_file.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\t\tactive_table_t[test_loc] = idx;\n")
        cu_file.write("\t\t\t\tactived = false;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\tif (active_cnt >= MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\tcommit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tactive_cnt = 0;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\tif (active_cnt > 0) {\n")
        cu_file.write("\t\t\tcommit2globalTable(active_table_t, active_cnt, gActiveTable, &gActiveTableSize, 0);\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tactive_cnt = 0;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t}\n")
        cu_file.write("\t\n")
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.write("__global void update_%s_gpu(%s *data, int num, int start_id)\n" %
                      (self.name.lower(), self.name))
        cu_file.write("{\n")
        cu_file.write("\tfor (int idx = tid; idx < gActiveTableSize; idx +=blockDim.x*gridDim.x) {\n")
        cu_file.write("\t\tbool fired = false;\n")
        cu_file.write("\t\tint test_loc = 0;\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tint nid = gActiveTable[idx];\n")
        cu_file.write("\t\tint gnid = start_id + nid; \n")
        cu_file.write("\t\t\n")

        cu_file.write("\t\tdata->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tdata->p_i_E[nid] *= data->p_CE[nid];\n")
        cu_file.write("\t\tdata->p_i_I[nid] *= data->p_CI[nid];\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tfired = data->p_vm[nid] >= data->p_v_thresh[nid];\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tgFireCount[gnid] += fired;\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tif (fired) {\n")
        cu_file.write("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);\n")
        cu_file.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\t\tfire_table_t[test_loc] = gnid;\n")
        cu_file.write("\t\t\t\tfired = false;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t\t\n")
        cu_file.write("\t\t\tdata->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;\n")
        cu_file.write("\t\t\tdata->p_vm[nid] = data->p_v_reset[nid];\n")
        cu_file.write("\t\t} else {\n")
        cu_file.write("\t\t\tgXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];\n")
        cu_file.write("\t\t\tdata->p_i_E[nid] += gNeuronInput[gnid];\n")
        cu_file.write("\t\t\tdata->p_i_I[nid] += gNeuronInput_I[gnid];\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tgNeuronInput[gnid] = 0;\n")
        cu_file.write("\t\tgNeuronInput_I[gnid] = 0;\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\t\n")

        cu_file.write("\t\tif (fire_cnt >= MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\tcommit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tfire_cnt = 0;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\t\n")

        cu_file.write("\t\tif (fired) {\n")
        cu_file.write("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);\n")
        cu_file.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\t\tfire_table_t[test_loc] = gnid;\n")
        cu_file.write("\t\t\t\tfired = false;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\tif (fire_cnt >= MAXBLOCKSIZE) {\n")
        cu_file.write("\t\t\tcommit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tfire_cnt = 0;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t\tif (fire_cnt > 0) {\n")
        cu_file.write("\t\t\tcommit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tfire_cnt = 0;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t\n")
        cu_file.write("\t}\n")
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.close()
        return
