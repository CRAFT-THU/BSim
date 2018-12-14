import math
import importlib
from ctypes import *
from typing import List, Dict, Sequence

import bsim
from bsim.neuron_model import NeuronModel
from bsim.synapse_model import SynapseModel
from bsim.cudamemop import cudamemops
from bsim.connection import Connection
from bsim.generator import CGenerator, CUDAGenerator
from bsim.population import Population
from bsim.projection import Projection


class Network(object):
    def __init__(self, dt: float = 0.0001, name: str = ''):
        self.dt = dt
        self.name = name
        self._so = None

        # Original Data
        self.populations = {}  # type: Dict[NeuronModel, List[Population]]
        self.projections = {}  # type: Dict[SynapseModel, List[Dict]]

        # Statics Data
        # Total neuron or synapse numbers
        self.neuron_num = 0  # type: int
        self.synapse_num = 0  # type: int
        # Max and Min delay
        self.min_delay = 1e10  # type: int
        self.max_delay = 0  # type: int
        self.g_max = 100
        self.g_min = -100
        self.max_block_size = 1024

        # Compiled Data
        self.neuron_models = []  # type: List[NeuronModel]
        self.neuron_nums = []  # type: List[int]
        self.neuron_data = []  # type: List[Population]
        self.synapse_models = []  # type: List[SynapseModel]
        self.synapse_nums = []  # type: List[int]
        self.synapse_data = []  # type: List[Projection]
        self.connection_data = []  # type: List[Connection]

        # GPU data
        self.neuron_data_gpu = []
        self.synapse_data_gpu = []
        self.connection_data_gpu = []

        # Temp Data
        self.population2id = {}  # type: Dict[Population, int]
        self.synapse2id = {}  # type: Dict[Projection, int]
        self.synapse2neuron_id = {}  # type: Dict[Projection, int]
        self.s_id2n_id = {}  # type: Dict[int, int]
        self.neuron2synapses = []  # type: List[List[List[Projection]]]
        self.neuron2synapses_rev = []  # type: List[List[List[Projection]]]

    def population(self, model: NeuronModel, **kwargs):
        kwargs['dt'] = self.dt
        p = Population(model=model, **kwargs)
        self._add_population(p, warn=True)

        return p

    def projection(self, model: NeuronModel, **kwargs):
        kwargs['dt'] = self.dt
        s = Projection(model=model, **kwargs)
        return s

    def one_to_one(self,
                   pre_population: Population, pre_id: int,
                   post_population: Population, post_id: int,
                   synapse: Projection):
        self._add_population(pre_population, warn=False)
        self._add_population(post_population, warn=False)

        if synapse.model in self.projections:
            self.projections[synapse.model].append({
                'type': 'n2n',
                'pre_population': pre_population,
                'pre_neuron': pre_id,
                'post_neuron': post_id,
                'post_population': post_population,
                'projection': synapse
            })
        else:
            self.projections[synapse.model] = [{
                'type': 'n2n',
                'pre_population': pre_population,
                'pre_neuron': pre_id,
                'post_neuron': post_id,
                'post_population': post_population,
                'projection': synapse
            }]

        return 0

    def all_to_all(self,
                   pre_population: Population,
                   post_population: Population,
                   projection: Projection):

        self._add_population(pre_population, warn=False)
        self._add_population(post_population, warn=False)

        if projection.model in self.projections:
            self.projections[projection.model].append({
                'type': 'all2all',
                'pre_population': pre_population,
                'post_population': post_population,
                'projection': projection
            })
        else:
            self.projections[projection.model] = [{
                'type': 'all2all',
                'pre_population': pre_population,
                'post_population': post_population,
                'projection': projection
            }]

        return 0

    def _add_population(self, population: Population, warn: bool = True):
        """
        Add a population to the network
        :param population: Population
        :param warn: bool
        :return: >=0 success else failed
        """
        if (population.model in self.populations) and (population in self.populations[population.model]):
            if warn:
                print('Population %s is already in the network %s', population.name, self.name)
            return -1

        if population.model in self.populations:
            self.populations[population.model].append(population)
        else:
            self.populations[population.model] = [population]

        return 0

    def _build_model(self):
        for neuron in self.populations:
            self.neuron_models.append(neuron)
            self.neuron_data.append(
                Population(model=neuron, num=0, name="{}_compact".format(neuron.name))
            )
        self.neuron_nums = [0] * (len(self.neuron_models) + 1)

        for synapse in self.projections:
            self.synapse_models.append(synapse)
            self.synapse_data.append(
                Projection(model=synapse, num=0, name="{}_compact".format(synapse.name))
            )
        self.synapse_nums = [0] * (len(self.synapse_models) + 1)

    def _build_neuron_data(self):
        for i in range(len(self.neuron_models)):
            data = self.neuron_data[i]
            for population in self.populations[self.neuron_models[i]]:
                self.population2id[population] = self.neuron_nums[i] + len(data)
                data.merge(population)
            self.neuron_nums[i + 1] = len(data) + self.neuron_nums[i]

        self.neuron_num = self.neuron_nums[len(self.neuron_models)]
        self.neuron2synapses = [[[] for _ in range(self.neuron_num)] for _ in range(len(self.synapse_models))]
        self.neuron2synapses_rev = [[[] for _ in range(self.neuron_num)] for _ in range(len(self.synapse_models))]

        return 0

    def _build_temp_connection(self):
        for model in self.projections:
            for c in self.projections[model]:
                src = c['pre_population']
                dst = c['post_population']
                src_id = self.population2id[src]
                dst_id = self.population2id[dst]
                if c['type'] == 'n2n':
                    # Neuron to neuron connection
                    src_id += int(c['pre_neuron'])
                    dst_id += int(c['post_neuron'])
                    syn = c['projection'][0]
                    syn.special['dst'] = dst_id
                    self.max_delay = max(self.max_delay, syn.special['delay'])
                    self.min_delay = min(self.min_delay, syn.special['delay'])
                    self.neuron2synapses[self.synapse_models.index(model)][src_id].append(syn)
                    self.neuron2synapses_rev[self.synapse_models.index(model)][dst_id].append(syn)
                    self.synapse2neuron_id[syn] = dst_id
                elif c['type'] == 'all2all':
                    # AlltoAll connection
                    for i in range(len(src)):
                        for j in range(len(dst)):
                            syn = c['projection'][i * len(dst) + j]
                            syn.special['dst'] = dst_id + j
                            self.max_delay = max(self.max_delay, syn.special['delay'])
                            self.min_delay = min(self.min_delay, syn.special['delay'])
                            self.neuron2synapses[self.synapse_models.index(model)][src_id + i].append(syn)
                            self.neuron2synapses_rev[self.synapse_models.index(model)][dst_id + j].append(syn)
                            self.synapse2neuron_id[syn] = dst_id + j
                else:
                    raise TypeError('Unsupported connection type')
        return 0

    def _build_synapse_data(self):
        for t in range(len(self.synapse_models)):
            for d in range(int(self.min_delay), int(self.max_delay + 1)):
                for i in range(len(self.neuron2synapses[t])):
                    for syn in self.neuron2synapses[t][i]:
                        if syn.special['delay'] == d:
                            sid = len(self.synapse_data[t])
                            self.synapse2id[syn] = sid
                            self.synapse_data[t].merge(syn)
                            self.s_id2n_id[sid] = self.synapse2neuron_id[syn]

            self.synapse_nums[t + 1] = self.synapse_nums[t] + len(self.synapse_data[t])
        self.synapse_num = self.synapse_nums[len(self.synapse_models)]

        return 0

    def _build_connection(self):
        length = self.neuron_num * (self.max_delay - self.min_delay + 1)
        self.connection_data = [Connection() for _ in self.synapse_models]
        for c in self.connection_data:
            c.delay_start = [0] * length
            c.delay_num = [0] * length

        for t in range(len(self.synapse_models)):
            for d in range(self.min_delay, self.max_delay + 1):
                for i in range(len(self.neuron2synapses[t])):
                    count = 0
                    for syn in self.neuron2synapses[t][i]:
                        if syn.special['delay'] == d:
                            if count == 0:
                                self.connection_data[t].delay_start[i+(d-self.min_delay)*self.neuron_num] = \
                                    self.synapse2id[syn]
                            count += 1
                    self.connection_data[t].delay_num[i + (d-self.min_delay) * self.neuron_num] = count
        return 0

    def _build_reverse_connection(self):
        # Reversed connection have no delay
        for i, c in enumerate(self.connection_data):
            c.rev_delay_start = [0] * self.neuron_num
            c.rev_delay_num = [0] * self.neuron_num
            c.rev_map2sid = [0] * (self.synapse_nums[i+1] - self.synapse_nums[i])

        g_count = 0
        for t in range(len(self.synapse_models)):
            for i in range(len(self.neuron2synapses_rev[t])):
                count = 0
                for syn in self.neuron2synapses_rev[t][i]:
                    if count == 0:
                        self.connection_data[t].rev_delay_start[i] = g_count
                    self.connection_data[t].rev_map2sid[g_count + count] = self.synapse2id[syn]
                    count += 1
                self.connection_data[t].rev_delay_num[i] = count
                g_count += count

        # length = self.neuron_num * (self.max_delay - self.min_delay + 1)
        # self.connection_data = [Connection() for _ in self.synapse_models]
        # for i in self.connection_data:
        #     self.connection_data[i].rev_delay_start = [0] * (length)
        #     self.connection_data[i].rev_delay_num = [0] * (length)
        #     self.connection_data[i].rev_map2sid = [0] * (self.synapse_num)

        # g_count = 0
        # for t in range(len(self.synapse_models)):
        #     for d in range(self.min_delay, self.max_delay + 1):
        #         for i in range(len(self.neuron2synapses_rev[t])):
        #             count = 0
        #             for j in range(len(self.neuron2synapses_rev[i][t])):
        #                 if self.neuron2synapses_rev[i][t][j].parameters['delay'] == d:
        #                     if count == 0:
        #                         self.connection_data[t].delay_start_rev[i + d * self.neuron_num] = g_count
        #                         self.connection_data[t].rev_map2sid[g_count] = \
        #                             self.synapse2id[self.synapse2id[self.neuron2synapses[t][i][j]]]
        #             self.connection_data[t].rev_delay_num[i + d * self.neuron_num] = count
        #             g_count += count
        return 0

    def _generate_runtime(self):
        h_gen = CGenerator('{}/code_gen/runtime.h'.format(pkg_dir))
        h_gen.if_define('runtime.h')
        h_gen.blank_line(2)
        h_gen.include("connection.h")
        h_gen.blank_line(2)

        h_gen.block('const int MAX_DELAY = {};'.format(self.max_delay))
        h_gen.block('const int MIN_DELAY = {};'.format(self.min_delay))
        h_gen.block('const float G_MAX = {};'.format(self.g_max))
        h_gen.block('const float G_MIN = {};'.format(self.g_min))

        for i in range(len(self.neuron_models)):
            block_size = 32
            h_gen.block("const int {}_BLOCK_SIZE = {};".format(self.neuron_models[i].name.upper(), int(block_size)))
            h_gen.block("const int {}_GRID_SIZE = {};".format(
                self.neuron_models[i].name.upper(), math.ceil(self.neuron_nums[i+1]/block_size)))

        for i in range(len(self.synapse_models)):
            block_size = 128
            h_gen.block("const int {}_BLOCK_SIZE = {};".format(
                self.synapse_models[i].name.upper(), int(block_size)))
            h_gen.block("const int {}_GRID_SIZE = {};".format(
                self.synapse_models[i].name.upper(), math.ceil(self.synapse_nums[i+1]/block_size)))
        h_gen.blank_line()

        h_gen.block("const int MAX_BLOCK_SIZE = {};".format(self.max_block_size))
        h_gen.block("const int FIRED_TABLE_SIZE = {};".format(self.neuron_num))

        h_gen.block('extern __device__ int * g_fired_table;')
        h_gen.block('extern __device__ int * g_fired_table_sizes;')

        for model in self.neuron_models:
            h_gen.block('extern __device__ int * g_active_{}_table;'.format(model.name.lower()))
            h_gen.block('extern __device__ int g_active_{}_table_size;'.format(model.name.lower()))

        for model in self.synapse_models:
            h_gen.block('extern __device__ CConnection * g_connection_{};'.format(model.name.lower()))

        external = set()
        for model in self.neuron_models:
            external |= set(model.parameters['external'])
        for model in self.synapse_models:
            external |= set(model.parameters['external'])
        external -= set('t')

        for i in external:
            h_gen.block('extern __device__ float * {};'.format(i))
        h_gen.blank_line()

        h_gen.block('extern "C" {')
        h_gen.block('\tvoid **init_runtime(CConnection **connections);')
        h_gen.block('}')
        h_gen.blank_line()

        h_gen.block('__device__ int commit2globalTable(int *shared_buf, volatile unsigned int size, '
                    'int *global_buf, int * global_size, int offset);')

        h_gen.end_if_define('runtime.h')
        h_gen.close()

        cu_gen = CUDAGenerator('{}/code_gen/runtime.cu'.format(pkg_dir))

        cu_gen.include_std("stdio.h")

        cu_gen.include('../c_code/helper_cuda.h')
        cu_gen.include("runtime.h")
        cu_gen.blank_line(2)

        cu_gen.block('__device__ int * g_fired_table;')
        cu_gen.block('__device__ int * g_fired_table_sizes;')

        for model in self.neuron_models:
            cu_gen.block('__device__ int * g_active_{}_table;'.format(model.name.lower()))
            cu_gen.block('__device__ int g_active_{}_table_size;'.format(model.name.lower()))

        for model in self.synapse_models:
            cu_gen.block('__device__ CConnection * g_connection_{};'.format(model.name.lower()))

        external = set()
        for model in self.neuron_models:
            external |= set(model.parameters['external'])
        for model in self.synapse_models:
            external |= set(model.parameters['external'])
        external -= set('t')

        for i in external:
            cu_gen.block('__device__ float * {};'.format(i))
        cu_gen.blank_line(2)

        cu_gen.block('void **init_runtime(CConnection ** connections)')
        cu_gen.block('{')
        cu_gen.block('\tint zero = 0;')
        cu_gen.block('\tint *p_int = NULL;')
        cu_gen.block('\tfloat *p_float = NULL;')
        cu_gen.blank_line()

        cu_gen.block('\tvoid **ret = static_cast<void**>(malloc(sizeof(void*) * {}));'.format(2))
        cu_gen.blank_line()

        cu_gen.malloc_symbol(symbol='g_fired_table_sizes', gpu='p_int', type_='int',
                             num='{}'.format(self.max_delay+1))
        cu_gen.block('\tret[0] = static_cast<void*>(p_int);')
        cu_gen.malloc_symbol(symbol='g_fired_table', gpu='p_int', type_='int',
                             num='{}'.format(self.neuron_num*(self.max_delay+1)))
        cu_gen.block('\tret[1] = static_cast<void*>(p_int);')
        # cu_gen.block('\tprintf("\\n%p, %p, %p\\n", ret, ret[0], ret[1]);')
        cu_gen.blank_line()

        for model in self.neuron_models:
            cu_gen.cu_line('cudaMemcpyToSymbol(g_active_{}_table_size, &zero, sizeof(int))'
                           .format(model.name.lower()))
            cu_gen.malloc_symbol(symbol='g_active_{}_table'.format(model.name.lower()), gpu='p_int', type_='int',
                                 num='{}'.format(self.neuron_num))
        cu_gen.block('\n')

        for i in external:
            cu_gen.malloc_symbol(symbol='{}'.format(i), gpu='p_float', type_='float', num='{}'.format(self.neuron_num))

        for i, model in enumerate(self.synapse_models):
            cu_gen.cu_line('cudaMemcpyToSymbol(g_connection_{}, &(connections[{}]), sizeof(CConnection*))'
                           .format(model.name.lower(), i))

        cu_gen.block('\treturn ret;')
        cu_gen.block('}')

        cu_gen.close()

    def to_gpu(self):
        for data in self.neuron_data:
            self.neuron_data_gpu.append(data.to_gpu())

        for data in self.synapse_data:
            self.synapse_data_gpu.append(data.to_gpu())

        for data in self.connection_data:
            self.connection_data_gpu.append(data.to_gpu())

        return 0

    def compile_(self):
        self._generate_runtime()

        src = '{}/c_code/runtime.cu {}/code_gen/runtime.cu'.format(pkg_dir, pkg_dir)

        for model in self.neuron_models:
            src += ' {}/code_gen/{}.compute.cu '.format(pkg_dir, model.name.lower())

        for model in self.synapse_models:
            src += ' {}/code_gen/{}.compute.cu '.format(pkg_dir, model.name.lower())

        if CUDAGenerator.compile_(
                src=src,
                output='{}/so_gen/runtime.so'.format(pkg_dir)
        ):
            self._so = cdll.LoadLibrary('{}/so_gen/runtime.so'.format(pkg_dir))
        else:
            self._so = None
            raise EnvironmentError('Compile file runtime.so failed')

        return

    def so(self):
        if not self._so:
            self.compile_()
        return self._so

    def build(self):
        self._build_model()
        self._build_neuron_data()
        self._build_temp_connection()
        self._build_synapse_data()
        self._build_connection()
        self._build_reverse_connection()

        return 0

    def run_gpu(self, time, log: Sequence[str] = None):
        cycle = int(time/self.dt)

        so = self.so()
        c_connection = importlib.import_module('bsim.code_gen.cconnection').CConnection

        so.init_runtime.restype = POINTER(c_void_p)
        log_info = so.init_runtime((POINTER(c_connection)*len(self.synapse_models))(*self.connection_data_gpu))

        fired_size = c_int(0)
        fired_neuron = (c_int * self.neuron_num)(0)
        fire_bin_log = open("fire.bin.log", "wb+")

        # fire_txt_log = open("fire.txt.log", "w+")
        # v = (c_float*self.neuron_num)(0)
        # neuron_data = self.neuron_data[0].from_gpu(self.neuron_data_gpu[0], self.neuron_nums[1], only_struct=True)

        for t in range(cycle):

            for i, model in enumerate(self.neuron_models):
                getattr(so, 'update_{}'.format(model.name.lower()))(self.neuron_data_gpu[i],
                                                                    self.neuron_nums[i+1] - self.neuron_nums[i],
                                                                    self.neuron_nums[i], t)

            for i, model in enumerate(self.synapse_models):
                getattr(so, 'update_{}'.format(model.name.lower()))(self.synapse_data_gpu[i],
                                                                    self.synapse_nums[i+1] - self.synapse_nums[i],
                                                                    self.synapse_nums[i], t)

            if isinstance(log, Sequence) and len(log) > 0:
                offset = t % (self.max_delay + 1)
                cudamemops.gpu2cpu_int(cast(log_info[0] + sizeof(c_int) * offset,
                                            POINTER(c_int)), pointer(fired_size), 1)
                cudamemops.gpu2cpu_int(cast(log_info[1] + self.neuron_num * offset * sizeof(c_int),
                                            POINTER(c_int)), fired_neuron, fired_size)
                fire_bin_log.write(bytes(fired_size))
                fire_bin_log.write(bytes(fired_neuron)[0:fired_size.value*sizeof(c_int)])
                # fire_txt_log.write(' '.join(map(str, list(fired_neuron)[0:fired_size.value])))
                # fire_txt_log.write('\n')

                # cudamemops.gpu2cpu_float(neuron_data.p_v, v, self.neuron_num)

        if log:
            fire_bin_log.close()
            # fire_txt_log.close()

        return 0
