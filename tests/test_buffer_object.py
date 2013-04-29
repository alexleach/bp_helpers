#!/usr/bin/env python

# Thanks to Mark English for sharing this. Originally posted at:-
#    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767
import unittest

from compile_util import *

def setUpModule():
    compile_test("test_buffer_object", "test_buffer_object.cpp")
    adjust_path()

def tearDownModule():
    clean_test()

import gc, sys

class TestCaseBoostPython(unittest.TestCase):

    def setUp(self):
        import test_buffer_object
        self.handle  = test_buffer_object.Container()

    def make_normal_obj(self):
        obj = self.handle.obj
        del self.handle
        return obj
    
    def make_buffered_obj(self):
        buf = self.handle.buf
        del self.handle
        return buf

    def test_standard_container(self):
        #This works but wrong lifetime
        obj = self.make_normal_obj()
        print "Basic, empty class properties:-"
        print "   Size: ", sys.getsizeof(obj)
        gc.collect()
        self.assertEqual(type(obj).__name__, 'Foo')

    def test_buffer_container(self):
        #This also works
        buf = self.make_buffered_obj()
        print "Buffer state:-"
        print "   Size: {0}".format(sys.getsizeof(buf))
        print "   Dir: {0}".format(dir(buf))
        print "   str(Buffer)  : ", str(buf)
        print "   repr(Buffer) : ", repr(buf)
        print "   buffer: ",
        print buf
        gc.collect()
        self.assertEqual(type(buf).__name__, 'Buffer')

    def test_make_10_containers(self):
        import test_buffer_object
        buffered_objects = []
        for i in xrange(10):
            buffered_objects.append( test_buffer_object.Container().buf )
        del(buffered_objects[4])

    def test_write_to_buffer(self):
        buf = self.make_buffered_obj()

    def profile_N(self, n=1000):
        import cProfile as profile
        profile.run("test_buffer_object.Container()")

if __name__ == "__main__":
    unittest.main()
