
from ctypes import *


class Static(Structure):
	_fields_ = [
		("p_dst", POINTER(c_int)),
		("p_weight", POINTER(c_double)),
	]

