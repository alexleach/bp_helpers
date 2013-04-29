#ifndef __BOOST_PYTHON_BUFFER_POINTER_CONVERTER__
#define __BOOST_PYTHON_BUFFER_POINTER_CONVERTER__

#include <ios>

namespace boost { namespace python {

  ////////////////////////////////////////////////////////////////////////////
  // Register to- and from- Python conversions for a type Pointee

    // buffer template.
    //
    // A to-Python converter, for objects exposing an iostream-like interface.
    template <class Pointee>
        struct buffer
    {
      typedef struct BoostPythonBuffer
      {
        PyObject_HEAD
        PyObject * b_base;
        PyObject * p_weakrefs;
        Pointee  * p_stream;
        //int b_readonly;
      } instance_type;

      // ctor / dtor
      buffer(){ printf("Initialising buffer object\n"); }
      ~buffer() { }

      // Objects providing support for Python buffer protocol:-
      static PyTypeObject    type_object;
      static PyMemberDef  type_members[];
      static PyBufferProcs     p_buffer; 

      // PyBufferProcs functions enabling C++ iostream support in Python buffer
      // objects
      static Py_ssize_t read_buffer(instance_type* self, Py_ssize_t idx, void **pp);
      // TO TEST:
      static Py_ssize_t write_buffer(instance_type* self, Py_ssize_t idx, void **pp);
      // TODO:
      static Py_ssize_t get_seg_count(instance_type* self, Py_ssize_t idx, void *pp);
      static Py_ssize_t getcharbuf(instance_type* self, Py_ssize_t idx, void *pp);

      // PyTypeObject members functions provided in PyBuffer_Type
      static long  int p_hash(instance_type * self);
      static PyObject* p_repr(instance_type * self);
      static PyObject* p_str(instance_type * self);

      // caller
      void operator()(void);

      // Boost Python converter requirements
      static bool convertible(void) { return true; }
      static PyObject * convert(Pointee const& value);

  #ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      static PyTypeObject const* get_pytype()
      {
          printf("in get_pytype\n");
          return &type_object;
      }
  #endif

    };

    template<class Pointee>
    PyObject * Buffer_New(PyTypeObject * type,
                                     PyObject     * args,
                                     PyObject     * kwds);

    template <class Pointee>
    void Buffer_Dealloc(Pointee* obj);

    // Make a PyTypeObject that follows the buffer protocol.
    //
    // @param Pointee- C++ class to expose to Python
    // @param WrappedObject - A PyObject struct to hold the C++ class.
    template <class Pointee>
        PyTypeObject make_buffer_type_object(void);

} } // End Boost Python namespaces

#endif
