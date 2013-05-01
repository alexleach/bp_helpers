#!/usr/bin/env python

import unittest

from compile_util import *

def setUpModule():
    compile_test("test_iostreams", "test_iostreams.cpp")
    adjust_path()

def tearDownModule():
    clean_test()

import gc, sys

class TestIOStreamBase(unittest.TestCase):
    def setUp(self):
        from test_iostreams import Container
        self.container  = Container()

class TestIStream(TestIOStreamBase):
    def test_weakref_istream(self):
        istream = self.container.istream
        is2 = istream
        del self.container
        print istream
        print is2
        return istream

    def test_istream_features(self):
        #This also works
        buf = self.container.istream
        print "Istream state:-"
        print "   Size:        : {0}".format(sys.getsizeof(buf))
        print "   Dir:         : {0}".format(dir(buf))
        print "   repr(Buffer) : {0}".format(repr(buf))
        print "   str(Buffer)  : {0}".format(str(buf))
        print "   type(Buffer) : {0}".format(type(buf))
        gc.collect()

    def test_make_10_istreams(self):
        from test_iostreams import Container
        buffered_objects = []
        for i in xrange(10):
            buffered_objects.append( Container().istream )
        del(buffered_objects[4])

    ### TO IMPLEMENT IN THE CODE!!! ###
    def _test_read_from_istream(self):
        """ TODO - test_write_to_ostream """
        buf = self.make_buffered_obj()

    def _test_profile_istream(self, n=1000):
        """ TODO - test_profile"""
        import cProfile as profile
        profile.run("test_buffer_object.Container()")


class TestOStream(TestIOStreamBase):
    def test_weakref_ostream(self):
        ostream = self.container.ostream
        os2 = ostream
        del self.container
        print os2
        return ostream

    def test_ostream_features(self):
        #This also works
        buf = self.container.ostream
        print "Ostream state:-"
        print "   Size:        : {0}".format(sys.getsizeof(buf))
        print "   Dir:         : {0}".format(dir(buf))
        print "   repr(Buffer) : {0}".format(repr(buf))
        print "   str(Buffer)  : {0}".format(str(buf))
        print "   type(Buffer) : {0}".format(type(buf))
        gc.collect()

    def test_make_10_ostreams(self):
        from test_iostreams import Container
        buffered_objects = []
        for i in xrange(10):
            container = Container()
            buffered_objects.append( container.ostream )
        print "made 10 ostreams"
        del(buffered_objects[4])

    ### TO IMPLEMENT IN THE CODE!!! ###
    def _test_write_to_ostream(self):
        """ TODO - test_write_to_ostream """
        buf = self.make_buffered_obj()

    def _test_profile_ostream(self, n=1000):
        """ TODO - test_profile"""
        import cProfile as profile
        profile.run("test_buffer_object.Container()")


if __name__ == "__main__":
    unittest.main()
