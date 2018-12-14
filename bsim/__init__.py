
import os
import sys

from bsim.env import pkg_dir
import bsim.neuron_model
import bsim.synapse_model
import bsim.network

Neuron = neuron_model.NeuronModel
Synapse = synapse_model.SynapseModel
Net = network.Network

sys.path.append(os.path.dirname(pkg_dir))

if not os.path.exists(os.path.join(pkg_dir, 'code_gen')):
    os.mkdir(os.path.join(pkg_dir, 'code_gen'))

if not os.path.exists(os.path.join(pkg_dir, 'so_gen')):
    os.mkdir(os.path.join(pkg_dir, 'so_gen'))


__all__ = ['Neuron', 'Synapse', 'Net']
