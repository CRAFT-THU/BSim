from typing import List, Dict

from bsim.connection import Connection
from bsim.neuron import Population
from bsim.synapse import Projection


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

        # C data
        self.neuron_data_c = []
        self.synapse_data_c = []
        self.connection_data_c = []

        # GPU data
        self.neuron_data_gpu = []
        self.synapse_data_gpu = []
        self.connection_data_gpu = []

        # Temp Data
        self.population2id = {}  # type: Dict[Population, Int]
        self.synapse2id = {}  # type: Dict[Projection, Int]
        self.synapse2neuron_id = {}  # type: Dict[Projection, Int]
        self.s_id2n_id = {}  # type: Dict[Projection, Int]
        self.neuron2synapses = []  # type: List[List[List[Projection]]]
        self.neuron2synapses_reverse = []  # type: List[List[List[Projection]]]

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

        return 1

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

        return 1

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

        if population.type in self.populations:
            self.populations[population.type].append(population)
        else:
            self.populations[population.type] = [population]

        return 0

    def _compile_model(self):
        for neuron in self.populations:
            neuron.compile()
            self.neuron_models.append(neuron)
            self.neuron_data.append(
                Population(model=neuron, num=0, name="%s_compact" % neuron.name)
            )
        self.neuron_nums = [0] * (len(self.neuron_models) + 1)

        for synapse in self.projections:
            synapse.compile()
            self.synapse_models.append(neuron)
            self.synapse_data.append(
                Projection(model=synapse, num=0, name="%s_compact" % synapse.name)
            )
        self.synapse_nums = [0] * (len(self.synapse_models) + 1)

    def _compile_neuron_data(self):
        for i in range(len(self.neuron_models)):
            data = self.neuron_data[i]
            for population in self.populations[i]:
                self.population2id[population] = self.neuron_num[i] + len(data)
                data.merge(population)
            self.neuron_nums[i + 1] = len(data) + self.neuron_num[i]

        self.neuron_num = self.neuron_nums[len(self.neuron_models)]
        self.neuron2synapses = [[[] for _ in range(self.neuron_num)] for _ in range(len(self.synapse_models))]
        self.neuron2synapses_reverse = [[[] for _ in range(self.neuron_num)] for _ in range(len(self.synapse_models))]

        return 1

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
                    syn = c['projection']
                    self.max_delay = max(self.max_delay, c.parameters['constant']['delay'])
                    self.min_delay = min(self.max_delay, c.parameters['constant']['delay'])
                    self.neuron2synapses[self.synapse_models.index(model)][src_id].append(syn)
                    self.neuron2synapses_reverse[self.synapse_models.index(model)][dst_id].append(syn)
                    self.synapse2neuron_id[syn] = dst_id
                elif c['type'] == 'all2all':
                    # AlltoAll connection
                    for i in range(len(src)):
                        for j in range(len(dst)):
                            syn = c['projection'][i * len(dst) + j]
                            self.max_delay = max(self.max_delay, c.parameters['delay'])
                            self.min_delay = min(self.max_delay, c.parameters['delay'])
                            self.neuron2synapses[self.synapse_models.index(model)][src_id + i].append(syn)
                            self.neuron2synapses_reverse[self.synapse_models.index(model)][dst_id + j].append(syn)
                            self.synapse2neuron_id[syn] = dst_id + j
                else:
                    raise TypeError('Unsupported connection type')
        return 1

    def _compile_synapse_data(self):
        for t in range(len(self.synapse_models)):
            for d in range(self.min_delay, self.max_delay + 1):
                for i in range(len(self.neuron2synapses[t])):
                    for j in range(len(self.neuron2synapses[i][t])):
                        syn = self.neuron2synapses[i][t][j]
                        if syn.parameters['delay'] == d:
                            self.synapse2id[syn] = len(self.synapse_data[t])
                            self.synapse_data[t].merge(syn)
                            self.s_id2n_id.append[self.synapse2neuron_id[syn]]

            self.synapse_nums[t + 1] = self.synapse_nums[t] + len(self.synapse_data[t])
        self.synapse_num = self.synapse_nums[len(self.synapse_models)]

        return 1

    def _compile_connection(self):
        length = self.neuron_num * (self.max_delay - self.min_delay + 1)
        self.connection_data = [Connection() for _ in self.synapse_models]
        for i in self.connection_data:
            self.connection_data[i].delay_start = [0] * (length)
            self.connection_data[i].delay_num = [0] * (length)

        for t in range(len(self.synapse_models)):
            for d in range(self.min_delay, self.max_delay + 1):
                for i in range(len(self.neuron2synapses[t])):
                    count = 0
                    for j in range(len(self.neuron2synapses[i][t])):
                        if self.neuron2synapses[i][t][j].parameters['delay'] == d:
                            if count == 0:
                                self.connection_data[t].delay_start[i + d * self.neuron_num] = \
                                    self.synapse2id[self.neuron2synapses[i][t][j]]
                    self.connection_data[t].delay_num[i + d * self.neuron_num] = count
        return 1

    def _compile_reverse_connection(self):
        length = self.neuron_num * (self.max_delay - self.min_delay + 1)
        self.connection_data = [Connection() for _ in self.synapse_models]
        for i in self.connection_data:
            self.connection_data[i].rev_delay_start = [0] * (length)
            self.connection_data[i].rev_delay_num = [0] * (length)
            self.connection_data[i].rev_map2sid = [0] * (self.synapse_num)

        g_count = 0
        for t in range(len(self.synapse_models)):
            for d in range(self.min_delay, self.max_delay + 1):
                for i in range(len(self.neuron2synapses_rev[t])):
                    count = 0
                    for j in range(len(self.neuron2synapses_rev[i][t])):
                        if self.neuron2synapses_rev[i][t][j].parameters['delay'] == d:
                            if count == 0:
                                self.connection_data[t].delay_start_rev[i + d * self.neuron_num] = g_count
                                self.connection_data[t].rev_map2sid[g_count] = \
                                    self.synapse2id[self.synapse2id[self.neuron2synapses[t][i][j]]]
                    self.connection_data[t].delay_num[i + d * self.neuron_num] = count
                    g_count += count
        return 1

    def _compile_c_types(self):

        return 1

    def _compile_to_gpu(self):

        return 1

    def _compile_so(self):

        return 1

    def compile_(self):
        self._compile_model()

        self._compile_neuron_data()
        self._compile_temp_connection()
        self._compile_synapse_data()
        self._compile_connection()
        self._compile_reverse_connection()

        # TODO generate C data structure
        self._compile_c_types()
        self._compile_to_gpu()
        self._compile_so()

        return 1

    def run(self):
        return 0
