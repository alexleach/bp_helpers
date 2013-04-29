#/usr/bin/env python

from compile_util import *

import unittest

def setUpModule():
    compile_test("refcounted_object", "test_refcounted_object.cpp")
    adjust_path()

class TestPyPrinter_base:

    def test_printer(self):
        from refcounted_object import run
        printer = self.PyPrinter()
        run(printer)

class TestPyPrinter(TestPyPrinter_base, unittest.TestCase):
    def setUp(self):
        from refcounted_object import APrinter
        # Override pure-virtual method.
        class PyPrinter(APrinter):
            def write(self, obj):
                print 'The value in obj is: {0}'.format(obj.getValue())
                print 'Calling from {0}; obj type: {1}'.format( type(self), obj )
        self.PyPrinter = PyPrinter


class TestPyPrinter2(TestPyPrinter_base, unittest.TestCase):

    def setUp(self):
        from refcounted_object import APrinter2
        # Override pure-virtual method.
        class PyPrinter(APrinter2):
            def write(self, obj):
                print 'The value in obj is: {0}'.format(obj.getValue())
                print 'Calling from {0}; obj type: {1}'.format( type(self), obj )
        self.PyPrinter = PyPrinter

if __name__ == '__main__':
    unittest.main()


