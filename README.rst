bp_helpers
==========


Introduction
------------

These are some pieces of semi-resuable code I've been using, whilst exposing
a C++ library to Python. The most reusable pieces of code here are no
doubt the parts that help expose standards-compliant templates.

This little repository contains several C++ headers for use with Boost
Python, a couple of source files, as well as a Makefile and some
Python unit tests.


Getting started
---------------

Well, the code here is only going to be useful if you've already got 
started with Boost Python. Still, if you want to test that it all works,
or test it after playing around with it, you can just run the Makefile, 
which will compile every C++ test file in src/tests/, by running a relevant 
Python unit-test module.

The Python unittests are provided in the tests/ directory. Each unittest
module is configured to compile the relevant C++ code, using Python's distutils, 
before running tests on the exposed interfaces.


Contents
--------

* Exposing C++ STL iostream's

  * buffer_pointer_converter.hpp
  Defines a return_value_policy where an iostream can be exposed like a Python buffer

  * return_buffer_object.hpp
  Defines a buffer<> template, which can be used to map STL iostreams to a PyTypeObject's
  PyBufferProcs struct.

And their corresponding source files:-

  * buffer_pointer_converter.cpp
  * return_buffer_object.cpp

* Exposing std::list

  * make_list.hpp
  Wraps std::list

  * make_callback.hpp
  Code to help call a Python function from C++ (in sort).
  Probably duplicated functionality...

* Thread safety

  * make_threadsafe.hpp
  RAII for Python GIL and ThreadState


* Misc

  * converters.hpp
  Some converters used to get a 'const char* const*' from various Python objects

  * get_object_id.hpp
  Get internal ID of python object, like id(obj) does, from python. I think this is 
  already available in Boost Python, so should probably delete this and figure out 
  how to use that instead.

  * make_constructor.hpp
  Use a raw Python argument signature when initialising a class, ie. `Foo(*args, **kwds)`

  * make_submodules.hpp
  Macros to call multiple Boost Python registration functions, within a module scope

  * refcounted_object.hpp
  This was inspired from somewhere. Don't actually use it currently..

Unit Tests
----------

  * tests/test_buffer_object.py
  * tests/test_make_list.py
  * tests/test_refcounted_object.py

Each unittest can be run directly, which will compile only the relevant C++ code and
run only the relevant tests.
