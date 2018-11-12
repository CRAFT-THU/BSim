
import numpy as np

def load_file(filename):
    f = open(filename)
    f_str = f.readlines()
    data_set = []
    for line in f_str:
        data_set.append([float(i) for i in line.split() ])

    return np.array(data_set)


