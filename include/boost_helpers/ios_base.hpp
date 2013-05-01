#ifndef __BOOST_PYTHON_IOS_BASE_INCLUDED__
#define __BOOST_PYTHON_IOS_BASE_INCLUDED__

#include <ios>

#include <boost/python/object.hpp>
#include <structmember.h>   ///< for T_OBJECT

namespace boost { namespace python { namespace converters {

    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // iostream_base<> template.
    //
    // A Python object instance holder, for C++ objects with iostream-like
    // interfaces.
    template <class Pointee>
    class iostream_base
        : public boost::python::object
    {
    public:
      typedef struct BoostPythonBuffer
      {
        PyObject_HEAD
        PyObject * b_base;
        PyObject * p_weakrefs;
        Pointee  * p_stream;
        //int b_readonly;
      } value_type;

      // ctor overloads, similar to `bp::str`.
      iostream_base();
      iostream_base(const char* s);
      iostream_base(char const* start, char const* finish);
      iostream_base(char const* start, std::size_t length);
      template <class T>
      explicit iostream_base(T const& other) ;
      // dtor
      ~iostream_base() { }

      // Generic PyTypeObject members:
      // Custom dealloc method for the ostreamed class.
      static void PyIOstream_base_Dealloc(value_type * obj);
      static PyObject* p_repr(value_type * self);

      // Objects providing support for Python iostream_base protocol:-
      static PyTypeObject    object_type;
      static PyMemberDef  type_members[];
      static PyBufferProcs      p_stream; 

      // PyBufferProcs functions enabling C++ iostream support in Python
      // iostream_base objects

      // caller
      PyObject* operator()(void);
      PyObject* operator()(Pointee& other);

      // Boost Python converter requirements
      static bool   convertible(void) { printf("checking iostream-base convertible\n"); return true; }
      static PyObject * convert(Pointee const& value);

      template <class Class>
      void visit(Class &cl) const { printf("in visit!\n"); }

  #ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      static PyTypeObject const* get_pytype()
      {
          printf("in iostream_base get_pytype\n");
          return &object_type;
      }
  #endif

    //BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(iostream_base<Pointee>, object)

    };

    template<class Pointee>
    PyObject * PyIOstream_base_New(PyTypeObject * subtype,
                                   PyObject     * args,
                                   PyObject     * kwds);


    // ---        IOstream_base template member definitions        --- //

    // IOstream_base ctors
    template <class Pointee>
    iostream_base<Pointee>::iostream_base(void)
    {
        printf("In initailiser void\n");
    }

    template <class Pointee>
    iostream_base<Pointee>::iostream_base(const char* s)
    {
        printf("In initailiser 2\n");
    }


    template <class Pointee>
    iostream_base<Pointee>::iostream_base(char const* start, char const* finish)
    {
        printf("In initailiser 3\n");
    }

    template <class Pointee>
    iostream_base<Pointee>::iostream_base(char const* start, std::size_t length)
    {
        printf("In initailiser 4\n");
    }

    template <class Pointee>
    template <class T>
    iostream_base<Pointee>::iostream_base(T const& other)
    {
        printf("In templated initailiser\n");
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee>
    PyObject* iostream_base<Pointee>::p_repr(value_type * self)
    {
        Pointee * stream = self->p_stream;
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        return PyString_FromFormat(
            "<Boost.Python.IOstream_base class, at %p>",
            //"<%s buffer for %p, size %zd, offset %zd at %p>",
            //status,
            //&stream,
            //BUFSIZ,
            //stream->tellg(),
            &stream);
    }

    // Struct to provide Python-side ostreaming support
    template <class Pointee>
    PyBufferProcs iostream_base<Pointee>::p_stream =
    {
        NULL,   // 
        NULL,   //
        NULL,   //
        NULL    //
    } ;

    // Call operator. Does this ever get used?
    template <class Pointee>
    PyObject* iostream_base<Pointee>::operator()(void)
    {
        printf("calling IOstream_base\n");
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
    PyObject* iostream_base<Pointee>::operator()(Pointee& other)
    {
        printf("calling iostream_base with a %s\n", typeid(other).name());
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
    PyObject * iostream_base<Pointee>::convert(Pointee const& value)
    {
      printf("In iostream_base convert\n");
      value_type * new_obj = PyObject_New(value_type, &object_type);
      new_obj->p_weakrefs  = NULL;
      new_obj->p_stream    = const_cast<Pointee*>(&value); //< This looks decidedly like a bad idea
      PyObject * py_obj    = reinterpret_cast<PyObject*>(new_obj);
      Py_INCREF(py_obj); //< When this line is commented, tp_dealloc is called. Uncommented, it's not.
      return py_obj;
    }

    // type_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    template <class Pointee>
    PyMemberDef iostream_base<Pointee>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
            iostream_base<Pointee>::value_type, p_weakrefs), 0},
        {0}
    } ;

    // PyOstream_Dealloc
    template <class Pointee>
    void iostream_base<Pointee>::PyIOstream_base_Dealloc(value_type * obj)
    {
      printf("In PyIOstream_base_Dealloc\n");
      // Allocate temporaries if needed, but do not begin destruction just yet
      if (obj->p_weakrefs != NULL)
      {
        PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
      }
      obj->ob_type->tp_free(obj);
    }

    PyDoc_STRVAR( iostream_doc, "Boost Python wrapped iostream-like object.");

    // object_type
    //
    // The PyTypeObject struct that manages each ostreamed PyObject instance.
    template <class Pointee>
    PyTypeObject iostream_base<Pointee>::object_type =
    {
        PyObject_HEAD_INIT(NULL)
        0                                           // ob_size
        , const_cast<char*>("Boost.Python.IOstream_base") // tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , (destructor)&PyIOstream_base_Dealloc      // tp_dealloc
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
        , iostream_doc                              // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , offsetof(value_type, p_weakrefs)          // tp_weaklistoffset
        , 0                                         // tp_iter
        , 0                                         // tp_iternext
        , 0                                         // tp_methods
        , type_members                              // tp_members
        , 0                                         // tp_getset
        , 0                                         // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , 0                                         // tp_dictoffset
        , 0                                         // tp_init
        , 0                                         // tp_alloc
        , (newfunc)&PyIOstream_base_New<Pointee>    // tp_new
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

    // PyIos_base_New
    //
    // Initialises ios_base class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // 'newfunc'.
    template<class Pointee>
    PyObject * PyIOstream_base_New(PyTypeObject * subtype,
                          PyObject     * args,
                          PyObject     * kwds)
    {
      printf("In PyIOstream_base_New\n");
      PyObject * obj = PyType_GenericNew(subtype, args, kwds);
      if (obj != NULL && !PyErr_Occurred()) // If created object ok
      {
        typedef typename iostream_base<Pointee>::value_type instance_t;
        instance_t * wrapped = reinterpret_cast<instance_t*>(obj);
        wrapped->p_weakrefs = NULL;
      }
      return obj;
    }

} } } // End Boost Python namespaces

#endif
