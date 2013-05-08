namespace boost { namespace python { 

    // ---        ostream template member definitions        --- //

    // ostream ctors
    template <class Pointee, class DerivedPolicies>
    ostream<Pointee, DerivedPolicies>::ostream(void)
    {
        printf("In ostream initailiser void\n");
        converter::registration& ostream_converter
            = const_cast<converter::registration&>(
                converter::registry::lookup(type_id<Pointee>()) );
        ostream_converter.m_class_object = &m_type;
        objects::register_dynamic_id< base_type >();
        objects::register_conversion<container_type, base_type >(false);
    }

    template <class Pointee, class DerivedPolicies>
    ostream<Pointee, DerivedPolicies>::ostream(const char* s)
    {
        printf("In ostream initailiser 2\n");
    }


    template <class Pointee, class DerivedPolicies>
    ostream<Pointee, DerivedPolicies>::ostream(char const* start, char const* finish)
    {
        printf("In ostream initailiser 3\n");
    }

    template <class Pointee, class DerivedPolicies>
    ostream<Pointee, DerivedPolicies>::ostream(char const* start, std::size_t length)
    {
        printf("In ostream initailiser 4\n");
    }

    template <class Pointee, class DerivedPolicies>
    template <class T>
    ostream<Pointee, DerivedPolicies>::ostream(T const& other)
    {
        printf("In ostream templated initailiser\n");
    }

    // Call operator. Does this ever get used?
    template <class Pointee, class DerivedPolicies>
    PyObject* ostream<Pointee, DerivedPolicies>::operator()(void)
    {
        printf("calling ostream\n");
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
    PyObject* ostream<Pointee, DerivedPolicies>::operator()(Pointee& other)
    {
        printf("calling ostream with a %s\n", typeid(other).name());
        if (m_type.tp_name == 0)
        {
            m_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&m_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    /*
    template <class Pointee, class DerivedPolicies>
    template <class Class>
    void ostream<Pointee, DerivedPolicies>::extension_def(Class &cl)
    {
        printf("In ostream's extension_def(). Class is: %s\n", typeid(Class).name());
        cl
            .def("write", &DerivedPolicies::write_int,
                 return_internal_reference<>() )
            ;
    }
    */

    template <class Pointee, class DerivedPolicies>
    Pointee& ostream<Pointee, DerivedPolicies>::p_write_int(
        Pointee& stream, int value)
    {
        stream << value;
        return stream;
    }

    //@{
    /// PyTypeObject functions

    // Create a new instance, from Python.
    // The default arguments for a Python buffer are:-
    //    buffer([object, [offset, [size]]])
    // Whereas the constructor for std::istream takes a streambuf pointer.
    // Don't see why we can't create the same Python constructor for IOStream 
    // Pointee's.
    // This shouldn't perform any blocking I/O operations, but should merely
    // point the internal C++ streambuf to the Python buffer.
    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_init(value_type* self,
                                                  PyObject*   args,
                                                  PyObject*   kwds)
    {
        printf("In OStream::p_init!\n");
        Pointee * m_ostream = self->m_stream;
        void * buf = NULL;
        /// Copied from buffer_new() in Python 2.7.4's Objects/bufferobject.c
        PyObject * obj = Py_None;
        Py_ssize_t offset = 0;
        Py_ssize_t size = Py_END_OF_BUFFER;
        if (PyErr_WarnPy3k("buffer-like OStreams not supported in Python 3.x", 1) < 0)
            return -1;
        if (!_PyArg_NoKeywords(DerivedPolicies::m_type.tp_name, kwds))
            return -1;
        if (!PyArg_ParseTuple(args, "|O:OStream", &obj))
            return -1;
        m_ostream = new Pointee();
        if (obj != Py_None)
        {
            printf("Creating new %s\n", typeid(Pointee).name());
            //PyObject * py_buf;
            printf("Creating buffer from Object of type: %s\n", obj->ob_type->tp_name);
            obj = PyBuffer_FromReadWriteObject(obj, 0, size);
            if (PyErr_Occurred())
                // As is, this basically only supports bytearray objects.
                // cStringIO doesn't work, memoryviews don't work, sys.stdout
                // doesn't work. OStreams obviously need better support than that!
                return -1;
            printf("calling bf_getwritebuffer\n");
            // Get the output buffer from the passed Python object
            if ((size = obj->ob_type->tp_as_buffer->bf_getwritebuffer(obj, 0, &buf)) < 0)
                return -1;
            //m_ostream->rdbuf()->pubsetbuf((char*)buf, size);
            m_ostream->rdbuf()->str((typename Pointee::char_type*)buf);
        }

        self->m_stream = m_ostream;
        return 0;
    }

    // p_new
    //
    // Initialises ostream class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // 'newfunc'.
    template<class Pointee, class DerivedPolicies>
    PyObject * ostream<Pointee, DerivedPolicies>::p_new(
        PyTypeObject * subtype, PyObject * args, PyObject * kwds)
    {
        value_type * obj = PyObject_NEW(value_type, &m_type);
        if (obj != NULL && !PyErr_Occurred()) // If created object ok
        {
            obj->m_weakrefs = NULL;
        }
        else
            throw_error_already_set();
        printf("ostream::p_new - Creating new %s\n", typeid(Pointee).name());
        obj->m_stream = new Pointee();
        return (PyObject*)obj;
    }

    // p_dealloc
    template <class Pointee, class DerivedPolicies>
    void ostream<Pointee, DerivedPolicies>::p_dealloc(value_type * obj)
    {
      printf("In PyOStream_Dealloc\n");
      // Allocate temporaries if needed, but do not begin destruction just yet
      if (obj->m_weakrefs != NULL)
      {
        PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
      }
      obj->ob_type->tp_free(obj);
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* ostream<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        Pointee * stream = self->m_stream;
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        threadstate UNBLOCK_THREADS;
        return PyString_FromFormat(
            "<Boost.Python.OStream, offset %zd at %p>",
            stream->tellp(),
            &stream);
    }
    //@}

    //@{
    /// PyBufferProcs functions

    // write_buffer
    //
    // Read data from pp into the wrapped class's internal strstream, by using
    // the strstream's << operator.
    // @param self - The PyObject instance holding a pointer to the C++ object
    //               The stream pointer should be in the objects m_stream member.
    // @param idx  - Segment of ostream ostream where to start writing.
    // @param pp   - Pointer to start of Python input ostream. This should be a
    //               null-terminated string.
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t ostream<Pointee, DerivedPolicies>::p_writebuf(value_type* self,
                                             Py_ssize_t idx, void **pp)
    {
        Py_ssize_t size = 0;
        Pointee& m_ostream = *self->m_stream;
        threadstate UNBLOCK_THREADS;
        typename Pointee::pos_type start_pos = m_ostream.tellp();
        m_ostream.seekp(idx);
        std::ios_base::iostate state = m_ostream.rdstate();
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
                m_ostream << *pp ;
                size = m_ostream.tellp() - start_pos;
                break;
        }
        return size;
    }

    // p_segcount
    //
    // PyBufferProcs requirement
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t ostream<Pointee, DerivedPolicies>::p_segcount(
        value_type* self, Py_ssize_t *lenp)
    {
        printf("in get_seg_count\n");
        return 0;
    }
    //@}

    //@{
    /// PyMappingMethod functions
    template <class Pointee, class DerivedPolicies>
    PyObject * ostream<Pointee, DerivedPolicies>::p_subscript(
        value_type* self, PyObject* item)
    {}

    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_ass_subscript(
        value_type* self, PyObject* item, PyObject* value)
    {}
    //@}

    //@{
    /// PySequenceMethods functions
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t ostream<Pointee, DerivedPolicies>::p_length(
        value_type* self)
    {
        return static_cast<Py_ssize_t>(p_size(self->m_stream));
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * ostream<Pointee, DerivedPolicies>::p_concat(
        value_type* self, PyObject* other)
    {
        //self->m_stream
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * ostream<Pointee, DerivedPolicies>::p_item(
        value_type* self, Py_ssize_t idx)
    {}

    template <class Pointee, class DerivedPolicies>
    PyObject * ostream<Pointee, DerivedPolicies>::p_slice(
        value_type* self, Py_ssize_t left, Py_ssize_t right)
    {}

    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_ass_item(
        value_type *self, Py_ssize_t idx, PyObject* other)
    {}

    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_ass_slice(
        value_type* self, Py_ssize_t left, Py_ssize_t right, PyObject* other)
    {}
    //@}

    //@{
    /// Private member functions
    // p_size
    // 
    // Get the current buffer size. This works by first seeking to the end of 
    // the buffer, then calculating the address difference between that and the
    // start. Finally, we return the current pointer position to where it was
    // when we called size.
    template <class Pointee, class DerivedPolicies>
    size_t ostream<Pointee, DerivedPolicies>::p_size(Pointee * ptr)
    {
        pos_type cur_pos;
        size_t size;
        std::ios_base::iostate state = ptr->rdstate();
        threadstate UNBLOCK_THREADS;
        switch (state)
        {
        case std::ios_base::eofbit:
            // If eofbit is set, assume we're at the end of the buffer, so just
            // return the current position (safely).
            return (size_t) p_tell(ptr);
            break;
        case std::ios_base::failbit || std::ios_base::badbit :
            ptr->clear();
            break;
        default:
            break;
        }
        cur_pos   = p_tell(ptr);  //< Get current position
        ptr = p_seek(ptr, 0, ptr->end); //< Seek to the end
        size = ptr->tellp();      //< Calculate the size.
        ptr->seekp(cur_pos);      //< Go back to previous position.
        return size;
    }

    template <class Pointee, class DerivedPolicies>
    size_t ostream<Pointee, DerivedPolicies>::p_tell(Pointee * ptr)
    {
        pos_type cur_pos;
        if ((cur_pos = ptr->tellp()) == -1)
        {
            /// Get state, to print a (relatively) useful exception message.
            //  On failure, state could be any of:-
            ///   eofbit  - no characters available to read
            ///   failbit - sentry construction failed
            ///   badbit  - stream error
            std::ios_base::iostate state = ptr->rdstate();
            const char* err;
            Py_ssize_t size = 0;
            switch (state)
            {
            case std::ios_base::eofbit:
                err = "EOF. This should not be recognised as an exception. "
                      "Please submit a bug report.";
                break;
            case std::ios_base::failbit:
                err = "Stream sentry creation failed.";
                break;
            case std::ios_base::badbit:
                err = "Stream error.";
                break;
            default:
                // Shouldn't get here!
                err = "Unknown error. Please submit a bug report";
                break;
            }
                ::PyErr_Format(
                    PyExc_RuntimeError
                    , "IStream tellg() call failed. %s"
                    , err);
                throw_error_already_set();
        }
        return cur_pos;
    }

    template <class Pointee, class DerivedPolicies>
    Pointee * ostream<Pointee, DerivedPolicies>::p_seek(
        Pointee * ptr, pos_type pos)
    {
        if (!ptr->good())
            ptr->clear();
        ptr->seekp(pos);
        check_seek_state(ptr);
        return ptr;
    }

    template <class Pointee, class DerivedPolicies>
    Pointee * ostream<Pointee, DerivedPolicies>::p_seek(
        Pointee * ptr, off_type offset, seekdir way)
    {
        if (!ptr->good())
            ptr->clear();
        ptr->seekp(offset, way);
        check_seek_state(ptr);
        return ptr;
    }

    template <class Pointee, class DerivedPolicies>
    void ostream<Pointee, DerivedPolicies>::check_seek_state(Pointee * ptr)
    {
        switch (ptr->rdstate())
        {
        case std::ios_base::eofbit:
            ptr->clear();
            break;
        case std::ios_base::failbit :
            ::PyErr_Format(PyExc_RuntimeError
                , "seekp() operation failed." );
            throw_error_already_set();
        case  std::ios_base::badbit :
            ::PyErr_Format(PyExc_RuntimeError
                , "seekp() operation failed. The integrity of the "
                "stream may have been affected" );
            throw_error_already_set();
        default:
            break;
        }
    }
    //@}

    // type_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    /*
    template <class Pointee, class DerivedPolicies>
    PyMemberDef ostream<Pointee, DerivedPolicies>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
            ostream<Pointee, DerivedPolicies>::value_type, m_weakrefs), 0},
        {0}
    } ;
    */

    PyDoc_STRVAR( ostream_doc, "Boost Python wrapped ostream-like object.");

    // Struct to provide Python-side ostreaming support
    template <class Pointee, class DerivedPolicies>
    PyBufferProcs ostream<Pointee, DerivedPolicies>::m_buffer =
    {
        NULL,
        (writebufferproc) ostream<Pointee, DerivedPolicies>::p_writebuf,
        (segcountproc)    ostream<Pointee, DerivedPolicies>::p_segcount,
        NULL
    } ;

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods ostream<Pointee, DerivedPolicies>::m_sequence = {
        (lenfunc)p_length,                 // sq_length
        (binaryfunc)p_concat,              // sq_concat
        NULL,                              // sq_repeat
        (ssizeargfunc)p_item,              // sq_item
        (ssizessizeargfunc)p_slice,        // sq_slice
        (ssizeobjargproc)p_ass_item,       // sq_ass_item
        (ssizessizeobjargproc)p_ass_slice, // sq_ass_slice
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods ostream<Pointee, DerivedPolicies>::m_mapping = {
        (lenfunc)p_length,              // mp_length
        (binaryfunc)p_subscript,        // mp_subscript
        (objobjargproc)p_ass_subscript, // mp_ass_subscript
    };

    // m_type
    //
    // The PyTypeObject struct that manages each ostreamed PyObject instance.
    template <class Pointee, class DerivedPolicies>
    typename ostream<Pointee, DerivedPolicies>::object_type
        ostream<Pointee, DerivedPolicies>::m_type =
    {
        PyObject_HEAD_INIT(NULL)
        0                                           // ob_size
        , const_cast<char*>("Boost.Python.OStream") // tp_name
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
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT
          | Py_TPFLAGS_BASETYPE  
          | Py_TPFLAGS_HAVE_WEAKREFS                //< tp_flags
        , ostream_doc                               // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , offsetof(value_type, m_weakrefs)          // tp_weaklistoffset
        , 0                                         // tp_iter
        , 0                                         // tp_iternext
        , 0                                         // tp_methods
        , 0 //type_members                          // tp_members
        , 0                                         // tp_getset
        , &base_type::m_type                   // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , 0                                         // tp_dictoffset
        , (initproc)&p_init                         // tp_init
        , 0                                         // tp_alloc
        , (newfunc)&p_new                           // tp_new
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

}   }