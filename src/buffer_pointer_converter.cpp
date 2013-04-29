#include "boost_helpers/buffer_pointer_converter.hpp"

namespace boost { namespace python { 
    // ---        Template member definitions        --- //

    template <class Pointee>
    PyBufferProcs buffer<Pointee>::p_buffer =
    {
        (readbufferproc)  buffer<Pointee>::read_buffer,
        (writebufferproc) buffer<Pointee>::write_buffer,
        (segcountproc)    buffer<Pointee>::get_seg_count,
        (charbufferproc)  buffer<Pointee>::getcharbuf,
    } ;

    template <class Pointee>
    Py_ssize_t buffer<Pointee>::read_buffer(instance_type* self, Py_ssize_t idx, void **pp)
    {
        if (idx != 0)
        {
          PyErr_SetString(PyExc_SystemError,
              "accessing non-existent buffer segment.");
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

    template <class Pointee>
    PyObject* buffer<Pointee>::p_repr(instance_type * self)
    {
        Pointee * stream = self->p_stream;
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        return PyString_FromFormat(
            "<Boost Python Buffer, size %zd, offset %zd at %p>",
            //"<%s buffer for %p, size %zd, offset %zd at %p>",
            //status,
            //&stream,
            BUFSIZ,
            stream->tellg(),
            &stream);
    }

    // Read available data istream in return it as a Python string.
    template <class Pointee>
    PyObject* buffer<Pointee>::p_str(instance_type * self)
    {
        //char * ptr = 0;
        Py_ssize_t size = 0;
        Pointee * stream = self->p_stream;
        std::streambuf * buf = stream->rdbuf();
        // Should make sure that the stream is not binary
        if (buf != NULL)
        {
            size = buf->in_avail();
            printf("Creating buffer %lu long\n", size+1);
            char ptr[size+1];
            buf->sgetn(ptr, size);
            return PyString_FromStringAndSize(ptr, size);
        }
        return PyString_FromStringAndSize('\0', 0);
    }

    template <class Pointee>
    void buffer<Pointee>::operator()(void)
    {
        printf("calling buffer\n");
        if (type_object.tp_name == 0)
        {
            type_object.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&type_object) < 0)
            {
                throw error_already_set();
            }
        }
    }

    template <class Pointee>
    PyObject * buffer<Pointee>::convert(Pointee const& value)
    {
      printf("In convert\n");
      instance_type * new_obj = PyObject_New(instance_type, &type_object);
      new_obj->p_weakrefs     = NULL;
      new_obj->p_stream       = const_cast<Pointee*>(&value); //< This looks decidedly like a bad idea
      PyObject * py_obj = reinterpret_cast<PyObject*>(new_obj);
      //Py_INCREF(py_obj);
      return py_obj;
    }

    template <class Pointee>
    Py_ssize_t buffer<Pointee>::write_buffer(instance_type* self, Py_ssize_t idx, void **pp)
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
                size = m_stream.tellg() - start_pos;
                break;
        }
        return size;
    }

    template <class Pointee>
    Py_ssize_t buffer<Pointee>::get_seg_count(
        instance_type* self, Py_ssize_t idx, void *pp)
    {
        printf("in get_seg_count\n");
        return 0;
    }

    template <class Pointee>
    Py_ssize_t buffer<Pointee>::getcharbuf(
        instance_type* self, Py_ssize_t idx, void *pp)
    {
        printf("in getcharbuf\n");
        return 0;
    }

    template <class Pointee>
    long int buffer<Pointee>::p_hash(instance_type * self)
    {
        return 0;
    }

    template <class Pointee>
    PyTypeObject buffer<Pointee>::type_object =
    {
            PyObject_HEAD_INIT(NULL)
            0                                           // ob_size
            , const_cast<char*>("Boost.Python.Buffer")  // tp_name
            , sizeof(instance_type)                     // tp_basicsize
            , 0                                         // tp_itemsize
            , (destructor)&Buffer_Dealloc<instance_type>// tp_dealloc
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
            ,   Py_TPFLAGS_DEFAULT
              //| Py_TPFLAGS_BASETYPE  
              | Py_TPFLAGS_HAVE_GETCHARBUFFER 
              | Py_TPFLAGS_HAVE_WEAKREFS                //< tp_flags
            , "Boost Python buffered object"            // tp_doc
            , 0                                         // tp_traverse
            , 0                                         // tp_clear
            , 0                                         // tp_richcompare
            , offsetof(instance_type, p_weakrefs)       // tp_weaklistoffset
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
            , (newfunc)Buffer_New<instance_type>        // tp_new
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

    template <class Pointee>
    PyMemberDef buffer<Pointee>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
            buffer<Pointee>::instance_type, p_weakrefs), 0},
        {0}
    } ;

}   }