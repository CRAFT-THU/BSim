
from ctypes import *


class Stdp_synapse(Structure):
	_fields_ = [
		("p_last_update", POINTER(c_int)),
		("p_dst", POINTER(c_int)),
		("p_delay", POINTER(c_int)),
		("p_d_apost", POINTER(c_float)),
		("p_tau_pre", POINTER(c_float)),
		("p_d_apre", POINTER(c_float)),
		("p_tau_post", POINTER(c_float)),
		("p_apre", POINTER(c_float)),
		("p_apost", POINTER(c_float)),
		("p_weight", POINTER(c_float)),
	]

