namespace boost { namespace python {

    // ---        istream template member definitions        --- //

    // istream ctors
    template <class Pointee, class DerivedPolicies>
    istream<Pointee, DerivedPolicies>::istream(void)
    {
        printf("In istream initailiser void\n");

        converter::registration& istream_converter
            = const_cast<converter::registration&>(
                converter::registry::lookup(type_id<Pointee>()) );
        istream_converter.m_class_object = &m_type;

        // Register the Base class
        objects::register_dynamic_id< base_type >();
        objects::register_conversion<container_type, base_type >(false);
    }

    template <class Pointee, class DerivedPolicies>
    istream<Pointee, DerivedPolicies>::istream(const char* s)
    {
        printf("In istream initailiser 2\n");
    }


    template <class Pointee, class DerivedPolicies>
    istream<Pointee, DerivedPolicies>::istream(char const* start, char const* finish)
    {
        printf("In istream initailiser 3\n");
    }

    template <class Pointee, class DerivedPolicies>
    istream<Pointee, DerivedPolicies>::istream(char const* start, std::size_t length)
    {
        printf("In istream initailiser 4\n");
    }

    template <class Pointee, class DerivedPolicies>
    template <class T>
    istream<Pointee, DerivedPolicies>::istream(T const& other)
    {
        printf("In istream templated initailiser\n");
    }

    // Call operator. Does this ever get used?
    template <class Pointee, class DerivedPolicies>
    PyObject* istream<Pointee, DerivedPolicies>::operator()(void)
    {
        printf("calling istream()\n");
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
    PyObject* istream<Pointee, DerivedPolicies>::operator()(Pointee& other)
    {
        printf("calling istream with a %s\n", typeid(other).name());
        if (m_type.tp_name == 0)
        {
            m_type.tp_name = const_cast<char*>(type_id<Pointee*>().name());
            if (PyType_Ready(&m_type) < 0)
            {
                throw error_already_set();
            }
        }
    }

    //@{
    /// PyTypeObject member functions
    //
    // Create a new instance, from Python.
    // The default arguments for a Python buffer are:-
    //    buffer([object, [offset, [size]]])
    // Whereas the constructor for std::istream takes a streambuf pointer.
    // Don't see why we can't create the same Python constructor for IOStream 
    // Pointee's.
    // This shouldn't perform any blocking I/O operations, but should merely
    // point the internal C++ streambuf to the Python buffer.
    template <class Pointee, class DerivedPolicies>
    int istream<Pointee, DerivedPolicies>::p_init(value_type* self,
                                                  PyObject*   args,
                                                  PyObject*   kwds)
    {
        printf("In IStream::p_init!\n");
        Pointee * m_istream = self->m_stream;
        void * buf;
        /// Copied from buffer_new() in Python 2.7.4's Objects/bufferobject.c
        PyObject * obj;
        Py_ssize_t offset = 0;
        Py_ssize_t size = Py_END_OF_BUFFER;
        if (PyErr_WarnPy3k("buffer-like IStreams not supported in Python 3.x", 1) < 0)
            return -1;
        if (!_PyArg_NoKeywords(DerivedPolicies::m_type.tp_name, kwds))
            return -1;
        if (!PyArg_ParseTuple(args, "O|nn:IStream", &obj, &offset, &size))
            return -1;
        obj = PyBuffer_FromObject(obj, offset, size);

        // My own additions, to get that Python buffer into a C++ IOStream
        if ((size = obj->ob_type->tp_as_buffer->bf_getreadbuffer(obj, 0, &buf)) < 0)
            return -1;
        m_istream->rdbuf()->str((typename Pointee::char_type*)buf);

        self->m_stream = m_istream;
        return 0;
    }

    // p_new
    //
    // Initialises istream class for derived types. Compiler refuses to allow
    // this to be a template-ised member function, due to the signature of 
    // `newfunc`.
    template<class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_new(
        PyTypeObject * subtype, PyObject * args, PyObject * kwds)
    {
        printf("In PyIStream_New\n");
        //value_type * obj = PyType_GenericNew(subtype, args, kwds);
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
    void istream<Pointee, DerivedPolicies>::p_dealloc(value_type * obj)
    {
      printf("In PyIStream::p_dealloc\n");
      // Allocate temporaries if needed, but do not begin destruction just yet
      if (obj->m_weakrefs != NULL)
      {
        PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
      }
      obj->ob_type->tp_free(obj);
    }

    // p_hash
    // 
    // Return hashed representation of current istreamfor >30% faster inequality
    // testing.
    template <class Pointee, class DerivedPolicies>
    long int istream<Pointee, DerivedPolicies>::p_hash(value_type * self)
    {
        printf("in p_hash\n");
        return 0;
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* istream<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        Pointee * stream = self->m_stream;
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        return PyString_FromFormat(
            "<Boost.Python.IStream, size %zd, offset %zd at %p>",
            p_size(self->m_stream),
            stream->tellg(),
            self);
    }

    // p_str
    //
    // Read available data in self's istream and return it as a Python string.
    template <class Pointee, class DerivedPolicies>
    PyObject* istream<Pointee, DerivedPolicies>::p_str(value_type * self)
    {
        Py_ssize_t size = 0;
        Pointee * stream = self->m_stream;
        if (stream != NULL)
        {
            std::streambuf * buf = stream->rdbuf();
            // Should make sure that the stream is not binary
            if (buf != NULL)
            {
                size = buf->in_avail();
                //vector<char> ptr; // don't really want to include vector just
                                    // for this.
                char ptr[size+1];   //< Is this safe across all platforms?
                                    //  Seems to work well, but -pedantic spits
                                    //  out a warning message.
                threadstate UNBLOCK_THREADS;
                size = stream->readsome(ptr, size); //< readsome is safer, but
                //buf->sgetn(ptr, size);            // buf->getn is probably faster.
                
                return PyString_FromStringAndSize(ptr, size);
            }
        }
        return PyString_FromStringAndSize('\0', 0);
    }
    //@}

    //@{
    /// PyBufferProcs members

    // read_buffer
    //
    // Reads data from internal stream into the array pointed to by pp
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t istream<Pointee, DerivedPolicies>::p_readbuf(value_type* self,
                                                            Py_ssize_t idx,
                                                            void **pp)
    {
        if (idx != 0)
        {
          PyErr_SetString(PyExc_SystemError,
              "accessing non-existent istream segment.");
          throw_error_already_set();
        }
        Pointee& m_istream = *self->m_stream;
        {
            threadstate UNBLOCK_THREADS;
            m_istream >> *pp ;
        }
        return (Py_ssize_t) p_tell(&m_istream);
    }

    // p_segcount
    //
    // PyBufferProcs requirement
    // From Python docs:-
    // Return the number of memory segments which comprise the buffer. If lenp
    // is not NULL, the implementation must report the sum of the sizes (in 
    // bytes) of all segments in *lenp. The function cannot fail.
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t istream<Pointee, DerivedPolicies>::p_segcount(
        value_type* self, Py_ssize_t * lenp)
    {
        Py_ssize_t size = static_cast<Py_ssize_t>(p_size(self->m_stream));
        if (lenp != NULL)
        {
            *lenp = size;
        }
        return 1;
    }

    // p_charbuf
    //
    // Requirement of PyBufferProcs struct, when the PyTypeObject has
    // Py_TPFLAGS_HAVE_GETCHARBUFFER bit set in its tp_flags.
    // From Python docs:-
    // Return the size of the segment idx that pp is set to. *pp is set to the 
    // memory buffer. Returns -1 on error.
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t istream<Pointee, DerivedPolicies>::p_charbuf(
        value_type* self, Py_ssize_t idx, void **pp)
    {
        // Taken from Python 2.7.4 bufferobject.c
        if (idx != 0)
        {
            PyErr_SetString(PyExc_SystemError,
                "Accessing non-existent buffer segment.");
            return -1;
        }
        *pp = self->m_stream->pword(idx);
        return p_size(self->m_stream);
    }
    //@}

    //@{
    /// PyMappingMethod functions
    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_subscript(
        value_type* self, PyObject* item)
    {}
    //@}

    //@{
    /// PySequenceMethods functions
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t istream<Pointee, DerivedPolicies>::p_length(
        value_type* self)
    {
        return static_cast<Py_ssize_t>(p_size(self->m_stream));
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_concat(
        value_type* self, PyObject* other)
    {
        //self->m_stream
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_repeat(
        value_type* self, Py_ssize_t count)
    {}

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_item(
        value_type* self, Py_ssize_t idx)
    {}

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_slice(
        value_type* self, Py_ssize_t left, Py_ssize_t right)
    {}
    //@}

    // p_size
    // 
    // Get the current buffer size. This works by first seeking to the end of 
    // the buffer, then calculating the address difference between that and the
    // start. Finally, we return the current pointer position to where it was
    // when we called size.
    template <class Pointee, class DerivedPolicies>
    size_t istream<Pointee, DerivedPolicies>::p_size(Pointee * m_istream)
    {
        pos_type cur_pos;
        size_t size;
        //Pointee * m_istream = self->m_stream;
        std::ios_base::iostate state = m_istream->rdstate();
        threadstate UNBLOCK_THREADS;
        switch (state)
        {
        case std::ios_base::eofbit:
            return (size_t) p_tell(m_istream);
            break;
        case std::ios_base::failbit || std::ios_base::badbit :
            m_istream->clear();
            break;
        default:
            break;
        }
        cur_pos   = p_tell(m_istream);  //< Get current position
        m_istream = p_seek(m_istream, 0, m_istream->end); //< Seek to the end
        size = m_istream->tellg();      //< Calculate the size.
        m_istream->seekg(cur_pos);      //< Go back to previous position.
        return size;
    }

    template <class Pointee, class DerivedPolicies>
    Pointee * istream<Pointee, DerivedPolicies>::p_seek(
        Pointee * ptr, pos_type pos)
    {
        if (!ptr->good())
            ptr->clear();
        ptr->seekg(pos);
        check_seek_state(ptr);
        return ptr;
    }

    template <class Pointee, class DerivedPolicies>
    Pointee * istream<Pointee, DerivedPolicies>::p_seek(
        Pointee * ptr, off_type offset, seekdir way)
    {
        if (!ptr->good())
            ptr->clear();
        ptr->seekg(offset, way);
        check_seek_state(ptr);
        return ptr;
    }

    template <class Pointee, class DerivedPolicies>
    void istream<Pointee, DerivedPolicies>::check_seek_state(Pointee * ptr)
    {
        switch (ptr->rdstate())
        {
        case std::ios_base::eofbit:
            ptr->clear();
            break;
        case std::ios_base::failbit :
            ::PyErr_Format(PyExc_RuntimeError
                , "seekg() operation failed." );
            throw_error_already_set();
        case  std::ios_base::badbit :
            ::PyErr_Format(PyExc_RuntimeError
                , "seekg() operation failed. The integrity of the "
                "stream may have been affected" );
            throw_error_already_set();
        default:
            break;
        }
    }

    template <class Pointee, class DerivedPolicies>
    size_t istream<Pointee, DerivedPolicies>::p_tell(Pointee * ptr)
    {
        pos_type cur_pos;
        if ((cur_pos = ptr->tellg()) == -1)
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

    // type_members
    //
    // Initialise a dynamic array of the type's extraneous members.
    /*
    template <class Pointee>
    PyMemberDef istream<Pointee>::type_members[] =
    {
        {const_cast<char*>("__weakref__"), T_OBJECT, offsetof(
            istream<Pointee>::value_type, m_weakrefs), 0},
        {0}
    } ;
    */

    PyDoc_STRVAR( istream_doc, "Boost Python wrapped istream-like object.");

    // Struct to provide Python-side buffering support
    template <class Pointee, class DerivedPolicies>
    PyBufferProcs istream<Pointee, DerivedPolicies>::m_buffer =
    {
        (readbufferproc)  &p_readbuf,
        NULL,
        (segcountproc)    &p_segcount,
        (charbufferproc)  &p_charbuf,
    } ;

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods istream<Pointee, DerivedPolicies>::m_sequence = {
        (lenfunc)p_length,                 // sq_length
        (binaryfunc)p_concat,              // sq_concat
        (ssizeargfunc)p_repeat,            // sq_repeat
        (ssizeargfunc)p_item,              // sq_item
        (ssizessizeargfunc)p_slice,        // sq_slice
        NULL,                              // sq_ass_item
        NULL                               // sq_ass_slice
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods istream<Pointee, DerivedPolicies>::m_mapping = {
        (lenfunc)p_length,              // mp_length
        (binaryfunc)p_subscript,        // mp_subscript
        NULL                            // mp_ass_subscript
    };

    // m_type
    //
    // The PyTypeObject struct that manages each buffered PyObject instance.
    template <class Pointee, class DerivedPolicies>
    //PyTypeObject istream<Pointee, DerivedPolicies>::m_type =
    typename istream<Pointee, DerivedPolicies>::object_type
        istream<Pointee, DerivedPolicies>::m_type =
    {
        PyObject_HEAD_INIT(NULL)
        0                                           // ob_size
        , const_cast<char*>("Boost.Python.IStream") // tp_name
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
        , (reprfunc)&p_str                          // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT
          | Py_TPFLAGS_BASETYPE  
          | Py_TPFLAGS_HAVE_GETCHARBUFFER 
          | Py_TPFLAGS_HAVE_WEAKREFS                //< tp_flags
        , istream_doc                               // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , offsetof(value_type, m_weakrefs)          // tp_weaklistoffset
        , 0                                         // tp_iter
        , 0                                         // tp_iternext
        , 0                                         // tp_methods
        , 0 //type_members                              // tp_members
        , 0                                         // tp_getset
        , &base_type::m_type                        // tp_base
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

}  } // End Boost Python Converter namespaces
