#!/usr/bin/env python
"""
Unit tests for the C++ iostream wrappers.
"""

import unittest

from compile_util import *

def setUpModule():
    from os.path import join
    compile_test("iostreams", join("test_iostreams.cpp"))
    adjust_path()

def tearDownModule():
    clean_test()

import gc, sys

class TestIOStreamBase(unittest.TestCase):
    """Test IOStream objects returned from a function."""
    def setUp(self):
        from iostreams import Container
        self.container  = Container()

class TestIStream(TestIOStreamBase):
    """Tests the return value policy for std::istream-like objects."""
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
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        gc.collect()

    def test_istream_init_w_string(self):
        from iostreams import IStream
        buf = IStream("foo bar baz")
        self.assertEqual(str(buf), "foo bar baz")

    def _test_make_5_istreams(self):
        from iostreams import Container
        buffered_objects = []
        for i in xrange(5):
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
    """Test return value policy for std::ostream-like objects."""
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
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        gc.collect()

    def _test_make_5_ostreams(self):
        from iostreams import Container
        buffered_objects = []
        for i in xrange(5):
            container = Container()
            buffered_objects.append( container.ostream )
        print "made 5 ostreams"
        del(buffered_objects[4])

    ### TO IMPLEMENT IN THE CODE!!! ###
    def _test_write_to_ostream(self):
        """ TODO - test_write_to_ostream """
        buf = self.make_buffered_obj()

    def _test_profile_ostream(self, n=1000):
        """ TODO - test_profile"""
        import cProfile as profile
        profile.run("test_buffer_object.Container()")


class TestIOStream(TestIOStreamBase):
    """Test return value policy features on std::iostream-like object.
    Actually runs tests on a class derived from `std::stringstream`"""
    def test_weakref_iostream(self):
        iostream = self.container.iostream
        ios2 = iostream
        del self.container
        self.assertEqual(ios2, iostream)

    def test_iostream_features(self):
        #This also works
        buf = self.container.iostream
        print "IOstream state:-"
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        gc.collect()

    def test_iostream_init_w_string(self):
        from iostreams import IOStream
        self.assertRaises(TypeError, IOStream, "foo bar baz" )

    def test_iostream_init_w_bytearray(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("foo bar baz"))
        self.assertEqual(str(buf), "foo bar baz")

    def _test_make_5_iostreams(self):
        from iostreams import Container
        buffered_objects = []
        for i in xrange(5):
            container = Container()
            buffered_objects.append( container.iostream )
        print "made 5 iostreams"
        del(buffered_objects[4])

class TestIStreamInstance(unittest.TestCase):
    def setUp(self):
        from iostreams import IStream
        self.istream = IStream

    def test_weakref_istream(self):
        istream = self.istream("foo bar baz")
        is2 = istream
        del self.istream
        print is2
        return istream

    def test_istream_class_features(self):
        buf = self.istream
        print "\nIstream Class state:-"
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   cl    : {0}".format(buf.__class__)
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        gc.collect()

    def test_istream_instance_features(self):
        buf = self.istream("foo bar baz")
        print "\nIstream Instance state:-"
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   cl    : {0}".format(buf.__class__)
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        gc.collect()

class TestIOStreamInstance(unittest.TestCase):
    def setUp(self):
        from iostreams import IOStream
        self.iostream = IOStream()

    def test_weakref_iostream(self):
        iostream = self.iostream
        ios2 = iostream
        del self.iostream
        print ios2
        return iostream

    def test_iostream_class_features(self):
        #This also works
        buf = self.iostream.__class__
        print "\nIOstream Class state:-"
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   cl    : {0}".format(buf.__class__)
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        print " type(cl): {0}".format(type(buf.__class__))
        gc.collect()

    def test_iostream_instance_features(self):
        #This also works
        buf = self.iostream
        print "\nIOstream Instance state:-"
        print "   Size  : {0}".format(sys.getsizeof(buf))
        print "   Dir   : {0}".format(dir(buf))
        print "   Dir(cl):{0}".format(dir(buf.__class__))
        print "   repr  : {0}".format(repr(buf))
        print "   str   : {0}".format(str(buf))
        print "   type  : {0}".format(type(buf))
        gc.collect()

class TestDerivedIOStream(unittest.TestCase):
    """Run tests on a class derived from an IOstream, and registered with 
    bp::class_<.., boost::python::bases<..> >"""
    def setUp(self):
        from iostreams import DerivedIOStream
        self.cl = DerivedIOStream

    def test_init_gc(self):
        inst = self.cl()

if __name__ == "__main__":
    unittest.main()

