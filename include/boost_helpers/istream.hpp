#ifndef __BOOST_PYTHON_BUFFER_INCLUDED__
#define __BOOST_PYTHON_BUFFER_INCLUDED__

#include "boost_helpers/ios_base.hpp"
#include <boost/python/object.hpp>
#include <structmember.h>   ///< for T_OBJECT
#include <istream>

namespace boost { namespace python { namespace converters {

    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // buffer<> template.
    //
    // A Python object instance holder, for C++ objects with iostream-like
    // interfaces.
    template <class Pointee>
    class istream
        : public iostream_base<Pointee>
    {
    public:

      typedef typename 
          boost::python::converters::iostream_base<Pointee>::value_type
              value_type;

      // ctor overloads, similar to string.
      istream() ; //{ printf("Initialising buffer object\n"); }
      istream(const char* s);
      istream(char const* start, char const* finish);
      istream(char const* start, std::size_t length);
      template <class T>
      explicit istream(T const& other) ;
      // dtor
      ~istream() { }

      // Custom dealloc method for the buffered class.
      static void PyIstream_Dealloc(value_type * obj);

      // Objects providing support for Python buffer protocol:-
      static PyTypeObject    object_type;
      static PyMemberDef  type_members[];
      static PyBufferProcs      p_buffer; 

      // PyBufferProcs functions enabling C++ iostream support in Python buffer
      // objects
      static Py_ssize_t read_buffer(value_type* self, Py_ssize_t idx, void **pp);

      // TODO:
      static Py_ssize_t get_seg_count(value_type* self, Py_ssize_t * lenp);
      static Py_ssize_t getcharbuf(value_type* self, Py_ssize_t idx, void *pp);

      // PyTypeObject members provided in PyBuffer_Type
      static long  int p_hash(value_type * self);
      static PyObject* p_str(value_type * self);

      // Generic PyTypeObject members
      static PyObject* p_repr(value_type * self);

      // caller
      PyObject* operator()(void);
      PyObject* operator()(Pointee& other);

      // Boost Python converter requirements
      //static bool   convertible(void) { printf("checking convertible\n"); return true; }
      static PyObject * convert(Pointee const& value);

      template <class Class>
      void visit(Class &cl) const { printf("in visit!\n"); }

  #ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      static PyTypeObject const* get_pytype()
      {
          printf("in get_pytype\n");
          return &object_type;
      }
  #endif

    //BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(istream<Pointee>, object)

    };

    template<class Pointee>
    PyObject * PyIstream_New(PyTypeObject * subtype,
                          PyObject     * args,
                          PyObject     * kwds);

    // ---        istream template member definitions        --- //

    // istream ctors
    template <class Pointee>
    istream<Pointee>::istream(void)
    {
        printf("In initailiser void\n");
    }

    template <class Pointee>
    istream<Pointee>::istream(const char* s)
    {
        printf("In initailiser 2\n");
    }


    template <class Pointee>
    istream<Pointee>::istream(char const* start, char const* finish)
    {
        printf("In initailiser 3\n");
    }

    template <class Pointee>
    istream<Pointee>::istream(char const* start, std::size_t length)
    {
        printf("In initailiser 4\n");
    }

    template <class Pointee>
    template <class T>
    istream<Pointee>::istream(T const& other)
    {
        printf("In templated initailiser\n");
    }

    // Struct to provide Python-side buffering support
    template <class Pointee>
    PyBufferProcs istream<Pointee>::p_buffer =
    {
        (readbufferproc)  istream<Pointee>::read_buffer,
        NULL,
        (segcountproc)    istream<Pointee>::get_seg_count,
        (charbufferproc)  istream<Pointee>::getcharbuf,
    } ;

    // read_buffer
    //
    // Reads data from internal stream into the array pointed to by pp
    template <class Pointee>
    Py_ssize_t istream<Pointee>::read_buffer(value_type* self,
                                            Py_ssize_t idx,
                                            void **pp)
    {
        if (idx != 0)
        {
          PyErr_SetString(PyExc_SystemError,
              "accessing non-existent istream segment.");
          throw_error_already_set();
        }
        Pointee& m_stream = *self->p_stream;
        /// Get state. On failure, state could be any of:-
        ///   eofbit  - no characters available to read
        ///   failbit - sentry construction failed
        ///   badbit  - stream error
        std::ios_base::iostate state = m_stream.rdstate();
        Py_ssize_t size = 0;
        switch (state)
        {
          case std::ios_base::eofbit:
              break;
          case std::ios_base::failbit:
              PyErr_SetString(PyExc_SystemError, "Stream sentry creation failed.");
              size = -1;
              break;
          case std::ios_base::badbit:
              PyErr_SetString(PyExc_SystemError, "Stream error.");
              size = -1;
              break;
          default:
              m_stream >> *pp ;
              size = m_stream.tellg();
              break;
        }
        return size;
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee>
    PyObject* istream<Pointee>::p_repr(value_type * self)
    {
        Pointee * stream = self->p_stream;
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        return PyString_FromFormat(
            "<Boost Python istream, size %zd, offset %zd at %p>",
            //"<%s buffer for %p, size %zd, offset %zd at %p>",
            //status,
            //&stream,
            BUFSIZ,
            stream->tellg(),
            &stream);
    }

    // p_str
    //
    // Read available in self's istream and return it as a Python string.
    template <class Pointee>
    PyObject* istream<Pointee>::p_str(value_type * self)
    {
      printf("in istream's p_str\n");
        Py_ssize_t size = 0;
        Pointee * stream = self->p_stream;
        std::streambuf * buf = stream->rdbuf();
        // Should make sure that the stream is not binary
        if (buf != NULL)
        {
            size = buf->in_avail();
            char ptr[size+1]; //< Is this safe across all platforms?
            size = stream->readsome(ptr, size); //< readsome is safer than getn
            //buf->sgetn(ptr, size);
            return PyString_FromStringAndSize(ptr, size);
        }
        return PyString_FromStringAndSize('\0', 0);
    }

    // Call operator. Does this ever get used?
    template <class Pointee>
    PyObject* istream<Pointee>::operator()(void)
    {
        printf("calling istream()\n");
        if (object_type.tp_name == 0)
        {
            object_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&object_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    // Another call operator. Does this get used?
    template <class Pointee>
    PyObject* istream<Pointee>::operator()(Pointee& other)
    {
        printf("calling istream with a %s\n", typeid(other).name());
        if (object_type.tp_name == 0)
        {
            object_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&object_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    // convert
    //
    // to_python converter requirement
    template <class Pointee>
    PyObject * istream<Pointee>::convert(Pointee const& value)
    {
      printf("In istream convert\n");
      value_type * new_obj = PyObject_New(value_type, &object_type);
      new_obj->p_weakrefs  = NULL;
      new_obj->p_stream    = const_cast<Pointee*>(&value); //< This looks decidedly like a bad idea
      PyObject * py_obj    = reinterpret_cast<PyObject*>(new_obj);
      Py_INCREF(py_obj); //< When this line is commented, tp_dealloc is called. Uncommented, it's not.
      return py_obj;
    }

    // get_seg_count
    //
    // PyBufferProcs requirement
    template <class Pointee>
    Py_ssize_t istream<Pointee>::get_seg_count(
        value_type* self, Py_ssize_t * lenp)
    {
        printf("in get_seg_count\n");
        return 0;
    }

    // getcharbuf
    //
    // Requirement of PyBufferProcs struct, when the PyTypeObject has
    // Py_TPFLAGS_HAVE_GETCHARBUFFER bit set in its tp_flags.
    template <class Pointee>
    Py_ssize_t istream<Pointee>::getcharbuf(
        value_type* self, Py_ssize_t idx, void *pp)
    {
        printf("in getcharbuf\n");
        return 0;
    }

    // p_hash
    // 
    // Return hashed representation of current istreamfor >30% faster inequality
    // testing.
    template <class Pointee>
    long int istream<Pointee>::p_hash(value_type * self)
    {
        printf("in p_hash\n");
        return 0;
    }

    // type_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    template <class Pointee>
    PyMemberDef istream<Pointee>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
            istream<Pointee>::value_type, p_weakrefs), 0},
        {0}
    } ;

    // PyIstream_Dealloc
    template <class Pointee>
    void istream<Pointee>::PyIstream_Dealloc(value_type * obj)
    {
      printf("In PyIstream_Dealloc\n");
      // Allocate temporaries if needed, but do not begin destruction just yet
      if (obj->p_weakrefs != NULL)
      {
        PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
      }
      obj->ob_type->tp_free(obj);
    }

    PyDoc_STRVAR( istream_doc, "Boost Python wrapped istream-like object.");

    // object_type
    //
    // The PyTypeObject struct that manages each buffered PyObject instance.
    template <class Pointee>
    PyTypeObject istream<Pointee>::object_type =
    {
        PyObject_HEAD_INIT(NULL)
        0                                           // ob_size
        , const_cast<char*>("Boost.Python.Istream") // tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , (destructor)&PyIstream_Dealloc            // tp_dealloc
        , 0                                         // tp_print
        , 0                                         // tp_getattr
        , 0                                         // tp_setattr
        , 0                                         // tp_compare
        , (reprfunc)&p_repr                         // tp_repr
        , 0                                         // tp_as_number
        , 0                                         // tp_as_sequence
        , 0                                         // tp_as_mapping
        , 0                                         // tp_hash 
        , 0                                         // tp_call
        , (reprfunc)&p_str                          // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &p_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT
          | Py_TPFLAGS_BASETYPE  
          | Py_TPFLAGS_HAVE_GETCHARBUFFER 
          | Py_TPFLAGS_HAVE_WEAKREFS                //< tp_flags
        , istream_doc                               // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , 0 //offsetof(value_type, p_weakrefs)          // tp_weaklistoffset
        , 0                                         // tp_iter
        , 0                                         // tp_iternext
        , 0                                         // tp_methods
        , type_members                              // tp_members
        , 0                                         // tp_getset
        , &iostream_base<Pointee>::object_type      // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , 0                                         // tp_dictoffset
        , 0                                         // tp_init
        , 0                                         // tp_alloc
        , (newfunc)&PyIstream_New<Pointee>          // tp_new
        , 0                                         // tp_free
        , 0                                         // tp_is_gc
        , 0                                         // tp_bases
        , 0                                         // tp_mro
        , 0                                         // tp_cache
        , 0                                         // tp_subclasses
        , 0                                         // tp_weaklist
#if PY_VERSION_HEX >= 0x02060000
        , 0                                         // tp_version_tag
#endif
#if PYTHON_API_VERSION >= 1012
        , 0                                         // tp_del
#endif
    };

    // PyIstream_New
    //
    // Initialises istream class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // `newfunc`.
    template<class Pointee>
    PyObject * PyIstream_New(PyTypeObject * subtype,
                             PyObject     * args,
                             PyObject     * kwds)
    {
      printf("In PyIstream_New\n");
      PyObject * obj = PyType_GenericNew(subtype, args, kwds);
      if (obj != NULL && !PyErr_Occurred()) // If created object ok
      {
        typedef typename istream<Pointee>::value_type instance_t;
        instance_t * wrapped = reinterpret_cast<instance_t*>(obj);
        wrapped->p_weakrefs = NULL;
      }
      return obj;
    }

}  }  } // End Boost Python Converter namespaces

#endif
