namespace boost { namespace python { 

    // ---        Buffer template member definitions        --- //

    // buffer ctors
    template <class Pointee, class DerivedPolicies>
    iostream<Pointee, DerivedPolicies>::iostream(void)
    {
        printf("In iostream initailiser void\n");
    }

    template <class Pointee, class DerivedPolicies>
    iostream<Pointee, DerivedPolicies>::iostream(const char* s)
    {
        printf("In iostream initailiser 2\n");
    }


    template <class Pointee, class DerivedPolicies>
    iostream<Pointee, DerivedPolicies>::iostream(char const* start, char const* finish)
    {
        printf("In iostream initailiser 3\n");
    }

    template <class Pointee, class DerivedPolicies>
    iostream<Pointee, DerivedPolicies>::iostream(char const* start, std::size_t length)
    {
        printf("In iostream initailiser 4\n");
    }

    template <class Pointee, class DerivedPolicies>
    template <class T>
    iostream<Pointee, DerivedPolicies>::iostream(T const& other)
    {
        printf("In iostream templated initailiser\n");
    }

    // Call operator. Does this ever get used?
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream<Pointee, DerivedPolicies>::operator()(void)
    {
        printf("calling buffer\n");
        if (m_type.tp_name == 0)
        {
            m_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&m_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    // Another call operator. Does this get used?
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream<Pointee, DerivedPolicies>::operator()(Pointee& other)
    {
        printf("calling buffer with a %s\n", typeid(other).name());
        if (m_type.tp_name == 0)
        {
            m_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&m_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    // Create a new instance, from Python.
    // The default arguments for a Python buffer are:-
    //    buffer([object, [offset, [size]]])
    // Whereas the constructor for std::istream takes a streambuf pointer.
    // Don't see why we can't create the same Python constructor for IOStream 
    // Pointee's.
    template <class Pointee, class DerivedPolicies>
    int iostream<Pointee, DerivedPolicies>::p_init(value_type* self,
                                                   PyObject*   args,
                                                   PyObject*   kwds)
    {
        printf("In iostream::p_init!\n");
        Pointee * m_stream = self->m_stream;
        /// Copied from buffer_new() in Python 2.7.4's Objects/bufferobject.c
        void * buf;
        PyObject * obj    = Py_None;
        Py_ssize_t offset = 0;

# if PY_VERSION_HEX < 0x03000000

        Py_ssize_t size   = Py_END_OF_BUFFER;
        if (PyErr_WarnPy3k("buffer-like IOStreams not supported in Python 3.x", 1) < 0)
            return -1;
        if (!_PyArg_NoKeywords(DerivedPolicies::m_type.tp_name, kwds))
            return -1;
        if (!PyArg_ParseTuple(args, "|Onn:iostream", &obj, &offset, &size))
            return -1;

        // My own additions, to get that Python buffer into a C++ IOStream
        if (obj != Py_None)
        {
            obj = PyBuffer_FromReadWriteObject(obj, offset, size);
            if ((size = obj->ob_type->tp_as_buffer->bf_getwritebuffer(obj, 0, &buf)) < 0)
                return -1;
            m_stream->rdbuf()->str((typename Pointee::char_type*)buf);
        }
# else

# endif
        self->buf = buf;
        self->m_stream = m_stream;
        return 0;
    }

    // p_repr
    //
    // Return a string containing some information about the buffer instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        std::streampos pos;
        Pointee * m_iostream = self->m_stream;
        if ((pos = m_iostream->tellg()) < 0)
        {
            m_iostream->clear();
            pos = 0;
        }
# if PY_VERSION_HEX < 0x03000000
        return PyString_FromFormat(
# else
        return PyUnicode_FromFormat(
# endif
            "<Boost.Python.IOStream, size %lu, offset %i at %p>",
            istream_base::p_size(m_iostream),
            int(pos),
            (void*)m_iostream);
    }

    PyDoc_STRVAR( iostream_doc, "Boost Python wrapped iostream-like object.");

    // Struct to provide Python-side buffering support
    // Have to deal with multiple inheritance, so populate
    // istream<Pointee>::m_buffer with the missing member,
    // from ostream<Pointee>::m_buffer
    template <class Pointee, class DerivedPolicies>
    PyBufferProcs iostream<Pointee, DerivedPolicies>::m_buffer =
    {
# if PY_VERSION_HEX < 0x03000000
        (readbufferproc)  istream_base::p_readbuf,  // bf_getreadbuffer
        (writebufferproc) ostream_base::p_writebuf, // bf_getwritebuffer
        (segcountproc)    istream_base::p_segcount, // bf_getsegcount
        (charbufferproc)  istream_base::p_charbuf,  // bf_getcharbuffer
# endif
# if PY_VERSION_HEX >= 0x02060000
        (getbufferproc)    base_type::p_getbuf,    // bf_getbuffer
        (releasebufferproc)base_type::p_releasebuf // bf_releasebuffer
# endif
    };

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods iostream<Pointee, DerivedPolicies>::m_sequence = {
        (lenfunc)istream_base::p_length,                 // sq_length
        (binaryfunc)ostream_base::p_concat,              // sq_concat
        (ssizeargfunc)istream_base::p_repeat,            // sq_repeat
        (ssizeargfunc)istream_base::p_item,              // sq_item
        (ssizessizeargfunc)istream_base::p_slice,        // sq_slice
        (ssizeobjargproc)ostream_base::p_ass_item,       // sq_ass_item
        (ssizessizeobjargproc)ostream_base::p_ass_slice, // sq_ass_slice
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods iostream<Pointee, DerivedPolicies>::m_mapping = {
        (lenfunc)istream_base::p_length,              // mp_length
        (binaryfunc)istream_base::p_subscript,        // mp_subscript
        (objobjargproc)ostream_base::p_ass_subscript, // mp_ass_subscript
    };

    // m_type
    //
    // The PyTypeObject struct that manages each buffered PyObject instance.
    template <class Pointee, class DerivedPolicies>
    //PyTypeObject iostream<Pointee, DerivedPolicies>::m_type =
    typename iostream<Pointee, DerivedPolicies>::object_type
        iostream<Pointee, DerivedPolicies>::m_type =
    {
        PyVarObject_HEAD_INIT(NULL, 0)
        0 //const_cast<char*>("Boost.Python.IOStream")// tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , (destructor)&base_type::p_dealloc         // tp_dealloc
        , 0                                         // tp_print
        , 0                                         // tp_getattr
        , 0                                         // tp_setattr
        , 0                                         // tp_compare
        , (reprfunc)&p_repr                         // tp_repr
        , 0                                         // tp_as_number
        , &m_sequence                               // tp_as_sequence
        , &m_mapping                                // tp_as_mapping
        , 0                                         // tp_hash 
        , 0                                         // tp_call
        , (reprfunc)&istream_base::p_str            // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT                        //< tp_flags
          | Py_TPFLAGS_BASETYPE  
# if PY_VERSION_HEX < 0x03000000
          | Py_TPFLAGS_HAVE_GETCHARBUFFER 
          | Py_TPFLAGS_HAVE_WEAKREFS
#   if PY_VERSION_HEX >= 0x02060000
          | Py_TPFLAGS_HAVE_NEWBUFFER
#   endif
# endif
        , iostream_doc                              // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , offsetof(value_type, m_weakrefs)          // tp_weaklistoffset
        , 0                                         // tp_iter
        , 0                                         // tp_iternext
        , 0                                         // tp_methods
        , 0 //type_members                          // tp_members
        , 0                                         // tp_getset
        , &base_type::m_type                        // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , 0                                         // tp_dictoffset
        , (initproc)&p_init                         // tp_init
        , 0                                         // tp_alloc
        , (newfunc)&base_type::p_new                           // tp_new
        , (freefunc)&base_type::p_free              // tp_free
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

}  } // End Boost Python namespaces
