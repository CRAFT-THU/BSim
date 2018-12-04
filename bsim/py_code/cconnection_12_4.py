
from ctypes import *


class CConnection(Structure):
	_fields_ = [
		("delay_start", POINTER(c_int * 12)),
		("delay_num", POINTER(c_int * 12)),
		("rev_delay_start", POINTER(c_int * 12)),
		("rev_delay_num", POINTER(c_int * 12)),
		("rev_map2sid", POINTER(c_int * 4)),
		("n_len", c_int),
		("r_n_len", c_int),
		("s_len", c_int)
	]

