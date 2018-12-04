from typing import List, Dict

import importlib

from bsim.connection import Connection
from bsim.population import Population
from bsim.projection import Projection


class Network(object):
    def __init__(self, dt: float = 0.0001, name: str = ''):
        self.dt = dt
        self.name = name

        # Original Data
        self.populations = {}  # type: Dict[NeuronModel, List[Population]]
        self.projections = {}  # type: Dict[SynapseModel, List[Dict]]

        # Statics Data
        # Total neuron or synapse numbers
        self.neuron_num = 0  # type: Int
        self.synapse_num = 0  # type: Int
        # Max and Min delay
        self.min_delay = 1e10  # type: Int
        self.max_delay = 0  # type: Int

        # Compiled Data
        self.neuron_models = []  # type: List[NeuronModel]
        self.neuron_nums = []  # type: List[Int]
        self.neuron_data = []  # type: List[Population]
        self.synapse_models = []  # type: SynapseModel
        self.synapse_nums = []  # type: List[Int]
        self.synapse_data = []  # type: List[Projection]
        self.connection_data = []  # type: List[Connection]

        # GPU data
        self.neuron_data_gpu = []
        self.synapse_data_gpu = []
        self.connection_data_gpu = []

        # Temp Data
        self.population2id = {}  # type: Dict[Population, Int]
        self.synapse2id = {}  # type: Dict[Projection, Int]
        self.synapse2neuron_id = {}  # type: Dict[Projection, Int]
        self.s_id2n_id = {}  # type: Dict[Int, Int]
        self.neuron2synapses = []  # type: List[List[List[Projection]]]
        self.neuron2synapses_rev = []  # type: List[List[List[Projection]]]

    def connect(self,
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

    def connect_alltoall(self,
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

    def _compile_model(self):
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

    def _compile_neuron_data(self):
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

    def _compile_temp_connection(self):
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

    def _compile_synapse_data(self):
        for t in range(len(self.synapse_models)):
            for d in range(self.min_delay, self.max_delay + 1):
                for i in range(len(self.neuron2synapses[t])):
                    for syn in self.neuron2synapses[t][i]:
                        #syn = self.neuron2synapses[t][i][j]
                        if syn.special['delay'] == d:
                            sid = len(self.synapse_data[t])
                            self.synapse2id[syn] = sid
                            self.synapse_data[t].merge(syn)
                            self.s_id2n_id[sid] = [self.synapse2neuron_id[syn]]

            self.synapse_nums[t + 1] = self.synapse_nums[t] + len(self.synapse_data[t])
        self.synapse_num = self.synapse_nums[len(self.synapse_models)]

        return 0

    def _compile_connection(self):
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
                                self.connection_data[t].delay_start[i + (d-self.min_delay)* self.neuron_num] = \
                                    self.synapse2id[syn]
                            count += 1
                    self.connection_data[t].delay_num[i + (d-self.min_delay) * self.neuron_num] = count
        return 0

    def _compile_reverse_connection(self):
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

    def to_gpu(self):
        for data in self.neuron_data:
            self.neuron_data_gpu.append(data.to_gpu())

        for data in self.synapse_data:
            self.synapse_data_gpu.append(data.to_gpu())

        for data in self.connection_data:
            self.connection_data_gpu.append(data.to_gpu())

        return 0

    def compile_(self):
        self._compile_model()

        self._compile_neuron_data()
        self._compile_temp_connection()
        self._compile_synapse_data()
        self._compile_connection()
        self._compile_reverse_connection()

        return 0

    def run_gpu(self):
        return 0
