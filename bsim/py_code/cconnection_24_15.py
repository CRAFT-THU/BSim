
from ctypes import *


class CConnection(Structure):
	_fields_ = [
		("delay_start", POINTER(c_int * 24)),
		("delay_num", POINTER(c_int * 24)),
		("rev_delay_start", POINTER(c_int * 8)),
		("rev_delay_num", POINTER(c_int * 8)),
		("rev_map2sid", POINTER(c_int * 15)),
		("n_len", c_int),
		("r_n_len", c_int),
		("s_len", c_int)
	]

