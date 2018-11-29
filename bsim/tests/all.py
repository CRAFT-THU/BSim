
import unittest


suite = unittest.TestSuite()
all_cases = unittest.defaultTestLoader.discover('.', '*_test.py')
for case in all_cases:
        suite.addTests(case)

runner = unittest.TextTestRunner()
runner.run(suite)
