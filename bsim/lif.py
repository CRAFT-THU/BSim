
class LIF_Curr(ArrayObj):
    def __init__(self, name = '', num = 1, 
            v, v_reset, v_rest, v_thresh, 
            c, tau, tau_exc, tau_inh,
            i_offset,
            refract_time):
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
            self.c
        else:
            print("%s, the neuron number in %s is invalid", name, str(num))
            self.v = 0


    def __len__(self):
        return self.num
