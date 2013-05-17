#  C++ iostream library wrappers
================================

###### Templates for exposing C++ iostream-like classes.

## Introduction

The C++98 standard defines a set of templates for working with input and output
streams. These provide a C++ programmer with safe, easy-to-use object interfaces
that can be used to manipulate standard input and output streams, files on disk,
etc. Even writing the simplest of `"hello, world!"` programs in C++, will often 
make use of classes defined in the system's iostream library implementation.

The iostream library is designed to be extended. One such reason for doing so
is to provide a means to 'serialise' objects or data streams between different
formats. From client code, this is very simple: a 'format flag' is passed to the
stream through one of the overloaded `>>` operators, and the next time the
stream is read from or written to, the requested format is used. The is a very
powerful technique in C++, allowing the use of a single class interface to
exchange data between any number of different formats.

The bp_helper/streaming sub-project contains class templates for exposing 
standards-compliant iostream classes to Python. The interface was written in
C++, using a combination of [Boost Python] and the [Python-C API].


## Example

From Python code, a simple interface is generated to work with iostreams; this
includes support for indexing, slicing, reading in to a string, and other
operations that depend on the type of stream being wrapped. An `iostream` is the
most flexible of the iostream library class templates, as this supports
reading, writing and seeking.

### From Python

```python
>>> # import extension class
>>> from my_iostreams import IOStream
>>> # create an IOStream instance from Python, with mutable object
>>> my_iostream_instance = IOStream(bytearray("Example buffer object"))

>>> print my_iostream_instance
Example buffer object
>>> print repr(my_iostream_instance)
<Boost.Python.IOStream, size 21, offset 0 at 0x1491f70>

>>> # replace "Example" with "your"
>>> my_iostream_instance[:4] = "your"
>>> del(my_iostream_instance[4:7]) # N.B. this has to copy the internal buffer.
>>> print my_iostream_instance
your buffer object
>>> print repr(my_iostream_instance)
<Boost.Python.IOStream, size 18, offset 0 at 0x1491f70>
```


### From C++

[test_iostreams.cpp] demonstrates how to wrap `std::istringstream`,
`std::ostringstream` and `std::stringstream`, and classes derived from them.
For completeness, and because I should also run some tests on them, I've also
written an example module showing how to wrap `std::fstream`. See the file
[src/tests/test_fstream.cpp] showing how this is done, and the file
tests/test_fstream.py to see example usage from Python.

## TODO

1. Make sure inheritance works properly for the object instance methods. This
   impacts a couple of things:-
  a. Intrinsic iostream methods, e.g. read() and write(), should be made
     available on the Python side. I have attempted to expose these within
     `iostream_base::extension_def`, but these methods don't appear anywhere!
  b. Classes derived from an iostream-like class and exposed to Python
     using bases<...> should include instance methods exposed in the derived
     class. Currently, those extra methods are added to the class object, but
     not to the class instances, which is where they should be.

[Boost Python]: http://www.boost.org/doc/libs/1_53_0/libs/python/doc/index.html
[test_iostreams.cpp]: ./src/tests/test_iostreams.cpp
[test_fstream.cpp]: ./src/tests/test_fstream.cpp
[Python-C API]: http://docs.python.org/2/c-api/
