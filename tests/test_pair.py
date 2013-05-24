from compile_util import *
import unittest

def setUpModule():
    compile_test("test_pair", "test_make_pair.cpp")
    # We've just compiled the extension module inplace. Insert cwd into
    #  PYTHONPATH.
    import os, sys
    sys.path.insert(0, os.getcwd())
    # import it and insert into global namespace
    global test_pair
    try:
        import test_pair
    except ImportError:
        raise unittest.SkipTest("Can't import test_pair compiled module. ")

class TestPair(unittest.TestCase):
    def test_return_pair(self):
        pair = test_pair.make_a_pair()
        print pair

if __name__ == '__main__':
    unittest.main()