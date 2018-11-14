
from bsim.arrayobj import  ArrayObj

class LIF_Curr(ArrayObj):

    def __init__(self, name = '', num = 1, 
            v = 0.0, v_reset = -60e-3, v_rest=-74e-3, v_thresh = -54e-3,
            c = 0.2e-9, tau = 10e-3, tau_exc = 2e-3, tau_inh = 0.6e-3,
            i_offset = 2e-9,
            refract_time = 0):
        """

        """
        super(LIF_Curr, self).__init__(name, num)
        if num > 1:
            self.v = v
        elif num == 1:
            self.v = v
            self.v_reset = v_reset
            self.v_rest = v_rest
            self.v_thresh = v_thresh
            self.c = c
            self.tau = tau

        else:
            print("%s, the neuron number in %s is invalid", name, str(num))
            self.v = 0


    def __len__(self):
        return self.num
