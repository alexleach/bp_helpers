namespace boost { namespace python {

    // ---        istream template member definitions        --- //

    // istream ctors
    template <class Pointee, class DerivedPolicies>
    istream<Pointee, DerivedPolicies>::istream(void)
        : converter::iostream_base<Pointee, DerivedPolicies>()
    {
        printf("In istream initialiser void\n");
        // Register the Base class
        objects::register_dynamic_id<container_type>();
        objects::register_conversion<container_type, base_type>();

        if (PyType_Ready(&this->m_type) < 0)
            boost::python::throw_error_already_set();
        Py_INCREF(&m_type);
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
        : converter::iostream_base<Pointee, DerivedPolicies>(other)
    {
        printf("In istream templated initailiser\n");
    }

    //@{
    /// Boost Python functions
    template <class Pointee, class DerivedPolicies>
    typename istream<Pointee, DerivedPolicies>::object_type const *
    istream<Pointee, DerivedPolicies>::get_pytype(void)
    {
        printf("in istream::get_pytype()\n");
        return &m_type;
    }

    /// Boost Python functions
    template <class Pointee, class DerivedPolicies>
    typename istream<Pointee, DerivedPolicies>::object_type *
    istream<Pointee, DerivedPolicies>::set_pytype(void)
    {
        printf("in istream::get_pytype()\n");
        return &m_type;
    }
    //@}

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
        printf("In istream::p_init!\n");
        Pointee * m_istream = self->m_stream;
        void * buf;
        /// Copied from buffer_new() in Python 2.7.4's Objects/bufferobject.c
        PyObject * obj;
        // setup default argumetns.
        Py_ssize_t offset = 0;
        Py_ssize_t size = Py_END_OF_BUFFER;
        if (!_PyArg_NoKeywords(DerivedPolicies::m_type.tp_name, kwds))
            return -1;
        if (!PyArg_ParseTuple(args, "O|nn:IStream", &obj, &offset, &size))
            return -1;
        if (PyBuffer_Check(obj))
        {
            obj = PyBuffer_FromObject(obj, offset, size);
            // My own additions, to get that Python buffer into a C++ IOStream
            if ((size = obj->ob_type->tp_as_buffer->bf_getreadbuffer(obj, 0, &buf)) < 0)
                return -1;
        }
        else if (PyObject_CheckBuffer(obj))
        {
            //Py_buffer view;
            self->m_view = new Py_buffer();
            printf("istream::p_init got a new style buffer\n");
            if (PyObject_GetBuffer(obj, self->m_view, PyBUF_SIMPLE) == -1)
                return -1;
            size = self->m_view->len;
            buf = self->m_view->buf;
            //self->m_view = &view;
        }
        else if (PyMemoryView_Check(obj))
        {
            Py_buffer view = * PyMemoryView_GET_BUFFER(obj);
            buf = view.buf;
            size = view.len;
        }
        else
        {
            PyErr_BadArgument();
            return -1;
        }
        printf("size = %i, buf = %p\n", int_type(size), buf);
        m_istream->rdbuf()->pubsetbuf((char_type*)buf, int_type(size));
        self->buf = &buf;
        return 0;
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
        Pointee * m_istream = self->m_stream;
        int offset;
        try {
            offset = p_tell(m_istream);
        } catch (const char*)
        {
            offset = -1;
        }
        //size_t size = p_size(self->m_stream);
        //const char * status = (stream->openmode & std::ios_base::out)
        //                        ? "read-write" : "read-only";
        return PyString_FromFormat(
            "<%s, size %i, offset %i at %p>",
            
            m_type.tp_name,
            p_size(m_istream),
            offset,
            (void*)m_istream);
    }

    // p_str
    //
    // Read available data in self's istream and return it as a Python string.
    template <class Pointee, class DerivedPolicies>
    PyObject* istream<Pointee, DerivedPolicies>::p_str(value_type * self)
    {
        off_type size = 0;
        Pointee * m_istream = self->m_stream;
        std::auto_ptr<char_type> m_ptr;
        {
            threadstate UNBLOCK_THREADS;
            std::streambuf * buf = m_istream->rdbuf();
            if ((size = buf->pubseekoff(0, m_istream->end, std::ios_base::in)) == -1)
            {
                UNBLOCK_THREADS.~threadstate();
                PyErr_SetString(PyExc_BufferError, "Cannot seek input stream.");
                return NULL;
            }
            buf->pubseekpos(0, std::ios_base::in);
            //size = buf->in_avail();
            //vector<char> ptr; // don't really want to include vector just for this
            printf("reading from the buffer %i characters\n", int_type(size));
            m_ptr = std::auto_ptr<char_type>(new char_type[size+1]);
            //size = self->m_stream->readsome(ptr.get(), size); //< readsome is safer, but
            buf->sgetn(m_ptr.get(), size);       // buf->sgetn is probably faster.
            // UNBLOCK_THREADS.~threadstate();
        }
        
        return PyString_FromStringAndSize(m_ptr.get(), size);
    }
    //@}

# if PY_VERSION_HEX < 0x03000000

    //@{
    /// PyBufferProcs members

    // read_buffer
    //
    // Reads formatted data from internal stream into the array pointed to by pp
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t istream<Pointee, DerivedPolicies>::p_readbuf(
        value_type * self, Py_ssize_t idx, void ** pp)
    {
        printf("In istream::p_readbuf\n");
        if (idx != 0)
        {
          PyErr_SetString(PyExc_SystemError,
              "accessing non-existent istream segment.");
          return -1;
        }

        int_type pos = int_type(idx);
        Pointee * m_istream = self->m_stream;
        {
            threadstate UNBLOCK_THREADS;
            m_istream->seekg(pos);
        }

        int_type size = 0;
        if (self->buf == NULL)
        {
            std::streambuf * m_buf = m_istream->rdbuf();
            size = m_buf->in_avail();
            char_type * buf  = static_cast<char_type*>(new char_type[size]);
            m_istream->get(buf, size);
            *pp = buf;
            self->buf = buf;
            printf("pp set to buf (%p)\n", *pp);
        }
        else
        {
            *pp = self->buf;
            pp += pos;
            size = m_istream->rdbuf()->in_avail();
            printf("pp set to self->buf (%p) after moving it back to idx: %i.", *pp, pos);
            printf("size remaining: %i\n", size);
        }
        return size;
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
        if (lenp != NULL)
        {
            *lenp = Py_ssize_t(p_size(self->m_stream));
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
        return Py_ssize_t(p_size(self->m_stream));
    }
    //@}
# endif

# if PY_VERSION_HEX >= 0x02060000
    //@{
    /// PyBufferProcs members
    template <class Pointee, class DerivedPolicies>
    int istream<Pointee, DerivedPolicies>::p_getbuf(
        value_type * self, Py_buffer * view, int flags)
    {
        void * buf = NULL;
        int_type len = 0;
        printf("In istream::p_getbuf\n");
        if (self->buf == NULL)
        {
            printf("buffer is null!\n");
            Pointee * m_istream = self->m_stream;
            threadstate UNBLOCK_THREADS;
            std::streambuf * m_buf = m_istream->rdbuf();
            m_buf->pubseekpos(0);
            len = m_buf->in_avail();
            buf = new char_type[len+1];
            len = m_buf->sgetn(static_cast<char*>(buf), len);
            self->buf = buf;
        }
        else
        {
            buf = self->buf;
            len = p_size(self->m_stream);
        }
        printf("size %i\n", p_size(self->m_stream));
        return PyBuffer_FillInfo(view, reinterpret_cast<PyObject*>(self),
                                 buf, Py_ssize_t(len), 0, flags);
    }

# endif

    //@{
    /// PyMappingMethod functions
    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_subscript(
        value_type * self, PyObject* key)
    {
        printf("In istream::p_subscript\n");
        long idx = PyInt_AsLong(key);
        if (idx == -1 && PyErr_Occurred())
            return NULL;

        Pointee * m_istream = self->m_stream;
        int_type size = p_size(m_istream);
        if (idx < 0)
            idx += size;
        else if (idx > size)
            idx -= size;

        char_type val;
        try {
            threadstate UNBLOCK_THREADS;
            m_istream = p_seek(m_istream, pos_type(idx));
            val = m_istream->get();
        }
        catch (const char* err) {
            PyErr_SetString(PyExc_IOError, err);
            return NULL;
        }
        return PyString_FromFormat("%c", val);
    }
    //@}

    //@{
    /// PySequenceMethods functions
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t istream<Pointee, DerivedPolicies>::p_length(
        value_type* self)
    {
        return Py_ssize_t(p_size(self->m_stream));
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_repeat(
        value_type* self, Py_ssize_t count)
    {
        printf("In iostream_base::p_repeat\n");
        PyObject *obj;
        register char *p;
        Pointee * m_istream = self->m_stream;
        Py_ssize_t size = Py_ssize_t(p_size(m_istream));
        if (count < 0)
            count = 0;

        if (count > PY_SSIZE_T_MAX / size) {
            PyErr_SetString(PyExc_MemoryError, "result too large");
            return NULL;
        }

        if ((obj = PyString_FromStringAndSize(NULL, size * count)) == NULL)
            return NULL;
        p = PyString_AS_STRING(obj);

        threadstate UNBLOCK_THREADS;
        while ( count-- )
        {
            m_istream->get(p, size + 1);
            p += size;
            m_istream->seekg(0);
        }
        return obj;
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_item(
        value_type* self, Py_ssize_t idx)
    {
        printf("In iostream_base::p_item\n");
    }

    template <class Pointee, class DerivedPolicies>
    PyObject * istream<Pointee, DerivedPolicies>::p_slice(
        value_type* self, Py_ssize_t left, Py_ssize_t right)
    {
        printf("In istream::p_slice\n");
        Pointee * m_istream = self->m_stream;
        Py_ssize_t size = Py_ssize_t(p_size(m_istream));
        if (left < 0)
            left = 0;
        if (right < 0)
            right = 0;
        if (right > size)
            right = size;
        if (right < left)
            right = left;
        PyObject * obj;
        register char * p;

        obj = PyString_FromStringAndSize(NULL, right - left);
        p = PyString_AS_STRING(obj);

        //m_istream = p_seek(m_istream, pos_type(left));
        threadstate UNBLOCK_THREADS;
        m_istream->seekg(pos_type(left));
        m_istream->get(p, 1 + right - left);
        return obj;
    }
    //@}

    //@{
    /// Private member functions

    template <class Pointee, class DerivedPolicies>
    void istream<Pointee, DerivedPolicies>::check_seek_state(Pointee * ptr)
    {
        std::ios_base::iostate state = ptr->rdstate();
        if (state & std::ios_base::eofbit)
            ptr->clear();
        else if (state & std::ios_base::failbit)
            throw "seekg() operation failed.";
        else if (state & std::ios_base::badbit)
            throw "seekg() operation failed. The integrity of the "
                "stream may have been affected" ;
    }

    template <class Pointee, class DerivedPolicies>
    Pointee * istream<Pointee, DerivedPolicies>::p_seek(
        Pointee * ptr, pos_type pos)
    {
        printf("In iostream_base::p_seek1\n");
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
        printf("In iostream_base::p_seek2\n");
        if (!ptr->good())
            ptr->clear();
        ptr->seekg(offset, way);
        check_seek_state(ptr);
        return ptr;
    }

    // p_size
    // 
    // Get the current buffer size. This works by first seeking to the end of 
    // the buffer, then calculating the address difference between that and the
    // start. Finally, we return the current pointer position to where it was
    // when we called size.
    template <class Pointee, class DerivedPolicies>
    typename istream<Pointee, DerivedPolicies>::int_type
    istream<Pointee, DerivedPolicies>::p_size(Pointee * m_istream)
    {
        pos_type cur_pos;
        int_type size = -1;
        std::ios_base::iostate state = m_istream->rdstate();
        threadstate UNBLOCK_THREADS;
        try
        {
            if (state & std::ios_base::eofbit)
            {
                return p_tell(m_istream);    
            }
            else if (state & (std::ios_base::failbit | std::ios_base::badbit))
                m_istream->clear();
            cur_pos = p_tell(m_istream);  //< Get current position
        }
        catch (const char* msg)
        {
            UNBLOCK_THREADS.~threadstate();
            PyErr_SetString(PyExc_IOError, msg);
            return -1;
        }
        m_istream->seekg(0, m_istream->end); //< seek to the end.
        size = m_istream->tellg();      //< Calculate the size.
        m_istream->seekg(cur_pos);      //< Go back to previous position.
        if (size == -1)
        {
            UNBLOCK_THREADS.~threadstate();
            PyErr_SetString(PyExc_TypeError, "stream does not support indexing");
            return -1;
        }
        return size;
    }

    template <class Pointee, class DerivedPolicies>
    typename istream<Pointee, DerivedPolicies>::pos_type
    istream<Pointee, DerivedPolicies>::p_tell(Pointee * ptr)
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
            if (state & std::ios_base::failbit)
                err = "Logical error on i/o operation.";
            else if (state & std::ios_base::badbit)
                err = "Read / write error on i/o operation.";
            else
                // Shouldn't get here!
                err = "Unknown error. Please submit a bug report";
            throw err;
        }
        return cur_pos;
    }
    //@}

    PyDoc_STRVAR( istream_doc, "Boost Python wrapped istream-like object.");

    // Struct to provide Python-side buffering support
    template <class Pointee, class DerivedPolicies>
    PyBufferProcs istream<Pointee, DerivedPolicies>::m_buffer =
    {
# if PY_VERSION_HEX < 0x03000000
        (readbufferproc)  &p_readbuf,       // bf_getreadbuffer
        NULL,                               // bf_getwritebuffer
        (segcountproc)    &p_segcount,      // bf_getsegcount
        (charbufferproc)  &p_charbuf,       // bf_getcharbuffer
# endif
# if PY_VERSION_HEX >= 0x02060000
        (getbufferproc)   &p_getbuf,        // bf_getbuffer
        (releasebufferproc)&base_type::p_releasebuf // bf_releasebuffer
# endif
    } ;

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods istream<Pointee, DerivedPolicies>::m_sequence = {
        (lenfunc)&p_length,                // sq_length
        NULL,                              // sq_concat
        (ssizeargfunc)&p_repeat,           // sq_repeat
        (ssizeargfunc)&p_item,             // sq_item
        (ssizessizeargfunc)&p_slice,       // sq_slice
        NULL,                              // sq_ass_item
        NULL,                              // sq_ass_slice
        NULL,                              // sq_contains
        NULL,                              // sq_inplace_concat
        NULL                               // sq_inplace_repeat
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods istream<Pointee, DerivedPolicies>::m_mapping = {
        (lenfunc)&p_length,             // mp_length
        (binaryfunc)&p_subscript,       // mp_subscript
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
        PyVarObject_HEAD_INIT(NULL, 0)
        const_cast<char*>("IStream")                // tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , 0 //(destructor)&p_dealloc                    // tp_dealloc
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
        , 0 //PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT                        //< tp_flags
          | Py_TPFLAGS_BASETYPE  
# if PY_VERSION_HEX < 0x03000000
          | Py_TPFLAGS_HAVE_GETCHARBUFFER 
          //| Py_TPFLAGS_HAVE_WEAKREFS
#   if PY_VERSION_HEX >= 0x02060000
          | Py_TPFLAGS_HAVE_NEWBUFFER
#   endif
# endif
        , istream_doc                               // tp_doc
        , 0                                         // tp_traverse
        , 0                                         // tp_clear
        , 0                                         // tp_richcompare
        , 0 //offsetof(value_type, m_weakrefs)          // tp_weaklistoffset
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
        , 0                                         // tp_new
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
