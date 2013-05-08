namespace boost { namespace python { 

    // ---        Buffer template member definitions        --- //

    // buffer ctors
    template <class Pointee, class DerivedPolicies>
    iostream<Pointee, DerivedPolicies>::iostream(void)
    {
        printf("Initialising iostream. inserting dynamic conversions\n");

        converter::registration& iostream_converter
            = const_cast<converter::registration&>(
                converter::registry::lookup(type_id<Pointee>()) );
        iostream_converter.m_class_object = &m_type;

        // Register the Base class
        objects::register_dynamic_id< istream_base >();
        objects::register_dynamic_id< ostream_base >();

        // Register the up-cast
        objects::register_conversion<container_type, istream_base >(false);
        objects::register_conversion<container_type, ostream_base >(false);


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

    // Struct to provide Python-side buffering support
    // Have to deal with multiple inheritance, so populate
    // istream<Pointee>::m_buffer with the missing member,
    // from ostream<Pointee>::m_buffer
    template <class Pointee, class DerivedPolicies>
    PyBufferProcs iostream<Pointee, DerivedPolicies>::m_buffer =
    {
        (readbufferproc)  istream_base::p_readbuf,
        (writebufferproc) ostream_base::p_writebuf,
        (segcountproc)    istream_base::p_segcount,
        (charbufferproc)  istream_base::p_charbuf,
    } ;

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
        printf("In IOstream::p_init!\n");
        Pointee * m_stream = self->m_stream;
        /// Copied from buffer_new() in Python 2.7.4's Objects/bufferobject.c
        PyObject * obj    = Py_None;
        Py_ssize_t offset = 0;
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
            printf("iostream::p_init - Creating new %s\n", typeid(Pointee).name());
            void * buf;
            obj = PyBuffer_FromReadWriteObject(obj, offset, size);
            if ((size = obj->ob_type->tp_as_buffer->bf_getwritebuffer(obj, 0, &buf)) < 0)
                return -1;
            m_stream->rdbuf()->str((typename Pointee::char_type*)buf);
        }

        self->m_stream = m_stream;
        return 0;
    }

    // p_new
    //
    // Initialises buffer class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // 'newfunc'.
    template<class Pointee, class DerivedPolicies>
    PyObject * iostream<Pointee, DerivedPolicies>::p_new(
        PyTypeObject * subtype, PyObject * args, PyObject * kwds)
    {
        printf("In iostream::p_new\n");
        value_type * obj = PyObject_NEW(value_type, &m_type);
        if (obj != NULL && !PyErr_Occurred()) // If created object ok
        {
            obj->m_weakrefs = NULL;
        }
        else
            throw_error_already_set();

        printf("iostream::p_new - Creating new %s\n", typeid(Pointee).name());
        obj->m_stream = new Pointee();
        return (PyObject*)obj;
    }

    // p_dealloc
    template <class Pointee, class DerivedPolicies>
    void iostream<Pointee, DerivedPolicies>::p_dealloc(value_type * obj)
    {
        printf("In PyIOStream::p_dealloc\n");
        // Allocate temporaries if needed, but do not begin destruction just yet
        if (obj->m_weakrefs != NULL)
        {
            PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
        }
        Py_TYPE(obj)->tp_free(obj);
    }

    // p_free
    template <class Pointee, class DerivedPolicies>
    void iostream<Pointee, DerivedPolicies>::p_free(value_type * obj)
    {
        printf("In PyIOStream::p_free\n");
        // Allocate temporaries if needed, but do not begin destruction just yet
        delete obj->m_stream;
    }

    // p_repr
    //
    // Return a string containing some information about the buffer instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        Pointee * stream = self->m_stream;
        size_t size;
        std::streampos pos;
        if (stream != NULL)
        {
            // Get the size and restore original position
            pos = stream->tellg();
            stream->seekg(0, stream->end);
            size = stream->tellg();
            stream->seekg(pos);
        }
        else
        {
            size = 0;
            pos  = 0;
        }
        return PyString_FromFormat(
            "<Boost.Python.IOStream, size %lu, offset %zd at %p>",
            size,
            pos,
            (PyObject*)self );
    }

    PyDoc_STRVAR( iostream_doc, "Boost Python wrapped iostream-like object.");

    // type_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    /*
    template <class Pointee>
    PyMemberDef iostream<Pointee>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
                  value_type, m_weakrefs), 0},
        {0}
    } ;
    */

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
        PyObject_HEAD_INIT(NULL)
        0                                           // ob_size
        , const_cast<char*>("Boost.Python.IOStream")// tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , (destructor)&p_dealloc                    // tp_dealloc
        , 0                                         // tp_print
        , 0                                         // tp_getattr
        , 0                                         // tp_setattr
        , 0                                         // tp_compare
        , (reprfunc)&p_repr                         // tp_repr
        , 0                                         // tp_as_number
        , &m_sequence                                         // tp_as_sequence
        , &m_mapping                                         // tp_as_mapping
        , 0                                         // tp_hash 
        , 0                                         // tp_call
        , (reprfunc)&istream_base::p_str            // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT
          | Py_TPFLAGS_BASETYPE  
          | Py_TPFLAGS_HAVE_GETCHARBUFFER 
          | Py_TPFLAGS_HAVE_WEAKREFS                //< tp_flags
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
        , (newfunc)&p_new                           // tp_new
        , (freefunc)&p_free                       // tp_free
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
