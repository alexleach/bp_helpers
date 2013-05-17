#!/usr/bin/env python2-dbg

"""
This is an example module, demonstrating how to expose std::fstream to Python.
It is designed to accompany `src/tests/test_fstream.cpp`

"""

## 
import unittest
from compile_util import *

# setUpModule is run by `unittest.main`. Here, it compiles test_iostream.cpp
# into an importable Python module.
def setUpModule():
    from os.path import join
    compile_test("fstream", join("test_fstream.cpp"))
    adjust_path()

def tearDownModule():
    #clean_test()
    sys.stderr.flush()

import gc, sys

class TestFStream(unittest.TestCase):
    def make_a_file(self):
        print "import fstream!"
        try:
            import fstream
        except ImportError:
            print "Cannot import fstream!"
            return []
        # make a file
        return fstream.fstream("a_file.txt")

    def test_open(self):
        # By default, file opens in read-write mode.
        open_file = self.make_a_file()
        open_file[0:] = "foo bar baz"
        print open_file

    def test_close(self):
        open_file = self.make_a_file()
        print "\ndir(instance): - ", dir(open_file)
        print "\ndir(class)", open_file.__class__, dir(open_file.__class__)
        #open_file.write("foo bar baz")
        open_file.close()

if __name__ == '__main__':
    unittest.main()
