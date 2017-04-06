#!/usr/bin/python

import math

dt = 1e-4

w = 0.2e-9

tau_m = 10e-3
tau_E = 5e-3
c_m = 0.2e-9

v_rest = -74e-3
vm = -73.29027443e-3

i_offset = 0

rm = tau_m/c_m

C1 = math.exp(-dt/tau_m)
CE = math.exp(-dt/tau_E)
C2 = rm*(1-C1)

i_tmp = i_offset + v_rest/rm

i_E = w * math.sqrt(CE)

i_E *= CE


i_E = 0.2e-9

#print i_E * CE

i_E = i_E * math.sqrt(CE)

v = -73.36964415e-3

print ((vm - C1*v)/C2 - i_tmp)/math.sqrt(CE)
print (C1*vm + C2 * (i_tmp + i_E))


