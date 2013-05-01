#ifndef __BOOST_PYTHON_OSTREAM_INCLUDED__
#define __BOOST_PYTHON_OSTREAM_INCLUDED__

#include "boost_helpers/ios_base.hpp"
#include <boost/python/object.hpp>
#include <structmember.h>   ///< for T_OBJECT
#include <ostream>


namespace boost { namespace python { namespace converters {

    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // ostream<> template.
    //
    // A Python object instance holder, for C++ objects with iostream-like
    // interfaces.
    template <class Pointee>
    class ostream
        : public iostream_base<Pointee>
    {
    public:

      typedef typename 
          boost::python::converters::iostream_base<Pointee>::value_type
              value_type;

      // ctor overloads, similar to `bp::str`.
      ostream();
      ostream(const char* s);
      ostream(char const* start, char const* finish);
      ostream(char const* start, std::size_t length);
      template <class T>
      explicit ostream(T const& other) ;
      // dtor
      ~ostream() { }

      // Custom dealloc method for the ostreamed class.
      static void PyOstream_Dealloc(value_type * obj);

      // Objects providing support for Python ostream protocol:-
      static PyTypeObject    object_type;
      static PyMemberDef  type_members[];
      static PyBufferProcs      p_stream; 

      // PyBufferProcs functions enabling C++ iostream support in Python ostream
      // objects
      static PyObject* p_repr(value_type * self);

      // TO TEST:
      static Py_ssize_t write_buffer(value_type* self, Py_ssize_t idx, void **pp);

      // TODO:
      static Py_ssize_t get_seg_count(value_type* self, Py_ssize_t * lenp);

      // caller
      PyObject* operator()(void);
      PyObject* operator()(Pointee& other);

      // Boost Python converter requirements
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

    //BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(ostream<Pointee>, object)

    };

    template<class Pointee>
    PyObject * PyOstream_New(PyTypeObject * subtype,
                           PyObject     * args,
                           PyObject     * kwds);


    // ---        ostream template member definitions        --- //

    // ostream ctors
    template <class Pointee>
    ostream<Pointee>::ostream(void)
    {
        printf("In initailiser void\n");
    }

    template <class Pointee>
    ostream<Pointee>::ostream(const char* s)
    {
        printf("In initailiser 2\n");
    }


    template <class Pointee>
    ostream<Pointee>::ostream(char const* start, char const* finish)
    {
        printf("In initailiser 3\n");
    }

    template <class Pointee>
    ostream<Pointee>::ostream(char const* start, std::size_t length)
    {
        printf("In initailiser 4\n");
    }

    template <class Pointee>
    template <class T>
    ostream<Pointee>::ostream(T const& other)
    {
        printf("In templated initailiser\n");
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee>
    PyObject* ostream<Pointee>::p_repr(value_type * self)
    {
        Pointee * stream = self->p_stream;
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        return PyString_FromFormat(
            "<Boost Python ostream, size %zd, offset %zd at %p>",
            //"<%s buffer for %p, size %zd, offset %zd at %p>",
            //status,
            //&stream,
            BUFSIZ,
            stream->tellp(),
            &stream);
    }

    // Call operator. Does this ever get used?
    template <class Pointee>
    PyObject* ostream<Pointee>::operator()(void)
    {
        printf("calling ostream\n");
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
    PyObject* ostream<Pointee>::operator()(Pointee& other)
    {
        printf("calling ostream with a %s\n", typeid(other).name());
        if (object_type.tp_name == 0)
        {
            object_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&object_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    // write_buffer
    //
    // Read data from pp into the wrapped class's internal strstream, by using
    // the strstream's << operator.
    // @param self - The PyObject instance holding a pointer to the C++ object
    //               The stream pointer should be in the objects p_stream member.
    // @param idx  - Segment of ostream ostream where to start writing.
    // @param pp   - Pointer to start of Python input ostream. This should be a
    //               null-terminated string.
    template <class Pointee>
    Py_ssize_t ostream<Pointee>::write_buffer(value_type* self,
                                             Py_ssize_t idx, void **pp)
    {
        Py_ssize_t size = 0;
        Pointee& m_stream = *self->p_stream;
        typename Pointee::pos_type start_pos = m_stream.tellp();
        m_stream.seekp(idx);
        std::ios_base::iostate state = m_stream.rdstate();
        switch (state)
        {
            case std::ios_base::eofbit:
                break;
            case std::ios_base::failbit:
                PyErr_SetString(PyExc_TypeError, "Stream sentry creation failed.");
                size = -1;
                break;
            case std::ios_base::badbit:
                PyErr_SetString(PyExc_SystemError, "Stream error.");
                size = -1;
                break;
            default:
                m_stream << *pp ;
                size = m_stream.tellp() - start_pos;
                break;
        }
        return size;
    }

    // convert
    //
    // to_python converter requirement
    template <class Pointee>
    PyObject * ostream<Pointee>::convert(Pointee const& value)
    {
      printf("In ostream convert\n");
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
    Py_ssize_t ostream<Pointee>::get_seg_count(
        value_type* self, Py_ssize_t *lenp)
    {
        printf("in get_seg_count\n");
        return 0;
    }

    // type_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    template <class Pointee>
    PyMemberDef ostream<Pointee>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
            ostream<Pointee>::value_type, p_weakrefs), 0},
        {0}
    } ;

    // PyOstream_Dealloc
    template <class Pointee>
    void ostream<Pointee>::PyOstream_Dealloc(value_type * obj)
    {
      printf("In PyOstream_Dealloc\n");
      // Allocate temporaries if needed, but do not begin destruction just yet
      if (obj->p_weakrefs != NULL)
      {
        PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
      }
      obj->ob_type->tp_free(obj);
    }

    PyDoc_STRVAR( ostream_doc, "Boost Python wrapped ostream-like object.");

    // Struct to provide Python-side ostreaming support
    template <class Pointee>
    PyBufferProcs ostream<Pointee>::p_stream =
    {
        NULL,
        (writebufferproc) ostream<Pointee>::write_buffer,
        (segcountproc)    ostream<Pointee>::get_seg_count,
        NULL
    } ;

    // object_type
    //
    // The PyTypeObject struct that manages each ostreamed PyObject instance.
    template <class Pointee>
    PyTypeObject ostream<Pointee>::object_type =
    {
        PyObject_HEAD_INIT(NULL)
        0                                           // ob_size
        , const_cast<char*>("Boost.Python.Ostream") // tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , (destructor)&PyOstream_Dealloc            // tp_dealloc
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
        , 0                                         // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &p_stream                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT
          | Py_TPFLAGS_BASETYPE  
          | Py_TPFLAGS_HAVE_WEAKREFS                //< tp_flags
        , ostream_doc                               // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , offsetof(value_type, p_weakrefs)      // tp_weaklistoffset
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
        , (newfunc)&PyOstream_New<Pointee>          // tp_new
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

    // Ostream_New
    //
    // Initialises ostream class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // 'newfunc'.
    template<class Pointee>
    PyObject * PyOstream_New(PyTypeObject * subtype,
                             PyObject     * args,
                             PyObject     * kwds)
    {
      printf("In PyOstream_New\n");
      PyObject * obj = PyType_GenericNew(subtype, args, kwds);
      if (obj != NULL && !PyErr_Occurred()) // If created object ok
      {
        typedef typename ostream<Pointee>::value_type instance_t;
        instance_t * wrapped = reinterpret_cast<instance_t*>(obj);
        wrapped->p_weakrefs = NULL;
      }
      return obj;
    }

}  }  } // End Boost Python namespaces

#endif
