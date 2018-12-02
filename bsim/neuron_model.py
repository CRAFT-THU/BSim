
import os

from bsim.model import Model
from bsim.model_compiler import compile_
from bsim.generator import CGenerator, CUDAGenerator, PyGenerator


class NeuronModel(Model):
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

        self.dir = os.path.dirname(__file__)

    def generate_h(self):
        h_gen = CGenerator("{}/c_code/{}.h".format(self.dir, self.name))

        h_gen.blank_line(2)
        h_gen.if_define("{}_H\n ".format(self.name.upper()))
        h_gen.blank_line(2)
        h_gen.include("../utils/BlockSize.h")
        h_gen.blank_line(2)

        h_gen.struct(self.name.capitalize())
        if self.refract:
            h_gen.line("int *p_refract_step")

        for i in self.parameters['variable']:
            h_gen.line("float *p_%s" % i)
        h_gen.line("}")
        h_gen.blank_line(1)

        h_gen.line_no_end('extern "C" {')
        h_gen.line("void update_{}({} *data, int num, int start_id, BlockSize *size)"
                   .format(self.name.lower(), self.name.capitalize()))
        h_gen.close_brace()
        h_gen.blank_line(1)

        h_gen.line_no_end('extern "C" {')
        h_gen.line("{} * to_gpu_{}({} *cpu, int num)"
                   .format(self.name.lower(), self.name.capitalize(), self.name.capitalize()))
        h_gen.close_brace()
        h_gen.blank_line(1)

        h_gen.line("__global void find_{}_gpu({} *data, int num, int start_id)"
                   .format(self.name.lower(), self.name))
        h_gen.blank_line(1)

        h_gen.line("__global void update_{}_gpu({} *data, int num, int start_id)"
                   .format(self.name.lower(), self.name))
        h_gen.blank_line(1)

        h_gen.end_if_define("{}_H".format(self.name.upper()))

        h_gen.close()
        return

    def generate_py(self):
        py_gen = PyGenerator('{}/c_code/{}.py'.format(self.dir, self.name))

        py_gen.import_("*", "ctypes")
        py_gen.class_(self.name.capitalize(), "Structure")
        py_gen.line("_fields_ = [")
        if self.refract:
            py_gen.line('("p_refract_step", POINTER(c_int)),', 2)

        for i in self.parameters['variable'][:-1]:
            py_gen.line('("p_{}", POINTER(c_float)),'.format(i), 2)
        py_gen.line('("p_{}", POINTER(c_float))'.format(self.parameters['variable'][-1]), 2)
        py_gen.line("]")
        py_gen.blank_line()

        py_gen.close()
        return

    def generate_data_cu(self, debug=False):
        c_gen = CUDAGenerator('./c_code/%s.cu' % self.name)

        c_gen.blank_line(2)
        c_gen.write('#include <stdlib.h>\n')
        c_gen.write("\n")
        c_gen.write('#include "../gpu_utils/mem_op.h"\n')
        c_gen.write("#include " + self.name + ".h\n")
        c_gen.write("\n\n")

        c_gen.write('extern "C" {\n')
        c_gen.write("\t%s * to_gpu_%s(%s *cpu, int num)\n" %
                     (self.name.lower(), self.name.capitalize(), self.name.capitalize()))
        c_gen.write("\t{\n")
        c_gen.write('\t\t%s * gpu = malloc(sizeof(%s);\n', (self.name.capitalize(), self.name.capitalize()))
        if self.refract:
            c_gen.write('\t\tgpu->p_refract_step = copyToGPU<real>(cpu->p_refract_step, num);\n')

        for i in self.parameters['variable']:
            c_gen.write('\t\tgpu->%s = copyToGPU<real>(cpu->%s, num);\n', (i, i))

        c_gen.write('\t\t%s * ret = copyToGPU<%s>(gpu, 1);\n', (self.name.capitalize(), self.name.capitalize()))

        c_gen.write('\n\t\treturn ret;\n')
        c_gen.write("\t}\n")
        c_gen.write("}\n")
        c_gen.write("\n")

        c_gen.close()
        return

    def generate_compute_cu(self, debug=False):
        cu_gen = open('./c_code/%s.cu' % self.name)

        cu_gen.write("\n")
        cu_gen.write('#include "../../gpu_utils/runtime.h"\n')
        cu_gen.write('\n')
        cu_gen.write('#include "%s.h"\n' % self.name)
        cu_gen.write('\n\n')

        cu_gen.write('extern "C" {\n')
        cu_gen.write("\tvoid update_%s(%s *data, int num, int start_id, BlockSize *size)\n" %
                      (self.name.lower(), self.name.capitalize()))
        cu_gen.write("\t{\n")
        cu_gen.write("\t\tfind_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id);\n" %
                      (self.name.lower(), self.name.capitalize()))
        cu_gen.write("\t\tupdate_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id)\n;" %
                      (self.name.lower(), self.name.capitalize()))
        cu_gen.write("\t}\n")
        cu_gen.write("}\n")
        cu_gen.write("\n")

        cu_gen.write("__global void find_%s_gpu(%s *data, int num, int start_id)\n" %
                      (self.name.lower(), self.name))
        cu_gen.write("{\n")

        cu_gen.write("\t__shared__ int active_table_t[MAXBLOCKSIZE];\n")
        cu_gen.write("\t__shared__ volatile int active_cnt;\n")
        cu_gen.write("\n")

        cu_gen.write("\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\tactive_cnt = 0;\n")
        cu_gen.write("\t}\n")
        cu_gen.write("\t__syncthreads();\n")
        cu_gen.write("\n")

        cu_gen.write("\tint tid = blockIdx.x * blockDim.x + threadIdx.x;\n")
        cu_gen.write("\tfor (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {\n")
        cu_gen.write("\t\tint test_loc = 0;\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tbool actived = data->p_refrac_step[idx] <= 0;\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tif (actived) {\n")
        cu_gen.write("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);\n")
        cu_gen.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\t\tactive_table_t[test_loc] = idx;\n")
        cu_gen.write("\t\t\t\tactived = false;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t} else {\n")
        cu_gen.write("\t\t\tgNeuronInput[start_id + idx] = 0;\n")
        cu_gen.write("\t\t\tgNeuronInput_I[start_id + idx] = 0;\n")
        cu_gen.write("\t\t\tdata->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tif (active_cnt >= MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\tcommit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);\n")
        cu_gen.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\t\t\tactive_cnt = 0;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tif (actived) {\n")
        cu_gen.write("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);\n")
        cu_gen.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\t\tactive_table_t[test_loc] = idx;\n")
        cu_gen.write("\t\t\t\tactived = false;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\tif (active_cnt >= MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\tcommit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);\n")
        cu_gen.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\t\t\tactive_cnt = 0;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\tif (active_cnt > 0) {\n")
        cu_gen.write("\t\t\tcommit2globalTable(active_table_t, active_cnt, gActiveTable, &gActiveTableSize, 0);\n")
        cu_gen.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\t\t\tactive_cnt = 0;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t}\n")
        cu_gen.write("\t\n")
        cu_gen.write("}\n")
        cu_gen.write("\n")

        cu_gen.write("__global void update_%s_gpu(%s *data, int num, int start_id)\n" %
                      (self.name.lower(), self.name))
        cu_gen.write("{\n")
        cu_gen.write("\tfor (int idx = tid; idx < gActiveTableSize; idx +=blockDim.x*gridDim.x) {\n")
        cu_gen.write("\t\tbool fired = false;\n")
        cu_gen.write("\t\tint test_loc = 0;\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tint nid = gActiveTable[idx];\n")
        cu_gen.write("\t\tint gnid = start_id + nid; \n")
        cu_gen.write("\t\t\n")

        cu_gen.write("\t\tdata->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tdata->p_i_E[nid] *= data->p_CE[nid];\n")
        cu_gen.write("\t\tdata->p_i_I[nid] *= data->p_CI[nid];\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tfired = data->p_vm[nid] >= data->p_v_thresh[nid];\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tgFireCount[gnid] += fired;\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tif (fired) {\n")
        cu_gen.write("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);\n")
        cu_gen.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\t\tfire_table_t[test_loc] = gnid;\n")
        cu_gen.write("\t\t\t\tfired = false;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t\t\n")
        cu_gen.write("\t\t\tdata->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;\n")
        cu_gen.write("\t\t\tdata->p_vm[nid] = data->p_v_reset[nid];\n")
        cu_gen.write("\t\t} else {\n")
        cu_gen.write("\t\t\tgXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];\n")
        cu_gen.write("\t\t\tdata->p_i_E[nid] += gNeuronInput[gnid];\n")
        cu_gen.write("\t\t\tdata->p_i_I[nid] += gNeuronInput_I[gnid];\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tgNeuronInput[gnid] = 0;\n")
        cu_gen.write("\t\tgNeuronInput_I[gnid] = 0;\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\t\n")

        cu_gen.write("\t\tif (fire_cnt >= MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\tcommit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);\n")
        cu_gen.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\t\t\tfire_cnt = 0;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\t\n")

        cu_gen.write("\t\tif (fired) {\n")
        cu_gen.write("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);\n")
        cu_gen.write("\t\t\tif (test_loc < MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\t\tfire_table_t[test_loc] = gnid;\n")
        cu_gen.write("\t\t\t\tfired = false;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\tif (fire_cnt >= MAXBLOCKSIZE) {\n")
        cu_gen.write("\t\t\tcommit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);\n")
        cu_gen.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\t\t\tfire_cnt = 0;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t__syncthreads();\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t\tif (fire_cnt > 0) {\n")
        cu_gen.write("\t\t\tcommit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);\n")
        cu_gen.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_gen.write("\t\t\t\tfire_cnt = 0;\n")
        cu_gen.write("\t\t\t}\n")
        cu_gen.write("\t\t}\n")
        cu_gen.write("\t\t\n")
        cu_gen.write("\t}\n")
        cu_gen.write("}\n")
        cu_gen.write("\n")

        cu_gen.close()
        return
