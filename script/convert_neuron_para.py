
import math
import numpy as np
from numpy import single as real

def convert(v_init, v_rest, v_reset, cm, tau_m, tau_refrac, tau_syn_E,
            tau_syn_I, v_thresh, i_offset, dt=1e-4):
    v_init = real(v_init)
    v_rest = real(v_rest)
    v_reset = real(v_reset)
    cm = real(cm)
    tau_m = real(tau_m)
    tau_refrac = real(tau_refrac)
    tau_syn_E = real(tau_syn_E)
    tau_syn_I = real(tau_syn_I)
    v_thresh = real(v_thresh)
    i_offset = real(i_offset)
    dt = real(dt)


    rm = tau_m / cm
    Cm = np.exp(-dt/tau_m)
    CE = np.exp(-dt/tau_syn_E)
    CI = np.exp(-dt/tau_syn_I)
    v_tmp = i_offset * rm + v_rest
    v_tmp *= (1-Cm)
    C_E = rm * tau_syn_E/(tau_syn_E - tau_m)
    C_I = rm * tau_syn_I/(tau_syn_I - tau_m)

    C_E = C_E * (CE - Cm)
    C_I = C_I * (CI - Cm)

    refrac_time = int(tau_refrac/dt)
    refrac_step = 0
    vm = v_init

    i_I = 0
    i_E = 0

    return (refrac_time, refrac_step, v_thresh, i_I, v_tmp, 
            0, C_I, CE, Cm, vm, v_reset, CI, i_E, C_E, 0)


if __name__ == '__main__':
    a = convert(1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9)
    b = convert(2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9)
    c = convert(3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9)


    for i,j,k in zip(a, b, c):
        t = [i]*2 + [j]*3 + [k] 
        print(t)

