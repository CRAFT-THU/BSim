
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
        super(NeuronModel, self).__init__()

        self.name = name
        self.threshold = threshold
        self.reset = reset
        self.refract = refract

        self.expressions, self.parameters = compile_({
            'computation': computation,
        })

        #TODO dynamic deal with reset and threshold

        self.parameters['special'] = set(('refract_step', 'refract_time')) if self.refract else set()
        self.parameters['outer'] = set(('i_exec', 'i_inh'))

        self.parameters['original'] |= set(('v_reset', 'v_threshold'))
        self.parameters['constant'] |= set(('v_reset', 'v_threshold'))
        self.parameters['original'] -= self.parameters['special'] | self.parameters['outer']
        self.parameters['variable'] -= self.parameters['special'] | self.parameters['outer']
        self.parameters['constant'] -= self.parameters['special'] | self.parameters['outer']

        self.dir = os.path.dirname(__file__)

    def generate_h(self):
        h_gen = CGenerator("{}/c_code/{}.h".format(self.dir, self.name.lower()))

        h_gen.blank_line(2)
        h_gen.if_define("{}_H ".format(self.name.upper()))
        h_gen.blank_line(2)
        h_gen.include("blocksize.h")
        h_gen.blank_line(2)

        h_gen.struct(self.name.capitalize(), 0)

        for i in self.parameters['special']:
            h_gen.line("int *p_%s" % i)

        for i in self.parameters['constant']:
            h_gen.line("float *p_%s" % i)

        for i in self.parameters['variable']:
            h_gen.line("float *p_%s" % i)

        h_gen.line("}", 0)
        h_gen.blank_line(1)

        h_gen.line_no_end('extern "C" {', 0)
        h_gen.line("void update_{}({} *data, int num, int start_id, BlockSize *size)"
                   .format(self.name.lower(), self.name.capitalize()))
        h_gen.line("{} * to_gpu_{}({} *cpu, int num)"
                   .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()))
        h_gen.line("{} * from_gpu_{}({} *gpu, int num)"
                   .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()))
        h_gen.close_brace()
        h_gen.blank_line(1)

        h_gen.line("__global__ void find_{}_gpu({} *data, int num, int start_id)"
                   .format(self.name.lower(), self.name.capitalize()), 0)
        h_gen.line("__global__ void update_{}_gpu({} *data, int num, int start_id)"
                   .format(self.name.lower(), self.name.capitalize()), 0)
        h_gen.blank_line(1)

        h_gen.end_if_define("{}_H".format(self.name.upper()))

        h_gen.close()
        return

    def generate_py(self):
        py_gen = PyGenerator('{}/py_code/{}.py'.format(self.dir, self.name.lower()))

        py_gen.blank_line()
        py_gen.import_("*", "ctypes")
        py_gen.blank_line(2)
        py_gen.class_(self.name.capitalize(), "Structure")
        py_gen.line("_fields_ = [")

        for i in list(self.parameters['special']):
            py_gen.line('("p_{}", POINTER(c_int)),'.format(i), 2)

        for i in list(self.parameters['constant']):
            py_gen.line('("p_{}", POINTER(c_float)),'.format(i), 2)

        for i in self.parameters['variable']:
            py_gen.line('("p_{}", POINTER(c_float)),'.format(i), 2)

        py_gen.line("]")
        py_gen.blank_line()

        py_gen.close()
        return

    def generate_data_cu(self, debug=False):
        cu_gen = CUDAGenerator('{}/c_code/{}.data.cu'.format(self.dir, self.name.lower()))

        cu_gen.blank_line(2)
        cu_gen.include_std('stdlib.h')
        cu_gen.blank_line()
        cu_gen.include("helper_cuda.h")
        cu_gen.include("{}.h".format(self.name.lower()))
        cu_gen.blank_line(2)

        cu_gen.line_no_end("{} * to_gpu_{}({} *cpu, int num)"
                           .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()), 0)
        cu_gen.open_brace()
        cu_gen.malloc(ret='gpu', type_=self.name.capitalize())

        for i in self.parameters['special']:
            cu_gen.to_gpu(ret='gpu->p_{}'.format(i), cpu='cpu->p_{}'.format(i), num='num', type_='int')

        for i in self.parameters['constant']:
            cu_gen.to_gpu(ret='gpu->p_{}'.format(i), cpu='cpu->p_{}'.format(i), num='num', type_='float')

        for i in self.parameters['variable']:
            cu_gen.to_gpu(ret='gpu->p_{}'.format(i), cpu='cpu->p_{}'.format(i), num='num', type_='float')

        cu_gen.line('{} * ret = NULL'.format(self.name.capitalize()))
        cu_gen.to_gpu(ret='ret', cpu='gpu', num='1', type_=self.name.capitalize())

        cu_gen.line('return ret')
        cu_gen.close_brace()
        cu_gen.blank_line()

        cu_gen.line_no_end("{} * from_gpu_{}({} *gpu, int num)"
                           .format(self.name.capitalize(), self.name.lower(), self.name.capitalize()), 0)
        cu_gen.open_brace()
        cu_gen.from_gpu(ret='cpu', gpu='gpu', type_=self.name.capitalize())

        cu_gen.line('return cpu')
        cu_gen.close_brace()
        cu_gen.blank_line()

        cu_gen.close()
        return

    def generate_compute_cu(self, debug=False):
        cu_gen = CUDAGenerator('{}/c_code/{}.compute.cu'.format(self.dir, self.name.lower()))

        cu_gen.blank_line()
        cu_gen.include("runtime.h")
        cu_gen.include("{}.h".format(self.name))
        cu_gen.blank_line(2)

        cu_gen.block("void update_%s(%s *data, int num, int start_id, BlockSize *size)" %
                      (self.name.lower(), self.name.capitalize()))
        cu_gen.block("{")
        cu_gen.block("\tfind_{}_gpu<<<size=>gridSize, size->blockSize>>>(({}*)data, num, start_id);"
                     .format(self.name.lower(), self.name.capitalize()))
        cu_gen.block("\tupdate_{}_gpu<<<size=>gridSize, size->blockSize>>>(({}*)data, num, start_id);"
                     .format(self.name.lower(), self.name.capitalize()))
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.block("__global void find_{}_gpu({} *data, int num, int start_id)"
                     .format(self.name.lower(), self.name))
        cu_gen.block("{")
        cu_gen.block("\t__shared__ int active_table_t[MAXBLOCKSIZE];")
        cu_gen.block("\t__shared__ volatile int active_cnt;")
        cu_gen.blank_line()

        cu_gen.block("\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\tactive_cnt = 0;")
        cu_gen.block("\t}")
        cu_gen.block("\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\tint tid = blockIdx.x * blockDim.x + threadIdx.x;")
        cu_gen.block("\tfor (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {")
        cu_gen.block("\t\tint test_loc = 0;")
        cu_gen.block("\t\tbool actived = data->p_refrac_step[idx] <= 0;")
        cu_gen.block("\t\tif (actived) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\t\tactive_table_t[test_loc] = idx;")
        cu_gen.block("\t\t\t\tactived = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t} else {")
        cu_gen.block("\t\t\tgNeuronInput[start_id + idx] = 0;")
        cu_gen.block("\t\t\tgNeuronInput_I[start_id + idx] = 0;")
        cu_gen.block("\t\t\tdata->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (active_cnt >= MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tactive_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (actived) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&active_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\t\tactive_table_t[test_loc] = idx;")
        cu_gen.block("\t\t\t\tactived = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (active_cnt >= MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tactive_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()
        cu_gen.block("\t\tif (active_cnt > 0) {")
        cu_gen.block("\t\t\tcommit2globalTable(active_table_t, active_cnt, gActiveTable, &gActiveTableSize, 0);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tactive_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t}")
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.block("__global void update_%s_gpu(%s *data, int num, int start_id)" %
                      (self.name.lower(), self.name))
        cu_gen.block("{")
        cu_gen.block("\tfor (int idx = tid; idx < gActiveTableSize; idx +=blockDim.x*gridDim.x) {")
        cu_gen.block("\t\tbool fired = false;")
        cu_gen.block("\t\tint test_loc = 0;")
        cu_gen.block("\t\tint nid = gActiveTable[idx];")
        cu_gen.block("\t\tint gnid = start_id + nid; ")
        cu_gen.blank_line()
        cu_gen.block("\t\tdata->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];")
        cu_gen.block("\t\tdata->p_i_E[nid] *= data->p_CE[nid];")
        cu_gen.block("\t\tdata->p_i_I[nid] *= data->p_CI[nid];")
        cu_gen.blank_line()
        cu_gen.block("\t\tfired = data->p_vm[nid] >= data->p_v_thresh[nid];")
        cu_gen.block("\t\tgFireCount[gnid] += fired;")
        cu_gen.block("\t\tif (fired) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\t\tfire_table_t[test_loc] = gnid;")
        cu_gen.block("\t\t\t\tfired = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t\tdata->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;")
        cu_gen.block("\t\t\tdata->p_vm[nid] = data->p_v_reset[nid];")
        cu_gen.block("\t\t} else {")
        cu_gen.block("\t\t\tgXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];")
        cu_gen.block("\t\t\tdata->p_i_E[nid] += gNeuronInput[gnid];")
        cu_gen.block("\t\t\tdata->p_i_I[nid] += gNeuronInput_I[gnid];")
        cu_gen.block("\t\t}")
        cu_gen.blank_line()
        cu_gen.block("\t\tgNeuronInput[gnid] = 0;")
        cu_gen.block("\t\tgNeuronInput_I[gnid] = 0;")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\t\tif (fire_cnt >= MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tfire_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.blank_line()

        cu_gen.block("\t\tif (fired) {")
        cu_gen.block("\t\t\ttest_loc = atomicAdd((int*)&fire_cnt, 1);")
        cu_gen.block("\t\t\tif (test_loc < MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\t\tfire_table_t[test_loc] = gnid;")
        cu_gen.block("\t\t\t\tfired = false;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t\tif (fire_cnt >= MAXBLOCKSIZE) {")
        cu_gen.block("\t\t\tcommit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tfire_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t__syncthreads();")
        cu_gen.block("\t\t")
        cu_gen.block("\t\tif (fire_cnt > 0) {")
        cu_gen.block("\t\t\tcommit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);")
        cu_gen.block("\t\t\tif (threadIdx.x == 0) {")
        cu_gen.block("\t\t\t\tfire_cnt = 0;")
        cu_gen.block("\t\t\t}")
        cu_gen.block("\t\t}")
        cu_gen.block("\t\t")
        cu_gen.block("\t}")
        cu_gen.block("}")
        cu_gen.blank_line()

        cu_gen.close()
        return
