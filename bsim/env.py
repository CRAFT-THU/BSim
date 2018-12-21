
import os
import ctypes


pkg_dir = os.path.dirname(__file__)

real = 'double'

c_real = getattr(ctypes, 'c_{}'.format(real))


