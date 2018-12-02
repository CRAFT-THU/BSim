
from bsim.model import BaseModel
from bsim.model_compiler import compile_


class SynapseModel(BaseModel):
    def __init__(self, computation: str='', pre: str='', post: str='', name: str=''):
        """
        Create SynapseModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.pre_learn = not (pre == '')
        self.post_learn = not (post == '')

        self.expressions, self.parameters = compile_({
            'computation': computation,
            'pre': pre,
            'post': post
        })

        self.parameters['original'].add('weight')
        self.parameters['original'].add('delay')
        self.parameters['variable'].add('weight')
        self.parameters['constant'].add('delay')

    def _generate_h(self):
        h_file = open("./c_code/%s.h" % self.name)

        h_file.write("\n\n")
        h_file.write("#ifndef %s_H\n " % self.name.upper())
        h_file.write("#define %s_H\n" % self.name.upper())
        h_file.write("\n\n")
        h_file.write('#include "../utils/BlockSize.h"')
        h_file.write("\n\n")

        h_file.write("struct %s {\n" % self.name.capitalize())
        h_file.write("\tint *p_dst;\n")

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

        #h_file.write("__global void find_%s_gpu(%s *neuron_data, int num, int start_id);\n" %
        #             (self.name.lower(), self.name))
        #h_file.write("\n")

        h_file.write("__global void update_%s_gpu(%s *data, int num, int start_id);\n" %
                     (self.name.lower(), self.name))
        h_file.write("\n")

        if self.post_learn:
            h_file.write("__global void learn_%s_post(%s *data, int num, int start_id);\n" %
                         (self.name.lower(), self.name))
            h_file.write("\n")

        h_file.write("#endif /* " + self.name.upper() + "_H */\n")

        h_file.close()
        return

    def _generate_py(self):
        py_file = open('./c_code/%s.py' % self.name )

        py_file.write("from ctypes import *\n\n\n")
        py_file.write("class " + self.name.capitalize() + "(Structure):\n")
        py_file.write("\t_fields_ = [\n")
        py_file.write('("p_dst", POINTER(c_int),\n')

        for i in self.parameters['variable'][:-1]:
            py_file.write('("p_%s"), POINTER(c_float),\n' % i)
        py_file.write('("p_%s"), POINTER(c_float)\n' % self.parameters['variable'][-1])
        py_file.write("]\n")
        py_file.write('\n')

        py_file.close()
        return

    def _generate_c(self):
        c_file = open('./c_code/%s.cpp' % self.name )

        c_file.write("\n\n")
        c_file.write('#include <stdlib.h>\n')
        c_file.write("\n")
        c_file.write('#include "../gpu_utils/mem_op.h"\n')
        c_file.write('#include "%s.h"\n' % self.name)
        c_file.write("\n\n")

        c_file.write('extern "C" {\n')
        c_file.write("\t%s * to_gpu_%s(%s *cpu, int num)\n" %
                     (self.name.lower(), self.name.capitalize(), self.name.capitalize()))
        c_file.write("\t{\n")
        c_file.write('\t\t%s * ret = malloc(sizeof(%s);\n', (self.name.capitalize(), self.name.capitalize()))
        if self.refract:
            c_file.write('\t\tgpu->p_dst = copyToGPU<real>(cpu->p_dst, num);\n')

        for i in self.parameters['variable']:
            c_file.write('\t\tgpu->%s = copyToGPU<real>(cpu->%s, num);\n', (i, i))

        c_file.write('\n\t\treturn ret;\n')
        c_file.write("\t}\n")
        c_file.write("}\n")
        c_file.write("\n")

        c_file.close()
        return

    def _generate_cu(self):
        cu_file = open('./c_code/%s.cu' % self.name)

        cu_file.write('extern "C" {\n')
        cu_file.write("\tvoid update_%s(%s *data, int num, int start_id, BlockSize *size)\n" %
                      (self.name.lower(), self.name.capitalize()))
        cu_file.write("\t{\n")
        # cu_file.write("\t\tfind_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id);\n" %
        #              (self.name.lower(), self.name.capitalize()))
        cu_file.write("\t\tupdate_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id)\n;" %
                      (self.name.lower(), self.name.capitalize()))
        if self.post_learn:
            cu_file.write("\t\tlearn_%s_post<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id)\n;" %
                          (self.name.lower(), self.name.capitalize()))
        cu_file.write("\t}\n")
        cu_file.write("}\n")
        cu_file.write("\n")

        cu_file.write("__global void update_%s_gpu(%s *data, int num, int start_id)\n" %
                      (self.name.lower(), self.name))
        cu_file.write("{\n")
        cu_file.write("\tfor (int delta_t = %s; delta_t<%s; delta_t++) {\n" % (0, "MAX_DELAY"))
        cu_file.write("\t\tint block_idx = blockIdx.x;\n")
        cu_file.write("\t\tint time_idx = (gCurrentIdx + %s - delta_t) % ( %s + 1);\n" % ("MAX_DELAY", "MAX_DELAY"))
        cu_file.write("\t\tint firedSize = gFiredTableSizes[time_idx];\n")
        cu_file.write("\t\tint num_per_block = (firedSize - 1) / gridDim.x + 1;\n")
        cu_file.write("\t\tint block_nums_minus_1 = (firedSize - 1) / num_per_block;\n")
        cu_file.write("\n")
        cu_file.write("\t\tint fired_size_block = 0;\n")
        cu_file.write("\t\tif (block_idx == block_nums_minus_1) {\n")
        cu_file.write("\t\tfired_size_block = firedSize - block_idx * num_per_block;\n")
        cu_file.write("\t\t} else if (block_idx < block_nums_minus_1) {\n")
        cu_file.write("\t\tfired_size_block = num_per_block;\n")
        cu_file.write("\t\t} else {\n")
        cu_file.write("\t\tfired_size_block = 0;\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\n")
        cu_file.write("\t\tfor (int idx = 0; idx < fired_size_block; idx++) {\n")
        cu_file.write("\t\t\tint nid = gFiredTable[time_idx * gFiredTableCap + (block_idx) * num_per_block + idx];\n")
        cu_file.write("\t\t\tint start_loc = gConnection->rev_delayStart[delta_t + nid * MAX_DELAY];\n")
        cu_file.write("\t\t\tint synapseNum = gConnection->rev_delayNum[delta_t + nid * MAX_DELAY];\n")
        cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
        cu_file.write("\t\t\t\tgLayerInput[nid]++;\n")
        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t\tfor (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {\n")
        cu_file.write("\t\t\t\tint sid = j+start_loc;\n")
        cu_file.write("\t\t\t\treal weight = d_synapses->p_weight[sid];\n")
        cu_file.write("\t\t\t\tif (weight >= 0) {\n")
        cu_file.write("\t\t\t\t\tatomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);\n")
        cu_file.write("\t\t\t\t} else {\n")
        cu_file.write("\t\t\t\t\tatomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);\n")
        cu_file.write("\t\t\t\t}\n")
        cu_file.write("\n")

        if (self.pre_learn):
            cu_file.write("\t\t\t\td_synapse->p_apre[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_pre[sid]));\n")
            cu_file.write("\t\t\t\td_synapse->p_apost[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_post[sid]));\n")
            cu_file.write("\n")
            cu_file.write("\t\t\t\td_synapse->p_pre[sid] += d_synapse->p_d_apre[sid];\n")
            cu_file.write("\t\t\t\td_synapse->p_weight[sid] = _clip(weight + d_synapse->p_apost[sid], %s, %s);\n" % ("gMin", "gMax"))
            cu_file.write("\t\t\t\td_synapse->p_last_update[sid] = gCurrentCycle;\n")

        cu_file.write("\t\t\t}\n")
        cu_file.write("\t\t}\n")
        cu_file.write("\t\t__syncthreads();\n")
        cu_file.write("\t}\n")
        cu_file.write("}\n")
        cu_file.write("\n")

        if self.post_learn:
            cu_file.write("__global void update_%s_gpu(%s *data, int num, int start_id)\n" %
                          (self.name.lower(), self.name))
            cu_file.write("{\n")
            cu_file.write("\tfor (int delta_t = %s; delta_t<%s; delta_t++) {\n" % (0, "MAX_DELAY"))
            cu_file.write("\t\tint block_idx = blockIdx.x;\n")

            cu_file.write("\t\tint time_idx = (gCurrentIdx + %s - delta_t) % (%s + 1);\n" % ("MAX_DELAY", "MAX_DELAY"))
            cu_file.write("\t\tint firedSize = gFiredTableSizes[time_idx];\n")
            cu_file.write("\t\tint num_per_block = (firedSize - 1) / gridDim.x + 1;\n")
            cu_file.write("\t\tint block_nums_minus_1 = (firedSize - 1) / num_per_block;\n")
            cu_file.write("\n")
            cu_file.write("\t\tint fired_size_block = 0;\n")
            cu_file.write("\t\tif (block_idx == block_nums_minus_1) {\n")
            cu_file.write("\t\tfired_size_block = firedSize - block_idx * num_per_block;\n")
            cu_file.write("\t\t} else if (block_idx < block_nums_minus_1) {\n")
            cu_file.write("\t\tfired_size_block = num_per_block;\n")
            cu_file.write("\t\t} else {\n")
            cu_file.write("\t\tfired_size_block = 0;\n")
            cu_file.write("\t\t}\n")
            cu_file.write("\n")
            cu_file.write("\t\tfor (int idx = 0; idx < fired_size_block; idx++) {\n")
            cu_file.write("\t\t\tint nid = gFiredTable[time_idx * gFiredTableCap + (block_idx) * num_per_block + idx];\n")
            cu_file.write("\t\t\tint start_loc = gConnection->rev_delayStart[delta_t + nid * MAX_DELAY];\n")
            cu_file.write("\t\t\tint synapseNum = gConnection->rev_delayNum[delta_t + nid * MAX_DELAY];\n")
            cu_file.write("\t\t\tif (threadIdx.x == 0) {\n")
            cu_file.write("\t\t\tgLayerInput[nid]++;\n")
            cu_file.write("\t\t\t}\n")
            cu_file.write("\t\t\tfor (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {\n")
            cu_file.write("\t\t\tint sid = gConnection->rev_map2sid[j+start_loc];\n")
            cu_file.write("\n")
            cu_file.write("\t\t\td_synapse->p_apre[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_pre[sid]));\n")
            cu_file.write("\t\t\td_synapse->p_apost[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_post[sid]));\n")
            cu_file.write("\t\t\td_synapse->p_apost[sid] += d_synapse->p_d_apost[sid];\n")
            cu_file.write("\t\t\td_synapse->p_weight[sid] = _clip(weight + d_synapse->p_pre[sid], %s, %s);\n" % ("gMin", "gMax"))
            cu_file.write("\t\t\td_synapse->p_last_update[sid] = gCurrentCycle;\n")
            cu_file.write("\t\t\t}\n")
            cu_file.write("\t\t}\n")
            cu_file.write("\t\t__syncthreads();\n")
            cu_file.write("\t}\n")
            cu_file.write("}\n")
            cu_file.write("\n")

        cu_file.close()
        return


