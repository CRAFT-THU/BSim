
import unittest

from bsim.connection import *


class TestConnectionMethods(unittest.TestCase):
    def test_data(self):
        c = Connection(debug=False)
        c.delay_start = [0, 0, 3, 0, 1, 0, 0, 0, 0, 2, 0, 0]
        c.delay_num = [1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0]
        c.rev_delay_start = [0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 3]
        c.rev_delay_num = [0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1]
        c.rev_map2sid = [0, 3, 1, 2]

        gpu = c.to_gpu()
        cpu = c.from_gpu(gpu, only_struct=False)
        self.assertListEqual(c.delay_start, list(cast(cpu.delay_start, POINTER(c_int*cpu.n_len)).contents))
        self.assertListEqual(c.delay_num, list(cast(cpu.delay_num, POINTER(c_int*cpu.n_len)).contents))
        self.assertListEqual(c.rev_delay_start, list(cast(cpu.rev_delay_start, POINTER(c_int*cpu.r_n_len)).contents))
        self.assertListEqual(c.rev_delay_num, list(cast(cpu.rev_delay_num, POINTER(c_int*cpu.r_n_len)).contents))
        self.assertListEqual(c.rev_map2sid, list(cast(cpu.rev_map2sid, POINTER(c_int*cpu.s_len)).contents))


if __name__ == '__main__':
    unittest.main()

