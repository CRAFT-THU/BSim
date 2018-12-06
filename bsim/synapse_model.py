
import os

from bsim.generator import CUDAGenerator
from bsim.model import Model
from bsim.model_compiler import compile_


class SynapseModel(Model):
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

        self.parameters['external'] = set(('t', 'i_exec', 'i_inh'))

        self.parameters['special'] = set(('dst', 'last_update', 'delay')) \
            if self.pre_learn or self.post_learn else set(('dst', 'delay'))

        self.parameters['original'] -= self.parameters['special'] | self.parameters['external']
        self.parameters['variable'] -= self.parameters['special'] | self.parameters['external']
        self.parameters['constant'] -= self.parameters['special'] | self.parameters['external']

        self.parameters['original'].add('weight')
        self.parameters['variable'].add('weight')

        self.dir = os.path.dirname(__file__)

    def generate_compute_cu(self, debug=False):
        cu_gen = CUDAGenerator('{}/c_code/{}.cu'.format(self.dir, self.name))

        cu_gen.include("runtime.h")
        cu_gen.include("connection.h")
        cu_gen.blank_line()

        cu_gen.line("__global__ void find_{}_gpu({} *data, int num, int start_id);"
                   .format(self.name.lower(), self.name.capitalize()), 0)
        cu_gen.line("__global__ void update_{}_gpu({} *data, int num, int start_id);"
                   .format(self.name.lower(), self.name.capitalize()), 0)
        cu_gen.blank_line(1)

        cu_gen.block('extern "C" {')
        cu_gen.block("\tvoid update_{}({} *data, int num, int start_id, int t)"
                     .format(self.name.lower(), self.name.capitalize()))
        cu_gen.block("\t{")
        # cu_gen.block("\t\tfind_%s_gpu<<<size=>gridSize, size->blockSize>>>((%s*)data, num, start_id);" %
        #              (self.name.lower(), self.name.capitalize()))
        cu_gen.block("\t\tupdate_{}_gpu<<<GRIDSIZE, BLOCKSIZE>>>(({}*)data, num, start_id, t);"
                     .format(self.name.lower(), self.name.capitalize()))
        if self.post_learn:
            cu_gen.block("\t\tlearn_{}_post<<<GRIDSIZE, BLOCKSIZE>>>(({}*)data, num, start_id, t);"
                         .format(self.name.lower(), self.name.capitalize()))
        cu_gen.block("\t}")
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.block("__device__ void _clip(float a, float min, float max)")
        cu_gen.block("{")
        cu_gen.block("\tif (a < min) {")
        cu_gen.block("\t\treturn min;")
        cu_gen.block("\t} else if (a > max) {")
        cu_gen.block("\t\treturn max;")
        cu_gen.block("\t} else {")
        cu_gen.block("\t\treturn a;")
        cu_gen.block("\t}")
        cu_gen.block("}")

        cu_gen.block("__global__ void update_{}_gpu({} *data, int num, int start_id, int t)"
                     .format(self.name.lower(), self.name.capitalize()))
        cu_gen.block("{")
        cu_gen.block("\tfor (int delta_t={}; delta_t<={}; delta_t++) {{".format("MIN_DELAY", "MAX_DELAY"))
        cu_gen.block("\t\tint block_idx = blockIdx.x;")
        cu_gen.block("\t\tint time_idx = (t + {} - delta_t) % ( {} + 1);".format("MAX_DELAY", "MAX_DELAY"))
        cu_gen.block("\t\tint firedSize = g_fired_tableSizes[time_idx];")
        cu_gen.block("\t\tint num_per_block = (firedSize - 1) / gridDim.x + 1;")
        cu_gen.block("\t\tint block_nums_minus_1 = (firedSize - 1) / num_per_block;")
        cu_gen.block("\t\tint fired_size_block = 0;")
        cu_gen.block("\t\tif (block_idx == block_nums_minus_1) {")
        cu_gen.block("\t\tfired_size_block = firedSize - block_idx * num_per_block;")
        cu_gen.block("\t\t} else if (block_idx < block_nums_minus_1) {")
        cu_gen.block("\t\tfired_size_block = num_per_block;")
        cu_gen.block("\t\t} else {")
        cu_gen.block("\t\tfired_size_block = 0;")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\tfor (int idx = 0; idx < fired_size_block; idx++) {")
        cu_gen.block("\t\t\tint nid = g_fired_table[time_idx * g_fired_tableCap + (block_idx) * num_per_block + idx];")
        cu_gen.block("\t\t\tint start_loc = connection->delayStart[delta_t + nid * MAX_DELAY];")
        cu_gen.block("\t\t\tint synapseNum = connection->delayNum[delta_t + nid * MAX_DELAY];")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tgLayerInput[nid]++;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t\tfor (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {")
        cu_gen.block("\t\t\t\tint sid = j+start_loc;")
        cu_gen.block("\t\t\t\treal weight = d_synapses->p_weight[sid];")
        cu_gen.block("\t\t\t\tif (weight >= 0) {")
        cu_gen.block("\t\t\t\t\tatomicAdd(&(gNeuronInput[datas->p_dst[sid]]), weight);")
        cu_gen.block("\t\t\t\t} else {")
        cu_gen.block("\t\t\t\t\tatomicAdd(&(gNeuronInput_I[datas->p_dst[sid]]), weight);")
        cu_gen.block("\t\t\t\t}")
        cu_gen.blank_line()

        if self.pre_learn:
            cu_gen.block("\t\t\t\tdata->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));")
            cu_gen.block("\t\t\t\tdata->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));")
            cu_gen.block("")
            cu_gen.block("\t\t\t\tdata->p_pre[sid] += data->p_d_apre[sid];")
            cu_gen.block("\t\t\t\tdata->p_weight[sid] = _clip(weight + data->p_apost[sid], {}, {});"
                         .format("GMIN", "GMAX"))
            cu_gen.block("\t\t\t\tdata->p_last_update[sid] = gCurrentCycle;")

        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t}")
        cu_gen.block("}")
        cu_gen.blank_line()

        if self.post_learn:
            # Nothing to do with delay
            cu_gen.block("__global__ void update_{}_gpu({} * data, int num, int start_id, int t)"
                         .format(self.name.lower(), self.name.capitalize()))
            cu_gen.block("{")
            cu_gen.block("\tint block_idx = blockIdx.x;")
            cu_gen.block("\tint time_idx = t%(MAX_DELAY+1);")
            cu_gen.block("\tint firedSize = g_fired_tableSizes[time_idx];")
            cu_gen.block("\tint num_per_block = (firedSize - 1) / gridDim.x + 1;")
            cu_gen.block("\tint block_nums_minus_1 = (firedSize - 1) / num_per_block;")
            cu_gen.block("\tint fired_size_block = 0;")
            cu_gen.block("\tif (block_idx == block_nums_minus_1) {")
            cu_gen.block("\tfired_size_block = firedSize - block_idx * num_per_block;")
            cu_gen.block("\t} else if (block_idx < block_nums_minus_1) {")
            cu_gen.block("\tfired_size_block = num_per_block;")
            cu_gen.block("\t} else {")
            cu_gen.block("\t\tfired_size_block = 0;")
            cu_gen.block("\t}")
            cu_gen.blank_line()
            cu_gen.block("\tfor (int idx = 0; idx < fired_size_block; idx++) {")
            cu_gen.block("\t\tint nid = g_fired_table[time_idx * g_fired_tableCap + (block_idx) * num_per_block + idx];")
            cu_gen.block("\t\tint start_loc = gConnection{}->rev_delayStart[nid];".format(self.name.capitalize()))
            cu_gen.block("\t\tint synapseNum = gConnection{}->rev_delayNum[nid];".format(self.name.capitalize()))
            cu_gen.block("\t\tfor (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {")
            cu_gen.block("\t\t\tint sid = connection->rev_map2sid[j+start_loc];")
            cu_gen.block("\t\t\tdata->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));")
            cu_gen.block("\t\t\tdata->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));")
            cu_gen.block("\t\t\tdata->p_apost[sid] += data->p_d_apost[sid];")
            cu_gen.block("\t\t\tdata->p_weight[sid] = _clip(weight + data->p_pre[sid], %s, %s);" % ("GMIN", "GMAX"))
            cu_gen.block("\t\t\tdata->p_last_update[sid] = gCurrentCycle;")
            cu_gen.block("\t\t}")
            cu_gen.block("\t}")
            cu_gen.block("\t__syncthreads();")
            cu_gen.block("}")
            cu_gen.blank_line()

            # cu_gen.block("__global__ void update_{}_gpu({} *data, int num, int start_id, CConnection * connection)"
            #              .format(self.name.lower(), self.name))
            # cu_gen.block("{")
            # cu_gen.block("\tfor (int delta_t={}; delta_t<={}; delta_t++) {{".format("MIN_DELAY", "MAX_DELAY"))
            # cu_gen.block("\t\tint block_idx = blockIdx.x;")
            # cu_gen.block("\t\tint time_idx = (gCurrentIdx + {} - delta_t) % ({} + 1);".format("MAX_DELAY", "MAX_DELAY"))
            # cu_gen.block("\t\tint firedSize = g_fired_tableSizes[time_idx];")
            # cu_gen.block("\t\tint num_per_block = (firedSize - 1) / gridDim.x + 1;")
            # cu_gen.block("\t\tint block_nums_minus_1 = (firedSize - 1) / num_per_block;")
            # cu_gen.block("")
            # cu_gen.block("\t\tint fired_size_block = 0;")
            # cu_gen.block("\t\tif (block_idx == block_nums_minus_1) {")
            # cu_gen.block("\t\tfired_size_block = firedSize - block_idx * num_per_block;")
            # cu_gen.block("\t\t} else if (block_idx < block_nums_minus_1) {")
            # cu_gen.block("\t\tfired_size_block = num_per_block;")
            # cu_gen.block("\t\t} else {")
            # cu_gen.block("\t\tfired_size_block = 0;")
            # cu_gen.block("\t\t}")
            # cu_gen.blank_line()
            # cu_gen.block("\t\tfor (int idx = 0; idx < fired_size_block; idx++) {")
            # cu_gen.block("\t\t\tint nid = g_fired_table[time_idx * g_fired_tableCap + (block_idx) * num_per_block + idx];")
            # cu_gen.block("\t\t\tint start_loc = connection->rev_delayStart[delta_t + nid * MAX_DELAY];")
            # cu_gen.block("\t\t\tint synapseNum = connection->rev_delayNum[delta_t + nid * MAX_DELAY];")
            # cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
            # cu_gen.block("\t\t\tgLayerInput[nid]++;")
            # cu_gen.block("\t\t\t}")
            # cu_gen.block("\t\t\tfor (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {")
            # cu_gen.block("\t\t\tint sid = connection->rev_map2sid[j+start_loc];")
            # cu_gen.block("")
            # cu_gen.block("\t\t\tdata->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));")
            # cu_gen.block("\t\t\tdata->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));")
            # cu_gen.block("\t\t\tdata->p_apost[sid] += data->p_d_apost[sid];")
            # cu_gen.block("\t\t\tdata->p_weight[sid] = _clip(weight + data->p_pre[sid], %s, %s);" % ("gMin", "gMax"))
            # cu_gen.block("\t\t\tdata->p_last_update[sid] = gCurrentCycle;")
            # cu_gen.block("\t\t\t}")
            # cu_gen.block("\t\t}")
            # cu_gen.block("\t\t__syncthreads();")
            # cu_gen.block("\t}")
            # cu_gen.block("}")
            # cu_gen.blank_line()

        cu_gen.close()
        return


