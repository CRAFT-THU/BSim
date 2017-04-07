#!/usr/bin/python

import math

dt = 1e-4

w = 0.2e-9

tau_m = 10e-3
tau_E = 2e-3
tau_I = 0.6e-3

c_m = 0.2e-9

v_rest = -74e-3

i_offset = 0

rm = tau_m/c_m

Cm = math.exp(-dt/tau_m)
CE = math.exp(-dt/tau_E)
CI = math.exp(-dt/tau_I)
C2 = rm*(1-C1)

#i_tmp = i_offset + v_rest/rm

v_tmp = v_rest + i_offset * rm


C_E = rm * tau_E / (tau_E - tau_m);
C_I = rm * tau_I / (tau_I - tau_m);


i_E = 0e-9
i_I = -3e-9

vm = -74e-3

v = -75.3731877e-3

C_m = vm - v_tmp - C_E * i_E - C_I * i_I


v_new = v_tmp + i_E * C_E * CE + i_I * C_I * CI + C_m * C1

v_new2 = Cm*vm + (1-C1)*v_tmp + i_E * C_E * (CE-Cm) + i_I * C_I * (CI-Cm)

print v_new
print v_new2
print v
#print (rm*tau_E/(tau_E - tau_m))


