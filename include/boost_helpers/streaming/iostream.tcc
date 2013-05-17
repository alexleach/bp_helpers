namespace boost { namespace python { 

    // ---        Buffer template member definitions        --- //

    // buffer ctors
    template <class Pointee, class DerivedPolicies>
    iostream<Pointee, DerivedPolicies>::iostream(void)
        : istream<Pointee, DerivedPolicies>(),
          ostream<Pointee, DerivedPolicies>()
    {
        printf("In iostream initailiser void\n");
        PyObject * bases;
        PyTypeObject * istream_type = &this->istream<Pointee, DerivedPolicies>::m_type;
        PyTypeObject * ostream_type = &this->ostream<Pointee, DerivedPolicies>::m_type;
        if ((bases = PyTuple_Pack(2,
            istream_type,
            ostream_type )) == NULL)
            //&iostream_holder.istream<Pointee, DerivedPolicies>::m_type,
            //&iostream_holder.ostream<Pointee, DerivedPolicies>::m_type )) == NULL)
        {
            PyErr_SetString(PyExc_TypeError,
                "Failed to add dependent base classes.");
            return;
        }
        m_type.tp_bases = bases;

        // Register the Base classes
        objects::register_dynamic_id<container_type>();
        objects::register_conversion<container_type, istream_base>();
        objects::register_conversion<container_type, ostream_base>();

        if (PyType_Ready(&this->m_type) < 0)
            boost::python::throw_error_already_set();
        Py_INCREF(&m_type);
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

    //@{
    /// Boost Python functions
    template <class Pointee, class DerivedPolicies>
    typename iostream<Pointee, DerivedPolicies>::object_type const *
    iostream<Pointee, DerivedPolicies>::get_pytype(void)
    {
        printf("in iostream::get_pytype()\n");
        return &m_type;
    }

    template <class Pointee, class DerivedPolicies>
    typename iostream<Pointee, DerivedPolicies>::object_type *
    iostream<Pointee, DerivedPolicies>::set_pytype(void)
    {
        printf("in iostream::set_pytype()\n");
        return &m_type;
    }
    //@}

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
        void * buf = self->buf = NULL;
        PyObject * obj    = Py_None;
        Py_ssize_t offset = 0;

        Py_ssize_t size   = Py_END_OF_BUFFER;
        if (!_PyArg_NoKeywords(DerivedPolicies::m_type.tp_name, kwds))
            return -1;
        if (!PyArg_ParseTuple(args, "|Onn:iostream", &obj, &offset, &size))
            return -1;

        // Get that Python buffer into a C++ IOStream
        //BufferLocker bufflock; //< RAII for new-style buffers.
        if (obj == Py_None)
            return 0;
        if (PyBuffer_Check(obj))
        {
            obj = PyBuffer_FromReadWriteObject(obj, offset, size);
            if ((size = obj->ob_type->tp_as_buffer->bf_getwritebuffer(obj, 0, &buf)) < 0)
                return -1;
        }
        else if (PyObject_CheckBuffer(obj))
        {
            //Py_buffer view;
            self->m_view = new Py_buffer();
            printf("iostream::p_init got a new style buffer\n");
            if (PyObject_GetBuffer(obj, self->m_view, PyBUF_WRITABLE) == -1)
                return -1;
            //bufflock.Lock(&view);
            buf = self->m_view->buf;
            size = self->m_view->len;
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
        m_stream->rdbuf()->pubsetbuf((char_type*)buf, size);
        self->buf = buf;
        return 0;
    }

    // p_repr
    //
    // Return a string containing some information about the buffer instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        int off;
        Pointee * m_iostream = self->m_stream;
        assert(m_iostream != NULL);
        if ((off = m_iostream->tellg()) < 0)
        {
            m_iostream->clear();
            off = -1;
        }

        int_type size;
        if (m_iostream != NULL)
        {
            if ((size=istream_base::p_size(m_iostream)) == -1)
                if ((size=ostream_base::p_size(m_iostream)) == -1)
                    size = -1;
            PyErr_Clear();
        }
        else
            size = -1;

        return PyString_FromFormat(
            "<%s, size %i, offset %i at %p>",
            m_type.tp_name,
            size,
            off,
            (void*)m_iostream);
    }

    // p_str
    template <class Pointee, class DerivedPolicies>
    PyObject* iostream<Pointee, DerivedPolicies>::p_str(value_type * self)
    {
        PyObject * str;
        if ((str = istream_base::p_str(self)) == NULL)
        {
            if ((str = ostream_base::p_str(self)) == NULL)
            {
                PyErr_SetString(PyExc_BufferError, "Cannot seek io stream.");
                return NULL;
            }
            PyErr_Clear();
        }
        return str;
    }

    template <class Pointee, class DerivedPolicies>
    int iostream<Pointee, DerivedPolicies>::p_ass_item(
        value_type *self, Py_ssize_t idx, PyObject* other)
    {
        printf(" in iostream::p_ass_item\n");
        if (other == NULL)
        {
            Pointee * m_iostream = self->m_stream;
            std::streambuf * m_buf = m_iostream->rdbuf();
            int_type size;
            char_type * buf;
            threadstate UNLOCK_THREADS;
            // Go to end to get the current size.
            if ((size = m_buf->pubseekoff(0, m_iostream->end, 
                                          m_iostream->in)) == -1)
            {
                UNLOCK_THREADS.~threadstate();
                PyErr_SetString(PyExc_BufferError, "Cannot seek buffer");
                return -1;
            }
            try {
                buf = new char_type[--size];
            }
            catch (std::bad_alloc& ba)
            {
                UNLOCK_THREADS.~threadstate();
                PyErr_SetString(PyExc_MemoryError, ba.what() );
                return -1;
            }
            m_buf->pubseekpos(0);
            m_buf->sgetn(&buf[0], pos_type(idx));
            m_buf->pubseekoff(1, m_iostream->cur, m_iostream->in);
            m_buf->sgetn(&buf[idx], size - idx);
            buf[size] = '\0';
            m_buf->pubsetbuf(buf, size);
            self->buf = buf;
            return 0;
        }
        else
            return  ostream_base::p_ass_item(self, idx, other);
    }

    template <class Pointee, class DerivedPolicies>
    int iostream<Pointee, DerivedPolicies>::p_ass_slice(
        value_type* self, Py_ssize_t left, Py_ssize_t right, PyObject* other)
    {
        printf(" in iostream::p_ass_slice\n");
        if (other == NULL)
        {
            // delete slice.
            Pointee * m_iostream = self->m_stream;
            std::streamsize size, newsize;
            std::streamsize diff = right - left;
            std::streambuf * m_buf = m_iostream->rdbuf();
            char_type * buf;
            threadstate UNLOCK_THREADS;
            m_buf->pubseekpos(0);
            size = m_buf->in_avail();
            newsize = size - diff;
            try {
                buf = new char_type[newsize];
            }
            catch (std::bad_alloc& ba)
            {
                UNLOCK_THREADS.~threadstate();
                PyErr_SetString(PyExc_MemoryError, ba.what() );
                return -1;
            }
            m_buf->sgetn(buf, std::streamsize(left));
            m_buf->pubseekpos(std::streampos(right), m_iostream->in);
            m_buf->sgetn(&buf[left], std::streamsize(size - right));
            self->buf = static_cast<void*>(buf);
            m_buf->pubsetbuf(buf, newsize);
            return 0;
        }
        else
            return  ostream_base::p_ass_slice(self, left, right, other);
    }

    template <class Pointee, class DerivedPolicies>
    int iostream<Pointee, DerivedPolicies>::p_ass_subscript(
        value_type* self, PyObject* item, PyObject* value)
    {
        printf(" in iostream::p_ass_subscript\n");
        if (value == NULL)
        {
            // delete from self->m_stream. Means copying everything in to a new
            // buffer.
            Pointee * m_iostream = self->m_stream;
            std::streambuf * m_buf = m_iostream->rdbuf();
            Py_ssize_t selfsize = Py_ssize_t(istream_base::p_size(m_iostream));

            // Almost exclusively taken from bufferobject.c
            if (PyIndex_Check(item))
            {
                Py_ssize_t idx = PyNumber_AsSsize_t(item, PyExc_IndexError);
                if (idx == -1 && PyErr_Occurred())
                    return -1;
                if (idx < 0)
                    idx += selfsize;
                return p_ass_item(self, idx, value);
            }
            else if (PySlice_Check(item))
            {
                Py_ssize_t start, stop, step, slicelength;

                if (PySlice_GetIndicesEx((PySliceObject *)item, selfsize,
                                &start, &stop, &step, &slicelength) < 0)
                    return -1;
                if (slicelength <= 0)
                    return 0;

                int_type newsize = int_type(selfsize - slicelength);
                char_type * buf = new char_type[newsize];
                int_type idx1, idx2;
                threadstate UNBLOCK_THREADS;
                m_buf->pubseekpos(0,  m_iostream->in);
                idx1 = idx2 = m_buf->sgetn(buf, std::streampos(start) );
                int_type stepdiff = step -1;
                while (slicelength--)
                {
                    m_buf->sbumpc();
                    idx1 += m_buf->sgetn(&buf[idx2], stepdiff) + 1;
                    idx2 += stepdiff;
                }
                m_buf->sgetn(&buf[idx2], selfsize - idx1);
                buf[newsize] = '\0';
                self->buf = buf;
                m_buf->pubsetbuf(buf, newsize);
                m_buf->pubsync();
            }
            else
            {
                PyErr_SetString(PyExc_TypeError,
                                "buffer indices must be integers");
                return -1;
            }
            return 0;
        }
        else
            return ostream_base::p_ass_subscript(self, item, value);
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
        NULL, //(readbufferproc)   istream_base::p_readbuf,  // bf_getreadbuffer
        NULL, //(writebufferproc)  ostream_base::p_writebuf, // bf_getwritebuffer
        NULL, //(segcountproc)     istream_base::p_segcount, // bf_getsegcount
        NULL, //(charbufferproc)   istream_base::p_charbuf,  // bf_getcharbuffer
# endif
# if PY_VERSION_HEX >= 0x02060000
        NULL, //(getbufferproc)    istream_base::p_getbuf,   // bf_getbuffer
        NULL, //(releasebufferproc)base_type::p_releasebuf   // bf_releasebuffer
# endif
    };

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods iostream<Pointee, DerivedPolicies>::m_sequence = {
        NULL, //(lenfunc)istream_base::p_length,                 // sq_length
        NULL, //(binaryfunc)ostream_base::p_concat,              // sq_concat
        NULL, //(ssizeargfunc)istream_base::p_repeat,            // sq_repeat
        NULL, //(ssizeargfunc)istream_base::p_item,              // sq_item
        NULL, //(ssizessizeargfunc)istream_base::p_slice,        // sq_slice
        (ssizeobjargproc)p_ass_item,       // sq_ass_item
        (ssizessizeobjargproc)p_ass_slice, // sq_ass_slice
        NULL,                              // sq_contains
        NULL,                              // sq_inplace_concat
        NULL                               // sq_inplace_repeat
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods iostream<Pointee, DerivedPolicies>::m_mapping = {
        NULL, //(lenfunc)istream_base::p_length,              // mp_length
        NULL, //(binaryfunc)istream_base::p_subscript,        // mp_subscript
        (objobjargproc)p_ass_subscript, // mp_ass_subscript
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
        const_cast<char*>("IOStream")               // tp_name
        , sizeof(value_type)                        // tp_basicsize
        , 0                                         // tp_itemsize
        , 0//(destructor)&base_type::p_dealloc      // tp_dealloc
        , 0                                         // tp_print
        , 0                                         // tp_getattr
        , 0                                         // tp_setattr
        , 0                                         // tp_compare
        , (reprfunc)&p_repr                         // tp_repr
        , 0                                         // tp_as_number
        , &m_sequence                               // tp_as_sequence
        , &m_mapping                                // tp_as_mapping
        , 0                                         // tp_hash
        , 0 //(ternaryfunc)&base_type::p_call           // tp_call
        , (reprfunc)&p_str                          // tp_str
        , 0 // PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT                        // tp_flags
          | Py_TPFLAGS_BASETYPE
# if PY_VERSION_HEX < 0x03000000
          | Py_TPFLAGS_HAVE_GETCHARBUFFER
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
        , 0 //&base_type::m_type                        // tp_base
        , 0                                         // tp_dict
        , 0                                         // tp_descr_get
        , 0                                         // tp_descr_set
        , 0                                         // tp_dictoffset
        , (initproc)&p_init                         // tp_init
        , 0                                         // tp_alloc
        , (newfunc)&base_type::p_new                // tp_new
        , 0 //(freefunc)&base_type::p_free          // tp_free
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
