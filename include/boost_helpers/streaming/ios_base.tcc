namespace boost { namespace python { namespace converter {

    // ---        IOstream_base template member definitions        --- //

    // IOstream_base ctors
    template <class Pointee, class DerivedPolicies>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(void)
    {
        printf("In iostream_base initailiser void\n");
    }

    template <class Pointee, class DerivedPolicies>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(const char* s)
    {
        printf("In iostream_base initailiser 2\n");
    }

    template <class Pointee, class DerivedPolicies>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(char const* start,
                                                           char const* finish)
    {
        printf("In iostream_base initailiser 3\n");
    }

    template <class Pointee, class DerivedPolicies>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(char const* start, std::size_t length)
    {
        printf("In iostream_base initailiser 4\n");
    }

    template <class Pointee, class DerivedPolicies>
    template <class T>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(T const& other)
    {
        printf("In iostream_base templated initailiser\n");
    }

    //@{
    /// Boost Python functions

    template <class Pointee, class DerivedPolicies>
    PyTypeObject const * iostream_base<Pointee, DerivedPolicies>::get_pytype()
    {
        printf("in ios_base::get_pytype()\n");
        return &DerivedPolicies::m_type;
    }

    // construct
    //
    // from_python converter requirement
    template <class Pointee, class DerivedPolicies>
    void iostream_base<Pointee, DerivedPolicies>::construct(
        PyObject* ptr, converter::rvalue_from_python_stage1_data* data)
    {
        printf("In iostream_base::construct\n");
        void * storage =
            ((converter::rvalue_from_python_storage<Pointee>*)data)->storage.bytes;
        extract<const Pointee&> stream(ptr);
        new (storage) Pointee();
        data->convertible = storage;
    }

    // convert
    //
    // to_python converter requirement
    template <class Pointee, class DerivedPolicies>
    PyObject * iostream_base<Pointee, DerivedPolicies>::convert(Pointee const& value)
    {
        printf("In iostream_base convert\n");
        typename DerivedPolicies::value_type * new_obj = PyObject_New(
            typename DerivedPolicies::value_type, &DerivedPolicies::m_type);
        new_obj->m_weakrefs  = NULL;
        new_obj->m_stream = const_cast<Pointee*>(&value);
        new_obj->m_new    = false;
        new_obj->buf      = NULL;
        PyObject * obj    = reinterpret_cast<PyObject*>(new_obj);
        return obj;
    }

    // convertible
    //
    // to_python converter requirement
    template <class Pointee, class DerivedPolicies>
    void * iostream_base<Pointee, DerivedPolicies>::convertible(
        PyObject* ptr)
    {
        printf("checking iostream-base convertible\n");
        if (Py_TYPE(ptr)->tp_as_buffer != NULL)
            return ptr;
    }

    // Call operator. Does this ever get used?
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream_base<Pointee, DerivedPolicies>::operator()(void)
    {
        printf("calling IOstream_base\n");
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
    PyObject* iostream_base<Pointee, DerivedPolicies>::operator()(Pointee& other)
    {
        printf("calling iostream_base with a %s\n", typeid(other).name());
        if (m_type.tp_name == 0)
        {
            m_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&m_type) < 0)
            {
                throw error_already_set();
            }
        }
    }
    //@}

    //@{
    /// PyTypeObject functions

    // p_new
    //
    // Initialises ios_base class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // 'newfunc'.
    template <class Pointee, class DerivedPolicies>
    typename iostream_base<Pointee, DerivedPolicies>::value_type *
    iostream_base<Pointee, DerivedPolicies>::p_new(
        object_type  * subtype,
        PyObject     * args,
        PyObject     * kwds)
    {
        printf("In iostream_base::p_new\n");
        value_type * self = PyObject_NEW(value_type, &DerivedPolicies::m_type);
        if (self != NULL && !PyErr_Occurred())
        {
            self->m_weakrefs = NULL;
            self->m_stream   = new Pointee();
            self->buf        = NULL;
            self->m_new      = true;
        }
        return self;
    }

    // p_dealloc
    template <class Pointee, class DerivedPolicies>
    void iostream_base<Pointee, DerivedPolicies>::p_dealloc(value_type * self)
    {
        printf("In iostream_base::p_dealloc\n");
        // Deallocate temporaries if needed, but do not begin destruction just yet
        if (self->m_weakrefs != NULL)
        {
            PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(self));
        }
        Py_TYPE(self)->tp_free(self);
    }

    // p_free
    template <class Pointee, class DerivedPolicies>
    void iostream_base<Pointee, DerivedPolicies>::p_free(value_type * self)
    {
        //printf("In iostream_base::p_free\n");
        if (self->m_new == true)
            delete self->m_stream;
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream_base<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        //Pointee * stream = self->m_stream;
# if PY_VERSION_HEX < 0x03000000
        return PyString_FromFormat(
# else
        return PyUnicode_FromFormat(
# endif
            "<Boost.Python.IOstream_base class, at %p>",
            (void*)self->m_stream);
    }
    //@}

# if PY_VERSION_HEX >= 0x02060000
    //@{
    /// PyBufferProcs members
    template <class Pointee, class DerivedPolicies>
    int iostream_base<Pointee, DerivedPolicies>::p_getbuf(
        value_type * self, Py_buffer * view, int flags)
    {
        printf("In iostream_base::p_getbuf\n");
        return PyBuffer_FillInfo(view, reinterpret_cast<PyObject*>(self),
                                 self->buf, Py_SIZE(self), 0, flags);
    }

    template <class Pointee, class DerivedPolicies>
    void iostream_base<Pointee, DerivedPolicies>::p_releasebuf(
        value_type * self, Py_buffer * view)
    {
        printf("In iostream_base::p_releasebuf\n");

    }
    //@}
# endif

    //@{
    /// PyTypeObject static members

    // Struct to provide Python-side ostreaming support
    template <class Pointee, class DerivedPolicies>
    PyBufferProcs iostream_base<Pointee, DerivedPolicies>::m_stream =
    {
# if PY_VERSION_HEX < 0x03000000
        NULL, // bf_getreadbuffer
        NULL, // bf_getwritebuffer
        NULL, // bf_getsegcount
        NULL, // bf_getcharbuffer
# endif
# if PY_VERSION_HEX >= 0x02060000
        (getbufferproc)&p_getbuf,           // bf_getbuffer
        (releasebufferproc)&p_releasebuf    // bf_releasebuffer
# endif
    };

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods iostream_base<Pointee, DerivedPolicies>::m_sequence = {
        NULL, // sq_length
        NULL, // sq_concat
        NULL, // sq_repeat
        NULL, // sq_item
        NULL, // sq_slice
        NULL, // sq_ass_item
        NULL, // sq_ass_slice
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods iostream_base<Pointee, DerivedPolicies>::m_mapping = {
        NULL, // mp_length
        NULL, // mp_subscript
        NULL, // mp_ass_subscript
    };

    // m_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    template <class Pointee, class DerivedPolicies>
    PyMemberDef iostream_base<Pointee, DerivedPolicies>::m_members[] =
    {
        { 
          const_cast<char*>("__weakref__"), T_OBJECT,
          offsetof(value_type, m_weakrefs), 0
        },
        {0}
    } ;

    PyDoc_STRVAR( iostream_base_doc, "Boost Python wrapped iostream-like object.");

    // m_type
    //
    // The PyTypeObject struct that manages each ostreamed PyObject instance.
    template <class Pointee, class DerivedPolicies>
    PyTypeObject iostream_base<Pointee, DerivedPolicies>::m_type =
    {
        PyVarObject_HEAD_INIT(NULL, 0)
        0 //const_cast<char*>("Boost.Python.IOstream_base") // tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , (destructor)&p_dealloc                    // tp_dealloc
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
        , 0                                         // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_stream                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT                        //< tp_flags
          | Py_TPFLAGS_BASETYPE  
# if PY_VERSION_HEX < 0x03000000
          | Py_TPFLAGS_HAVE_WEAKREFS
#   if PY_VERSION_HEX >= 0x02060000
          | Py_TPFLAGS_HAVE_NEWBUFFER
#   endif
# endif
        , iostream_base_doc                         // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , offsetof(value_type, m_weakrefs)          // tp_weaklistoffset
        , 0                                         // tp_iter
        , 0                                         // tp_iternext
        , 0                                         // tp_methods
        , m_members                                 // tp_members
        , 0                                         // tp_getset
        , &PyBaseObject_Type                        // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , 0                                         // tp_dictoffset
        , 0                                         // tp_init
        , 0                                         // tp_alloc
        , (newfunc)&p_new                           // tp_new
        , (freefunc)&p_free                         // tp_free
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
    //@}

}   }   }
