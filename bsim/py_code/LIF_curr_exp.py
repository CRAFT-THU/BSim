from ctypes import *
class Lif_curr_exp(Structure):
	_fields_ = [
		("p_refract_step", POINTER(c_int)),
		("p_v", POINTER(c_float))
	]

