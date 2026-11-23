"""Driver for the sigma Python unit tests.

Discovers and runs every test in this directory and its subdirectories.
Registered with CTest as ``py_sigma`` by the top-level CMakeLists.txt.
"""

import os
import sys
import unittest

if __name__ == "__main__":
    my_dir = os.path.dirname(os.path.realpath(__file__))
    loader = unittest.TestLoader()
    tests = loader.discover(my_dir)
    testrunner = unittest.runner.TextTestRunner()
    ret = not testrunner.run(tests).wasSuccessful()
    sys.exit(ret)
