namespace boost { namespace python { namespace converter {

    // ---        IOstream_base template member definitions        --- //

    // IOstream_base ctors
    template <class Pointee, class DerivedPolicies>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(void)
        : object(handle<object_type>(borrowed(&DerivedPolicies::m_type)))
    {
        printf("In iostream_base initailiser void. Looking up %s\n",
            type_id<Pointee>().name() );

        // Insert type_id into registry
        converter::registration& stream_converter
            = const_cast<converter::registration&>(
                converter::registry::lookup(type_id<Pointee>()) );

        stream_converter.m_class_object = &DerivedPolicies::m_type;

        printf("inserting to_python converter\n");
        // Insert to_python converter into registry
        converter::registry::insert(
            (converter::to_python_function_t)&DerivedPolicies::convert,
            boost::python::type_id<Pointee>(),
            &DerivedPolicies::get_pytype );

        objects::register_dynamic_id<object_type>();

        if (PyType_Ready(&m_type) < 0)
            boost::python::throw_error_already_set();
        Py_INCREF(&m_type);
    }

    /*
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
    */

    template <class Pointee, class DerivedPolicies>
    template <class T>
    iostream_base<Pointee, DerivedPolicies>::iostream_base(T const& other)
    {
        printf("In iostream_base templated initailiser\n");
    }

    //@{
    /// Boost Python functions

    template <class Pointee, class DerivedPolicies>
    typename iostream_base<Pointee, DerivedPolicies>::object_type const *
    iostream_base<Pointee, DerivedPolicies>::get_pytype(void)
    {
        printf("in ios_base::get_pytype()\n");
        return &m_type;
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
        /*
        return boost::python::incref(
            boost::python::object(
                boost::python::handle<>(
                    boost::python::borrowed(
                        &value ) ) ).ptr() );
        */

        typename DerivedPolicies::value_type * new_obj = PyObject_New(
            typename DerivedPolicies::value_type, &DerivedPolicies::m_type);
        new_obj->buf      = NULL;
        new_obj->m_new    = false;
        new_obj->m_dict   = NULL;
        new_obj->m_stream = const_cast<Pointee*>(&value);
        new_obj->m_view   = NULL;
        new_obj->m_weakrefs  = NULL;
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

    // def_impl
    // Taken from boost::python::class_<...>::def_impl
    template <class Pointee, class DerivedPolicies>
    template <class T, class Fn, class Helper>
    inline void * iostream_base<Pointee, DerivedPolicies>::def_impl(
        T*
      , char const* name
      , Fn fn
      , Helper const& helper
      , ...
    )
    {
        printf("iostream_base::def_impl\n");
        boost::python::objects::add_to_namespace(
            *this
          , name
          , make_function(
                fn
              , helper.policies()
              , helper.keywords()
              , boost::python::detail::get_signature(fn, (T*)0)
            )
          , helper.doc()
        );

        //this->def_default(name, fn, helper, mpl::bool_<Helper::has_default_implementation>());
    }

    // def
    // Taken from boost::python::class_<...>::def_maybe_overloads
    template <class Pointee, class DerivedPolicies>
    template <class Fn, class A1>
    void iostream_base<Pointee, DerivedPolicies>::def(
        char const* name
        , Fn fn
        , A1 const& a1
        , ...)
    {
        printf("iostream_base::def\n");
        this->def_impl(
            boost::python::detail::unwrap_wrapper((Pointee*)0)
          , name
          , fn
          , boost::python::detail::def_helper<A1>(a1)
          , &fn
        );

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
        PyObject     * /* args */,
        PyObject     * /* kwds */)
    {
        typedef typename DerivedPolicies::value_type obj_type;
        printf("In iostream_base::p_new\n");
        value_type * self = PyObject_NEW(typename DerivedPolicies::value_type,
                                         &DerivedPolicies::m_type);

        // Get the instance size.
        // from bp src/object/class.cpp
        /*
        PyObject* instance_size_obj = PyObject_GetAttrString(
            subtype->tp_dict, const_cast<char*>("__instance_size__") );

        ssize_t instance_size = instance_size_obj ? 
    # if PY_VERSION_HEX >= 0x03000000
            PyLong_AsSsize_t(instance_size_obj) : 0;
    # else
            PyInt_AsLong(instance_size_obj) : 0;
    # endif
        if (instance_size < 0)
            instance_size = 0;
        PyErr_Clear(); // Clear any errors that may have occurred.

        obj_type* self = (obj_type*) subtype->tp_alloc(subtype, instance_size);
        */

        if (self != NULL && !PyErr_Occurred())
        {
            self->buf        = NULL;
            self->m_dict     = subtype->tp_dict;
            self->m_new      = true;
            self->m_stream   = new Pointee();
            self->m_view     = NULL;
            self->m_weakrefs = NULL;
        }
        //Py_INCREF(self->m_stream);
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
        printf("iostream_base::p_free\n");
        if (self->m_view != NULL)
        {
            printf("Releasing buffer\n");
            PyBuffer_Release(self->m_view);
        }
        if (self->m_new == true)
        {
            printf("deleting stream\n");
            delete self->m_stream;
        }
        printf("%s free'd\n", Py_TYPE(self)->tp_name);
    }

    // p_call
    template <class Pointee, class DerivedPolicies>
    PyObject * iostream_base<Pointee, DerivedPolicies>::p_call(
        PyObject * other, PyObject * args, PyObject * kwds)
    {
        printf("in iostream_base::operator(self, args, kwds)\n");
        PyObject * obj = (PyObject*)(
            PyObject_New(
                typename DerivedPolicies::value_type,
                &DerivedPolicies::m_type) );
        if (Py_TYPE(obj)->tp_init != NULL)
            Py_TYPE(obj)->tp_init(obj, args, kwds);
        Py_INCREF(obj);
        return obj;
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream_base<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        //Pointee * stream = self->m_stream;
        return PyString_FromFormat(
            "<%s, at %p>",
            m_type.tp_name,
            (void*)self->m_stream);
    }

    // p_type_is_gc
    template <class Pointee, class DerivedPolicies>
    int iostream_base<Pointee, DerivedPolicies>::p_type_is_gc(
        object_type * py_type)
    {
        return py_type->tp_flags & Py_TPFLAGS_HEAPTYPE;
    }

    // Ordinarily, descriptors have a certain assymetry: you can use
    // them to read attributes off the class object they adorn, but
    // writing the same attribute on the class object always replaces
    // the descriptor in the class __dict__.  In order to properly
    // represent C++ static data members, we need to allow them to be
    // written through the class instance.  This function of the
    // metaclass makes it possible.
    template <class Pointee, class DerivedPolicies>
    int iostream_base<Pointee, DerivedPolicies>::class_setattro(
        PyObject *obj, PyObject *name, PyObject* value)
    {
        // Must use "private" Python implementation detail
        // _PyType_Lookup instead of PyObject_GetAttr because the
        // latter will always end up calling the descr_get function on
        // any descriptor it finds; we need the unadulterated
        // descriptor here.
        PyObject * a = _PyType_Lookup(downcast<object_type>(obj), name);

        // a is a borrowed reference or 0

        // If we found a static data descriptor, call it directly to
        // force it to set the static data member
        //if (a != 0 && PyObject_IsInstance(a, objects::static_data()))
        //    return Py_TYPE(a)->tp_descr_set(a, obj, value);
        //else
            return PyType_Type.tp_setattro(obj, name, value);
    }
    //@}

# if PY_VERSION_HEX >= 0x02060000
    //@{
    /// PyBufferProcs members

    template <class Pointee, class DerivedPolicies>
    void iostream_base<Pointee, DerivedPolicies>::p_releasebuf(
        value_type * self, Py_buffer * view)
    {
        printf("In iostream_base::p_releasebuf\n");
        if (self->m_new == true && self->buf  != NULL)
        {
            printf("deleting self->buf\n");
            //delete[] static_cast<char*>(self->buf);
            //self->buf = NULL;
        }
    }
    //@}
# endif

    //@{
    /// PyGetSet member functions
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream_base<Pointee, DerivedPolicies>::ios_get_dict(
        PyObject* op, void*)
    {
        value_type* inst = downcast<value_type>(op);
        if (inst->m_dict == NULL)
            inst->m_dict = PyDict_New();
        return python::xincref(inst->m_dict);
    }

    template <class Pointee, class DerivedPolicies>
    int iostream_base<Pointee, DerivedPolicies>::ios_set_dict(
        PyObject* op, PyObject* dict, void*)
    {
        value_type * inst = downcast<value_type>(op);
        python::xdecref(inst->m_dict);
        inst->m_dict = python::incref(dict);
        return 0;
    }
    ///@}

    //@{
    /// helper functions
    template <class Pointee, class DerivedPolicies>
    int iostream_base<Pointee, DerivedPolicies>::get_buf(
        PyObject* obj, void ** buf, Py_ssize_t * size)
    {
        if (obj == NULL)
        {
            PyErr_SetString(PyExc_BufferError,
                "ostream does not support partial deletion (use iostream)");
            return -1;
        }
        if (PyBuffer_Check(obj))
        {
            PyBufferProcs *pb;
            pb = obj ? obj->ob_type->tp_as_buffer : NULL;
            if ( (*pb->bf_getsegcount)(obj, NULL) != 1 )
            {
                /* ### use a different exception type/message? */
                PyErr_SetString(PyExc_TypeError,
                                "single-segment buffer object expected");
                return -1;
            }
            if ((*size = (*pb->bf_getreadbuffer)(obj, 0, buf)) < 0)
                return -1;
        }
        else if (PyObject_CheckBuffer(obj))
        {
            Py_buffer view;
            printf("ostream::p_ass_subscript - got a new style buffer\n");
            if (PyObject_GetBuffer(obj, &view, PyBUF_SIMPLE) == -1)
                return -1;
            *buf = view.buf;
            *size = view.len;
        }
        else if (PyMemoryView_Check(obj))
        {
            Py_buffer view = * PyMemoryView_GET_BUFFER(obj);
            *buf = view.buf;
            *size = view.len;
        }
        else
        {
            extract<Pointee&> get_stream(obj);
            if (get_stream.check())
            {
                printf("in get_buf() - got a wrapped %s class instance\n",
                        typeid(Pointee).name());
                return 0;
            }
            PyErr_BadArgument();
            return -1;
        }
        return 0;
    }
    //@}

    //@{
    /// PyTypeObject static members
    template <class Pointee, class DerivedPolicies>
    PyGetSetDef iostream_base<Pointee, DerivedPolicies>::ios_getsets[] =
    {
        {const_cast<char*>("__dict__"),  ios_get_dict,  ios_set_dict, NULL, 0},
        {0, 0, 0, 0, 0}
    };

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
        NULL,                               // bf_getbuffer
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
        NULL, // sq_contains
        NULL, // sq_inplace_concat
        NULL  // sq_inplace_repeat
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
        const_cast<char*>("IOstream_base")          // tp_name
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
        , (ternaryfunc)&p_call                      // tp_call
        , 0                                         // tp_str
        , &PyObject_GenericGetAttr                  // tp_getattro
        , &class_setattro                           // tp_setattro
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
        , ios_getsets                              // tp_getset
        , &PyBaseObject_Type                        // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , offsetof(value_type, m_dict)              // tp_dictoffset
        , 0                                         // tp_init
        , 0 //PyType_GenericAlloc                       // tp_alloc
        , (newfunc)&p_new                           // tp_new
        , (freefunc)&p_free                         // tp_free
        , (inquiry)&p_type_is_gc                    // tp_is_gc
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
