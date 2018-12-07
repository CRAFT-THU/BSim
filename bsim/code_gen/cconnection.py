
from ctypes import *


class CConnection(Structure):
	_fields_ = [
		("delay_start", POINTER(c_int)),
		("delay_num", POINTER(c_int)),
		("rev_delay_start", POINTER(c_int)),
		("rev_delay_num", POINTER(c_int)),
		("rev_map2sid", POINTER(c_int)),
		("n_len", c_int),
		("r_n_len", c_int),
		("s_len", c_int)
	]

