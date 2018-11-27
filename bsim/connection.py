
from ctypes import *

class Connection(object):
    def __init__(self):
        self.delay_start = []
        self.delay_num = []
        self.rev_delay_start = []
        self.rev_delay_num = []
        self.rev_map2sid = []

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

