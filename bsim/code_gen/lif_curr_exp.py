
from ctypes import *


class Lif_curr_exp(Structure):
	_fields_ = [
		("p_refract_time", POINTER(c_int)),
		("p_refract_step", POINTER(c_int)),
		("p_Cinh", POINTER(c_double)),
		("p_v_tmp", POINTER(c_double)),
		("p_v_reset", POINTER(c_double)),
		("p_Cexec", POINTER(c_double)),
		("p_C_inh", POINTER(c_double)),
		("p_C", POINTER(c_double)),
		("p_C_exec", POINTER(c_double)),
		("p_v_threshold", POINTER(c_double)),
		("p_i_exec", POINTER(c_double)),
		("p_v", POINTER(c_double)),
		("p_i_inh", POINTER(c_double)),
	]

