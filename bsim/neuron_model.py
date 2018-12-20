
from bsim.env import pkg_dir
from bsim.model import Model
from bsim.model_compiler import compile_
from bsim.generator import CUDAGenerator


class NeuronModel(Model):
    def __init__(self, computation: str = '', threshold: str = 'v > vt', reset: str = 'v = vr',
                 refract: bool = True, name: str = '', compile_config: str = '', debug: bool = False):
        """
        Create NeuronModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        super(NeuronModel, self).__init__(compile_config=compile_config, debug=debug)

        self.name = name
        self.threshold = threshold
        self.reset = reset
        self.refract = refract

        self.expressions, self.parameters = compile_({
            'computation': computation,
        }, config=self.compile_config)

        #TODO dynamic deal with reset and threshold

        self.parameters['special'] = set(('refract_step', 'refract_time')) if self.refract else set()
        self.parameters['external'] = set(('g_i_exec', 'g_i_inh'))

        self.parameters['original'] |= set(('v_reset', 'v_threshold'))
        self.parameters['constant'] |= set(('v_reset', 'v_threshold'))
        self.parameters['original'] -= self.parameters['special'] | self.parameters['external']
        self.parameters['variable'] -= self.parameters['special'] | self.parameters['external']
        self.parameters['constant'] -= self.parameters['special'] | self.parameters['external']

    def generate_compute_cu(self):
        cu_gen = CUDAGenerator('{}/code_gen/{}.compute.cu'.format(pkg_dir, self.name.lower()))

        cu_gen.blank_line()
        cu_gen.include("runtime.h")
        cu_gen.include("{}.h".format(self.name.lower()))
        cu_gen.blank_line(2)

        cu_gen.block("__global__ void find_{}_gpu({} *data, int num, int start_id)"
                     .format(self.name.lower(), self.name.capitalize()))
        cu_gen.block("{")
        cu_gen.block("\t__shared__ int active_table_t[MAX_BLOCK_SIZE];")
        cu_gen.block("\t__shared__ volatile int active_cnt;")
        cu_gen.blank_line()

        cu_gen.block("\tif (threadIdx.x == 0 && blockIdx.x == 0) {")
        cu_gen.block("\t\tactive_cnt = 0;")
        cu_gen.block("\t\tg_active_{}_table_size = 0;".format(self.name.lower()))
        cu_gen.block("\t}")
        cu_gen.block("\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\tint tid = blockIdx.x * blockDim.x + threadIdx.x;")
        cu_gen.block("\tfor (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {")
        cu_gen.block("\t\tint test_loc = 0;")
        cu_gen.block("\t\tbool actived = data->p_refract_step[idx] <= 0;")
        cu_gen.block("\t\tif (actived) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\t\tactive_table_t[test_loc] = idx;")
        cu_gen.block("\t\t\t\tactived = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t} else {")
        cu_gen.block("\t\t\tg_i_exec[start_id + idx] = 0;")
        cu_gen.block("\t\t\tg_i_inh[start_id + idx] = 0;")
        cu_gen.block("\t\t\tdata->p_refract_time[idx] = data->p_refract_time[idx] - 1;")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (active_cnt >= MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(active_table_t, MAX_BLOCK_SIZE, g_active_{}_table, "
                     "&g_active_{}_table_size, 0);".format(self.name.lower(), self.name.lower()))
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tactive_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (actived) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\t\tactive_table_t[test_loc] = idx;")
        cu_gen.block("\t\t\t\tactived = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (active_cnt >= MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(active_table_t, MAX_BLOCK_SIZE, g_active_{}_table, "
                     "&g_active_{}_table_size, 0);".format(self.name.lower(), self.name.lower()))
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tactive_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (active_cnt > 0) {")
        cu_gen.block("\t\t\tcommit2globalTable(active_table_t, active_cnt, g_active_{}_table, "
                     "&g_active_{}_table_size, 0);".format(self.name.lower(), self.name.lower()))
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tactive_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t}")
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.block("__global__ void update_%s_gpu(%s *data, int num, int start_id, int t)" %
                      (self.name.lower(), self.name.capitalize()))
        cu_gen.block("{")
        cu_gen.block("\tint delay_idx = t % (MAX_DELAY+1);")
        cu_gen.block("\t__shared__ int fire_table_t[MAX_BLOCK_SIZE];")
        cu_gen.block("\t__shared__ volatile int fire_cnt;")
        cu_gen.block("\tif (threadIdx.x == 0 && blockIdx.x == 0) {")
        cu_gen.block("\t\tfire_cnt = 0;")
        cu_gen.block("\t\tg_fired_table_sizes[delay_idx] = 0;")
        cu_gen.block("\t}")
        cu_gen.block("\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\tint tid = blockIdx.x * blockDim.x + threadIdx.x;")
        cu_gen.block("\tfor (int idx = tid; idx < g_active_{}_table_size; idx +=blockDim.x*gridDim.x) {{"
                     .format(self.name.lower()))
        cu_gen.block("\t\tbool fired = false;")
        cu_gen.block("\t\tint test_loc = 0;")
        cu_gen.block("\t\tint nid = g_active_{}_table[idx];".format(self.name.lower()))
        cu_gen.block("\t\tint gnid = start_id + nid; ")
        cu_gen.blank_line()
        cu_gen.block("\t\tdata->p_v[nid] = data->p_Cm[nid] * data->p_v[nid] + data->p_v_tmp[nid] + "
                     "data->p_i_exec[nid] * data->p_C_exec[nid] + data->p_i_inh[nid] * data->p_C_inh[nid];")
        cu_gen.block("\t\tdata->p_i_exec[nid] *= data->p_Cexec[nid];")
        cu_gen.block("\t\tdata->p_i_inh[nid] *= data->p_Cinh[nid];")
        cu_gen.blank_line()
        cu_gen.block("\t\tfired = data->p_v[nid] >= data->p_v_threshold[nid];")
        #cu_gen.block("\t\tgFireCount[gnid] += fired;")
        cu_gen.block("\t\tif (fired) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\t\tfire_table_t[test_loc] = gnid;")
        cu_gen.block("\t\t\t\tfired = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t\tdata->p_refract_step[nid] = data->p_refract_time[nid] - 1;")
        cu_gen.block("\t\t\tdata->p_v[nid] = data->p_v_reset[nid];")
        cu_gen.block("\t\t} else {")
        #cu_gen.block("\t\t\tgXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];")
        cu_gen.block("\t\t\tdata->p_i_exec[nid] += g_i_exec[gnid];")
        cu_gen.block("\t\t\tdata->p_i_inh[nid] += g_i_inh[gnid];")
        cu_gen.block("\t\t}")
        cu_gen.blank_line()
        cu_gen.block("\t\tg_i_exec[gnid] = 0;")
        cu_gen.block("\t\tg_i_inh[gnid] = 0;")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\t\tif (fire_cnt >= MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(fire_table_t, MAX_BLOCK_SIZE, g_fired_table, "
                     "&g_fired_table_sizes[delay_idx], FIRED_TABLE_SIZE*delay_idx);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tfire_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\t\tif (fired) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\t\tfire_table_t[test_loc] = gnid;")
        cu_gen.block("\t\t\t\tfired = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t\tif (fire_cnt >= MAX_BLOCK_SIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(fire_table_t, MAX_BLOCK_SIZE, g_fired_table, "
                     "&g_fired_table_sizes[delay_idx], FIRED_TABLE_SIZE*delay_idx);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tfire_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t\t")
        cu_gen.block("\t\tif (fire_cnt > 0) {")
        cu_gen.block("\t\t\tcommit2globalTable(fire_table_t, fire_cnt, g_fired_table, "
                     "&g_fired_table_sizes[delay_idx], FIRED_TABLE_SIZE*delay_idx);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tfire_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t")
        cu_gen.block("\t}")
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.block("void update_%s(%s *data, int num, int start_id, int t)" %
                     (self.name.lower(), self.name.capitalize()))
        cu_gen.block("{")
        cu_gen.block("\tfind_{}_gpu<<<{}_GRID_SIZE, {}_BLOCK_SIZE>>>(({}*)data, num, start_id);"
                     .format(self.name.lower(), self.name.upper(), self.name.upper(), self.name.capitalize()))
        cu_gen.block("\tupdate_{}_gpu<<<{}_GRID_SIZE, {}_BLOCK_SIZE>>>(({}*)data, num, start_id, t);"
                     .format(self.name.lower(), self.name.upper(), self.name.upper(), self.name.capitalize()))
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.close()

        return
