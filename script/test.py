
import numpy as np

a = np.loadtxt('../build/bin/g_v.data')
b = np.loadtxt('/tmp/pycharm_project_471/test/v.txt.log')

c = a - b

e = np.maximum(abs(a), abs(b))

d = np.where(e>0, abs(c)/e, 0)

print("Value: ")
print("\tmax: {} min: {} abs_max: {}".format(c.max(), c.min(), abs(c).max()))
print("Percentage: ")
print("\tmax: {} min: {} abs_max: {}".format(d.max(), d.min(), abs(d).max()))
