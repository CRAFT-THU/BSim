
from ctypes import *


class CConnection(Structure):
    _fields_ = [
        ("delayStart", POINTER(c_int)),
        ("delayNum", POINTER(c_int)),
        ("rev_delayStart", POINTER(c_int)),
        ("rev_delayNum", POINTER(c_int)),
        ("rev_map2sid", POINTER(c_int)),
        ("n_num", c_int),
        ("s_num", c_int)
    ]

