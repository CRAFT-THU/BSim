#!/usr/bin/python

import os

import generate_type_gpu
import generate_type_cpu

synapse_path = "../src/synapse/"
neuron_path = "../src/neuron/"

def get_type_paras():
    #paras = {
    #        "v_reset" : "real",
    #        "v_thresh" : "real",
    #        "i_E" : "real",
    #        "i_I" : "real",
    #        "vm" : "real",
    #        "C1" : "real",
    #        "C2" : "real",
    #        "CE" : "real",
    #        "CI" : "real",
    #        "i_tmp" : "real",
    #        "refrac_step" : "int",
    #        "refrac_time" : "int",
    #        }

    paras = {
            "weight" : "real",
            }

    return paras

def get_type_name():
    #return "LIFE"
    return "Static"

def get_type_type():
    #return "Neuron"
    return "Synapse"

def get_path(type_type):
    script_path = os.path.abspath(__file__)
    script_dir = os.path.split(script_path)[0]

    r_path = ""
    if type_type == "Neuron":
        r_path = neuron_path
    elif type_type == "Synapse":
        r_path = synapse_path
    else:
        r_path = ""

    return os.path.join(script_dir, r_path)

if __name__ == '__main__':
    paras = get_type_paras()
    type_name = get_type_name()
    type_type = get_type_type()
    path_name = get_path(type_type)

    #generate_type_cpu.generate_h_file(paras, type_name, type_type, path_name)
    #generate_type_cpu.generate_cpp_file(paras, type_name, type_type, path_name)

    type_type += "s"
    generate_type_gpu.generate_h_file(paras, type_name, type_type, path_name)
    generate_type_gpu.generate_cpp_file(paras, type_name, type_type, path_name)
    generate_type_gpu.generate_cu_file(paras, type_name, type_type, path_name)
    

