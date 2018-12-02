
import unittest

import env

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
        self.assertListEqual(c.delay_start, list(cpu.delay_start.contents))
        self.assertListEqual(c.delay_num, list(cpu.delay_num.contents))
        self.assertListEqual(c.rev_delay_start, list(cpu.rev_delay_start.contents))
        self.assertListEqual(c.rev_delay_num, list(cpu.rev_delay_num.contents))
        self.assertListEqual(c.rev_map2sid, list(cpu.rev_map2sid.contents))


if __name__ == '__main__':
    unittest.main()

