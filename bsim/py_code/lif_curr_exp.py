
from ctypes import *


class Lif_curr_exp(Structure):
	_fields_ = [
		("p_refract_step", POINTER(c_int)),
		("p_refract_time", POINTER(c_int)),
		("p_v_reset", POINTER(c_float)),
		("p_Cexec", POINTER(c_float)),
		("p_Cm", POINTER(c_float)),
		("p_Cinh", POINTER(c_float)),
		("p_v_tmp", POINTER(c_float)),
		("p_C_inh", POINTER(c_float)),
		("p_C_exec", POINTER(c_float)),
		("p_v_threshold", POINTER(c_float)),
		("p_v", POINTER(c_float)),
	]

