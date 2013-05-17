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

# handy switch to turn off all this output
PRINT_INFO = True

def print_info(buf):
    if PRINT_INFO:
        print( "   Size  : {0}".format(sys.getsizeof(buf)))
        print( "   Dir   : {0}".format(dir(buf)) )
        print( "   cl    : {0}".format(buf.__class__) )
        print( "   Dir(cl):{0}".format(dir(buf.__class__)) )
        print( "   repr  : {0}".format(repr(buf)) )
        print( "   str   : {0}".format(str(buf)) )
        print( "   type  : {0}".format(type(buf)) )

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
        print(istream)
        print(is2)
        return istream

    def test_istream_features(self):
        #This also works
        buf = self.container.istream
        print("Istream state:-")
        print_info(buf)
        gc.collect()

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
        print(os2)
        return ostream

    def test_ostream_features(self):
        #This also works
        buf = self.container.ostream
        print("Ostream state:-")
        print_info(buf)
        gc.collect()

    def test_get_ostream(self):
        buf = self.container.get_ostream()
        del(buf)

    def test_ostream_concat(self):
        buf = self.container.get_ostream()
        buf2 = self.container.ostream
        buf = buf + buf2

    def test_ostream_setitem(self):
        # Can write to it, but we can't read it!
        buf = self.container.get_ostream()
        buf[0:4] = "foo "
        buf[0]  = "b"

    def test_ostream_write(self):
        buf = self.container.ostream
        buf.write("written into the buffer")

    def _test_default_get_ostream(self):
        default_buf = self.container.get_internal_reference_ostream()
        buf = self.container.get_ostream()
        getsize = sys.getsizeof
        self.assertGreater(getsize(default_buf), getsize(buf))

    def _test_make_5_ostreams(self):
        from iostreams import Container
        buffered_objects = []
        for i in xrange(5):
            container = Container()
            buffered_objects.append( container.ostream )
        print("made 5 ostreams")
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

    def test_iostream_features(self):
        #This also works
        buf = self.container.iostream
        print("IOstream state:-")
        print_info(buf)
        gc.collect()

    def test_iostream_init_w_string(self):
        # string is immutable. The "O" in IO stream doesn't like that!
        from iostreams import IOStream
        self.assertRaises(BufferError, IOStream, "foo bar baz" )

    def test_iostream_init_w_bytearray(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("foo bar baz"))
        self.assertEqual(str(buf), "foo bar baz")

    def test_len(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("foo bar"))
        self.assertEqual(len(buf), len("foo bar"))

    def test_iostream_delitem(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("abcdef"))
        del(buf[1])
        _cmp = bytearray("abcdef")
        del(_cmp[1])
        self.assertEqual(str(buf), str(_cmp))

    def test_iostream_delslice(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("foo bar"))
        _cmp = bytearray("foo bar")
        del(buf[2:4])
        del(_cmp[2:4])
        self.assertEqual(str(buf), str(_cmp))

    def test_iostream_delslice_step(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("abcdefghijkl"))
        _cmp = bytearray("abcdefghijkl")
        _slice = slice(2,11,5)
        del(buf[_slice])
        del(_cmp[_slice])
        self.assertEqual(str(buf), str(_cmp))

    def test_iostream_setitem(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("foo"))
        buf[0] = "b"
        self.assertEqual(str(buf), "boo")
        buf[-1]  = "f"
        self.assertEqual(str(buf), "bof")

    def test_iostream_setslice(self):
        from iostreams import IOStream
        buf = IOStream(bytearray("foo bar baz"))
        buf[:3] = "baz"
        self.assertEqual(str(buf), "baz bar baz")
        buf[-1] = 'y'
        print('buffer: ', buf)
        size = 5
        offset = len(buf) + size
        buf[offset:offset+size] = "x" * size
        print('buffer: ', buf)
        buf[offset:offset] = "x" * size
        print('buffer: ', buf)
        del(buf)
        print ''

    def test_write(self):
        iostream = self.container.iostream
        iostream.write("this has been written")
        print iostream

    def test_weakref_iostream(self):
        iostream = self.container.iostream
        ios2 = iostream
        del self.container
        self.assertEqual(ios2, iostream)

    def _test_make_5_iostreams(self):
        from iostreams import Container
        buffered_objects = []
        for i in xrange(5):
            container = Container()
            buffered_objects.append( container.iostream )
        print("made 5 iostreams")
        del(buffered_objects[4])

class TestIStreamInstance(unittest.TestCase):
    def setUp(self):
        from iostreams import IStream
        self.istream = IStream

    def test_weakref_istream(self):
        istream = self.istream("foo bar baz")
        is2 = istream
        del self.istream
        print(is2)
        return istream

    def test_istream_class_features(self):
        buf = self.istream
        print("\nIstream Class state:-")
        print_info(buf)
        gc.collect()

    def test_istream_init_w_string(self):
        buf = self.istream("foo bar baz")
        self.assertEqual(str(buf), "foo bar baz")

    def test_istream_instance_features(self):
        buf = self.istream("foo bar baz")
        print("\nIstream Instance state:-")
        print_info(buf)
        gc.collect()

    def test_istream_length(self):
        buf = self.istream("foo bar")
        self.assertEqual(len(buf), len("foo bar"))

    def test_istream_repeat(self):
        buf = self.istream("foo ")
        self.assertEqual( buf * 3, "foo foo foo ")

    def test_istream_slice(self):
        buf = self.istream("foo bar baz")
        self.assertEqual(buf[-3:],  "baz")

    def test_istream_slice_fail(self):
        buf = self.istream("foo bar baz")
        self.assertEqual(buf[-3:3], "")

    def test_istream_subscript(self):
        buf = self.istream("foo bar baz")
        self.assertEqual(buf[0], "f")

    def test_istream_subscript_string_fails(self):
        buf = self.istream("foo bar baz")
        self.assertRaises(TypeError, buf.__getitem__, "foo")

class TestDerivedBaseIOStream(unittest.TestCase):
    """Run tests on a class derived from an IOstream, and registered with 
    boost::python::bases<..> in the class_<> instantiation."""
    def setUp(self):
        from iostreams import DerivedBaseIOStream
        self.cl = DerivedBaseIOStream

    def test_init_gc(self):
        buf = self.cl()

    def test_features(self):
        buf = self.cl()
        print_info(buf)

class TestDerivedIOStream(unittest.TestCase):
    """Run tests on a class derived from an IOstream, but not registered with 
    boost::python::bases<..>.
    When test_features runs, it can be seen that instance sizes are twice the
    size of instances created when using bases<..>"""
    def setUp(self):
        from iostreams import DerivedIOStream
        self.cl = DerivedIOStream

    def test_init_gc(self):
        buf = self.cl()

    def test_features(self):
        buf = self.cl()
        print_info(buf)

class TestBufferIOStream(unittest.TestCase):
    """Test that a normal buffer() object can understance a wrapped IOStream"""
    def make_iostream(self):
        from iostreams import IOStream
        return IOStream(bytearray("foo bar baz"))

    def test_init(self):
        x = buffer(self.make_iostream())

    def test_index(self):
        buf = buffer(self.make_iostream())
        iostream = self.make_iostream()
        self.assertEqual(buf[1], iostream[1])
        self.assertEqual(buf[-2], iostream[-2])

    def test_slice(self):
        buf = buffer(self.make_iostream())
        iostream = self.make_iostream()
        self.assertEqual(buf[0:3], iostream[0:3])
        self.assertEqual(buf[-3:], iostream[-3:])

    def test_str(self):
        buf = buffer(self.make_iostream())
        iostream = self.make_iostream()
        self.assertEqual(str(buf), str(iostream))

class TestMemoryviewIOStream(unittest.TestCase):
    """Test that a normal buffer() object can understance a wrapped IOStream"""
    def init_iostream(self):
        from iostreams import IOStream
        return IOStream(bytearray("foo bar baz"))

    def return_iostream(self):
        from iostreams import Container
        return Container().iostream

    def test_init(self):
        memview = memoryview(self.init_iostream())

    def test_index(self):
        memview = memoryview(self.init_iostream())
        iostream = self.init_iostream()
        self.assertEqual(memview[3], iostream[3])

    def test_slice(self):
        memview = memoryview(self.init_iostream())
        iostream = self.init_iostream()
        self.assertEqual(memview[3:6], iostream[3:6])

    def test_return(self):
        from iostreams import IOStream, Container
        iostream = self.return_iostream()
        memview = memoryview(self.return_iostream())

    def test_return_index(self):
        memview = memoryview(self.return_iostream())
        iostream = self.return_iostream()
        self.assertEqual(memview[3], iostream[3])

    def test_return_slice(self):
        memview = memoryview(self.return_iostream())
        iostream = self.return_iostream()
        self.assertEqual(memview[3:6], iostream[3:6])

if __name__ == "__main__":
    unittest.main()

