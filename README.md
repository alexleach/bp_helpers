#  bp_helpers

###### Reusable code for exposing C++ code to Python, using Boost Python


## Introduction

These are some pieces of semi-resuable code I've been using, whilst exposing
a C++ library to Python. The most reusable pieces of code here are no
doubt the parts that help expose C++ standards-compliant classes.

This little repository contains several C++ headers for use with Boost
Python, a couple of source files, as well as a Makefile and some
Python unit tests.


### Getting started

Well, the code here is only going to be useful if you've already got 
started with Boost Python. Still, if you want to test that it all works,
or test it after playing around with it, you can just run the Makefile, 
which will compile every C++ test file in src/tests/, by running a relevant 
Python unit-test module.

The Python unittests are provided in the `tests/` directory. Each unittest
module is configured to compile the relevant C++ code, using Python's distutils, 
before running tests on the exposed interfaces.


## Contents
============

### Exposing C++ STL iostream's
===============================

This became a project in itself. The [C++ ios library] contains a fairly large
collection of abstract and specialised classes. Initially, I used a [single
header file][buffer.hpp] in an attempt to have a generic template that could be 
used to wrap any standard-compliant iostream. A complimentary header file was
written, containing both a Boost Python [CallPolicy] and [HolderGenerator].
These are described below, and example usage is demonstrated in
[return_buffer_object.cpp] and [test_buffer_object.py].

#### New iostream wrappers
--------------------------

It soon became apparent that one template class was not enough to cover all the
bases of the C++ iostream library. The new bp_helper wrappers for the iostreams
are now documented in a [separate page][iostream wrappers].

#### Old iostream wrapper
-------------------------

- Header Files:-
  - [buffer.hpp]

    Defines a `buffer<>` template `ResultConverter`, allowing an iostream to
    be exposed with a custom `PyTypeObject`. This is the object that manages
    and describes each Python object instance to the C-API. The `PyTypeObject`
    exposed here was designed to conform to Python's 
    ["old style buffer object"s][1].
  - [return_buffer_object.hpp]

    Defines a `return_value_policy` and `HolderGenerator` for the `buffer<>`
    template. These can be used by Boost Python code to wrap iostream-like 
    objects to a `PyTypeObject`'s internal `PyBufferProcs` struct. This 
    essentially enables buffering at Python's C-API level.


### Exposing std::list
======================

- [make_list.hpp]

  Wraps std::list

- [make_callback.hpp]

  Code to help call a Python function from C++ (in the `sort` function).
  Probably duplicated functionality...


### Thread safety
=================

- [make_threadsafe.hpp]

  RAII for Python's `PyGILState_STATE` and `PyThreadState` objects.


### Misc
========

- [converters.hpp]

  Some converters used to get a `const char* const*` from various Python objects

- [get_object_id.hpp]

  Get internal ID of python object, like id(obj) does, from python. I think this is 
  already available in Boost Python, so should probably delete this and figure out 
  how to use that instead.

- [make_constructor.hpp]

  Use a raw Python argument signature when initialising a class, ie. `Foo(*args, **kwds)`

- [make_submodules.hpp]

  Macros to call multiple Boost Python registration functions, within a module scope

- [refcounted_object.hpp]

  This was inspired from somewhere. Don't actually use it currently..

### Unit Tests
==============

- [tests/test_buffer_object.py]
- [tests/test_make_list.py]
- [tests/test_refcounted_object.py]

Each unittest can be run directly, using Python. This will compile the relevant C++ code,
before proceding to run the unittests.

### TODO
========

- [buffer.hpp]

Should be able to choose from a few specialisations of STD streams: read-only, read-write,
binary and seekable. Read-only and read-write cannot be tested for at run-time, so 
specialisations will need to be chosen at compile-time. Making a template that can decide 
this automatically is no doubt possible, but is beyond my current level of knowledge of C++.

Not only that, but the `PyTypeObject` underlying the buffer template is still missing an
implementation of the built-in function: `tp_hash`.

The `PyBufferProcs` object still needs working `segcountproc`, and `charbufferproc`
implementations.


- test_all.py

Should probably write a global unittest file, instead of using a Makefile. This should make
everything here properly platform independent.

[1]: http://docs.python.org/2/c-api/buffer.html#old-style-buffer-objects

[C++ io library]: http://www.cplusplus.com/reference/ios/
[CallPolicy]: http://www.boost.org/doc/libs/1_53_0/libs/python/doc/v2/CallPolicies.html#CallPolicies-concept
[ResultConverter]: http://www.boost.org/doc/libs/1_53_0/libs/python/doc/v2/ResultConverter.html#ResultConverter-concept
[ResultConverterGenerator]: http://www.boost.org/doc/libs/1_53_0/libs/python/doc/v2/ResultConverter.html#ResultConverterGenerator-concept

[iostream wrappers]: ./IOSTREAM.md
[buffer.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/buffer.hpp
[converters.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/converters.hpp
[get_object_id.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/get_object_id.hpp
[return_buffer_object.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/return_buffer_object.hpp
[refcounted_object.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/refcounted_object.hpp
[make_constructor.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/make_constructor.hpp
[make_submodules.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/make_submodules.hpp
[make_threadsafe.hpp]: https://github.com/alexleach/bp_helpers/blob/master/include/boost_helpers/make_threadsafe.hpp

[buffer.cpp]: https://github.com/alexleach/bp_helpers/blob/master/src/buffer.cpp
[return_buffer_object.cpp]: https://github.com/alexleach/bp_helpers/blob/master/src/return_buffer_object.cpp
[make_list.hpp]: https://github.com/alexleach/bp_helpers/blob/master/src/make_list.cpp
[make_callback.hpp]: https://github.com/alexleach/bp_helpers/blob/master/src/make_callback.cpp

[tests/test_buffer_object.py]: http://github.com/alexleach/bp_helpers/blob/master/tests/test_buffer_object.py
[tests/test_make_list.py]: http://github.com/alexleach/bp_helpers/blob/master/tests/test_make_list.py
[tests/test_refcounted_object.py]: http://github.com/alexleach/bp_helpers/blob/master/tests/test_refcounted_object.py

