namespace boost { namespace python { 

    // ---        ostream template member definitions        --- //

    // ostream ctors
    template <class Pointee, class DerivedPolicies>
    ostream<Pointee, DerivedPolicies>::ostream(void)
    {
        printf("In ostream initialiser void\n");
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
        void * buf = NULL;
        /// Copied from buffer_new() in Python 2.7.4's Objects/bufferobject.c
        PyObject * obj = Py_None;


        Py_ssize_t size = Py_END_OF_BUFFER;
        if (PyErr_WarnPy3k("buffer-like OStreams not supported in Python 3.x", 1) < 0)
            return -1;
        if (!_PyArg_NoKeywords(DerivedPolicies::m_type.tp_name, kwds))
            return -1;
        if (!PyArg_ParseTuple(args, "|O:OStream", &obj))
            return -1;
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
        }
        self->m_stream->rdbuf()->str((typename Pointee::char_type*)buf);
        return 0;
    }

    // p_repr
    //
    // Return a string containing some information about the istream instance.
    template <class Pointee, class DerivedPolicies>
    PyObject* ostream<Pointee, DerivedPolicies>::p_repr(value_type * self)
    {
        Pointee * m_ostream = self->m_stream;
        size_t offset;
        try {
            offset = p_tell(m_ostream);
        } catch (const char*)
        {
            offset = -1;
        }
        return PyString_FromFormat(
            "<Boost.Python.OStream, offset %lu at %p>",
            offset,
            (void*)m_ostream);
    }
    //@}

# if PY_VERSION_HEX < 0x03000000

    //@{
    /// PyBufferProcs functions

    // write_buffer
    //
    // Read data from pp into the wrapped class's internal strstream, by using
    // the strstream's << operator.
    // @param self - The PyObject instance holding a pointer to the C++ object
    //               The stream pointer should be in the objects m_stream member.
    // @param idx  - Segment of ostream ostream where to start writing.
    // @param pp   - Pointer to start of Python output ostream. This should be a
    //               null-terminated string.
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t ostream<Pointee, DerivedPolicies>::p_writebuf(
        value_type* self, Py_ssize_t idx, void **pp)
    {
        printf("In ostream::p_writebuf\n");
        //Py_ssize_t size = 0;
        //Pointee& m_ostream = *self->m_stream;
        std::streambuf * m_buf = self->m_stream->rdbuf();
        char buf[BUFSIZ];
        threadstate UNBLOCK_THREADS;
        m_buf->pubseekpos(idx);
        m_buf = m_buf->pubsetbuf(buf, BUFSIZ);
        *pp = m_buf;
        //size = m_buf->in_avail();
        //typename Pointee::pos_type start_pos = m_ostream.tellp();
        //m_ostream.seekp(idx);
        if (!self->m_stream->good())
        {
            std::ios_base::iostate state = self->m_stream->rdstate();
            const char * err;
            if (state & std::ios_base::failbit)
                err = "Logical error on i/o operation.";
            else if (state & std::ios_base::badbit)
                err = "Read / write error on i/o operation.";
            else
                err = "Unknown error. Please file a bug report.";
            UNBLOCK_THREADS.~threadstate();
            PyErr_SetString(PyExc_IOError, err);
            return -1;
        }
        else
        {

            //m_ostream << *pp ;
            //size = m_ostream.tellp() - start_pos;
        }
        return m_buf->in_avail();
    }

    // p_segcount
    //
    // PyBufferProcs requirement
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t ostream<Pointee, DerivedPolicies>::p_segcount(
        value_type * self, Py_ssize_t * lenp)
    {
        printf("in get_seg_count\n");
        if (lenp != NULL)
        {
            *lenp = p_size(self->m_stream);
        }
        return 1;
    }
    //@}

# endif

    //@{
    /// PyMappingMethod functions
    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_ass_subscript(
        value_type* self, PyObject* item, PyObject* value)
    {
        printf("In p_ass_subscript\n");
        Pointee * m_ostream = self->m_stream;
        Py_ssize_t selfsize = p_size(m_ostream);
        Py_ssize_t othersize;
        // Almost exclusively taken from bufferobject.c
        PyBufferProcs *pb;
        pb = value ? value->ob_type->tp_as_buffer : NULL;
        if ( pb == NULL ||
             pb->bf_getreadbuffer == NULL ||
             pb->bf_getsegcount == NULL )
        {
            PyErr_BadArgument();
            return -1;
        }
        if ( (*pb->bf_getsegcount)(value, NULL) != 1 )
        {
            /* ### use a different exception type/message? */
            PyErr_SetString(PyExc_TypeError,
                            "single-segment buffer object expected");
            return -1;
        }
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
            void *ptr2;

            if (PySlice_GetIndicesEx((PySliceObject *)item, selfsize,
                            &start, &stop, &step, &slicelength) < 0)
                return -1;

            if ((othersize = (*pb->bf_getreadbuffer)(value, 0, &ptr2)) < 0)
                return -1;

            if (othersize != slicelength) {
                PyErr_SetString(
                    PyExc_TypeError,
                    "right operand length must match slice length");
                return -1;
            }

            threadstate UNBLOCK_THREADS;
            if (slicelength == 0)
                return 0;
            else if (step == 1) {
                m_ostream->write(static_cast<char*>(ptr2), slicelength);
                return 0;
            } else {
                Py_ssize_t cur, i;
                m_ostream->seekp(start);
                std::streambuf * buf = m_ostream->rdbuf();
                for (cur = start, i = 0; i < slicelength; cur += step, i++)
                    buf->sputc(((char*)ptr2)[i]);
            }
            return 0;
        } else {
            PyErr_SetString(PyExc_TypeError,
                            "buffer indices must be integers");
            return -1;
        }
    }
    //@}

    //@{
    /// PySequenceMethods functions
    template <class Pointee, class DerivedPolicies>
    Py_ssize_t ostream<Pointee, DerivedPolicies>::p_length(
        value_type* self)
    {
        return p_size(self->m_stream);
    }

    template <class Pointee, class DerivedPolicies>
    typename ostream<Pointee, DerivedPolicies>::value_type *
    ostream<Pointee, DerivedPolicies>::p_concat(
        value_type* self, value_type* other)
    {
        printf("In iostream_base::p_concat\n");
        value_type * obj = base_type::p_new(&DerivedPolicies::m_type, Py_None, Py_None);
        threadstate UNBLOCK_THREADS;
        obj->m_stream->operator <<(self->m_stream->rdbuf());
        obj->m_stream->operator <<(other->m_stream->rdbuf());
        return obj;
    }

    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_ass_item(
        value_type *self, Py_ssize_t idx, PyObject* other)
    {
        printf("In ostream::p_ass_item\n");
        size_t size = p_size(self->m_stream);
        if (idx < 0 || idx > static_cast<Py_ssize_t>(size))
        {
            PyErr_SetString(PyExc_IndexError, "ostream assigment out of range");
            return -1;
        }
        char ch = extract<char>(other);
        threadstate UNBLOCK_THREADS;
        std::streambuf * m_buf = self->m_stream->rdbuf();
        m_buf->pubseekpos(std::streampos(idx), std::ios_base::out);
        //Pointee * m_ostream = p_seek(self->m_stream, pos_type(idx) );
        //m_ostream->put(ch);
        m_buf->sputc(ch);
        return 0;
    }

    template <class Pointee, class DerivedPolicies>
    int ostream<Pointee, DerivedPolicies>::p_ass_slice(
        value_type* self, Py_ssize_t left, Py_ssize_t right, PyObject* other)
    {
        printf("In ostream::p_ass_slice\n");
        // Almost exclusively taken from bufferobject.c
        void *ptr2;
        PyBufferProcs *pb;
        Py_ssize_t sl_len;
        Py_ssize_t count;

        pb = other ? other->ob_type->tp_as_buffer : NULL;
        if ( pb == NULL ||
             pb->bf_getreadbuffer == NULL ||
             pb->bf_getsegcount == NULL )
        {
            PyErr_BadArgument();
            return -1;
        }
        if ( (*pb->bf_getsegcount)(other, NULL) != 1 )
        {
            /* ### use a different exception type/message? */
            PyErr_SetString(PyExc_TypeError,
                            "single-segment buffer object expected");
            return -1;
        }
        if ( (count = (*pb->bf_getreadbuffer)(other, 0, &ptr2)) < 0 )
            return -1;

        // Python's buffer, in bufferobject.c is much stricter than this. I
        // found it didn't work too well, and over-complicated things. Also, the
        // streambuf object seems to be safe, even when referring to indices
        // outside of the current buffer region.
        Pointee * m_ostream = self->m_stream;
        std::streambuf * m_buf = self->m_stream->rdbuf();
        Py_ssize_t size = p_size(self->m_stream);

        threadstate UNBLOCK_THREADS;
        if ( left < 0 )
            left = 0;
        else if (left > size)
        {
            m_buf->pubseekpos(std::streampos(size), std::ios_base::out);
            register char c = m_ostream->fill();
            while (size++ < left)
            {
                m_buf->sputc(c);
            }
        }
        sl_len = right - left;
        if ( right < left )
            right = left;
        else if (right > (size + sl_len))
            m_ostream->width(right);

        m_buf->pubseekpos(std::streampos(left), std::ios_base::out);
        //m_ostream->operator <<(ptr2);
        if ( sl_len )
            m_buf->sputn(static_cast<const char*>(ptr2), sl_len);

        return 0;
    }
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
        if (state & std::ios_base::eofbit)
            // If eofbit is set, assume we're at the end of the buffer, so just
            // return the current position (safely).
            try {
                return (size_t) p_tell(ptr);
            }
            catch (std::string err)
            {
                UNBLOCK_THREADS.~threadstate();
                PyErr_SetString(PyExc_IOError, err.c_str());
                PyErr_Print();
                return -1;
            }
        else if ((state & (std::ios_base::failbit | std::ios_base::badbit)) > 0)
            ptr->clear();
        try {
            cur_pos   = p_tell(ptr);  //< Get current position
        }
        catch (std::string err)
        {
            UNBLOCK_THREADS.~threadstate();
            PyErr_SetString(PyExc_IOError, err.c_str());
            PyErr_Print();
            return -1;
        }
        ptr = p_seek(ptr, 0, ptr->end); //< Seek to the end
        size = ptr->tellp();      //< Calculate the size.
        ptr->seekp(cur_pos);      //< Go back to previous position.
        return size;
    }

    template <class Pointee, class DerivedPolicies>
    size_t ostream<Pointee, DerivedPolicies>::p_tell(Pointee * ptr)
    {
        pos_type cur_pos;
        if ((cur_pos = ptr->tellp()) >= 0)
            return cur_pos;
        /// Get state, to print a (relatively) useful exception message.
        //  On failure, state could be any of:-
        ///   eofbit  - no characters available to read
        ///   failbit - sentry construction failed
        ///   badbit  - stream error
        std::ios_base::iostate state = ptr->rdstate();
        std::string err("OStream tellp() call failed.\n");
        if (state & std::ios_base::failbit)
            err += "Logical error on i/o operation.";
        else if (state & std::ios_base::badbit)
            err += "Read / write error on i/o operation.";
        else
            // Shouldn't get here!
            err += "Unknown error. Please submit a bug report";
        throw err;
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
        NULL,                               // bf_getreadbuffer
        (writebufferproc)&p_writebuf,    // bf_getwritebuffer
        (segcountproc)   &p_segcount,    // bf_getsegcount
        NULL,                               // bf_getcharbuffer
        NULL,                               // bf_getbuffer
        NULL                                // bf_releasebuffer
    } ;

    template <class Pointee, class DerivedPolicies>
    PySequenceMethods ostream<Pointee, DerivedPolicies>::m_sequence = {
        (lenfunc)p_length,                 // sq_length
        (binaryfunc)p_concat,              // sq_concat
        NULL,                              // sq_repeat
        NULL,                              // sq_item
        NULL,                              // sq_slice
        (ssizeobjargproc)p_ass_item,       // sq_ass_item
        (ssizessizeobjargproc)p_ass_slice, // sq_ass_slice
    };

    template <class Pointee, class DerivedPolicies>
    PyMappingMethods ostream<Pointee, DerivedPolicies>::m_mapping = {
        (lenfunc)p_length,              // mp_length
        NULL,                           // mp_subscript
        (objobjargproc)p_ass_subscript, // mp_ass_subscript
    };

    // m_type
    //
    // The PyTypeObject struct that manages each ostreamed PyObject instance.
    template <class Pointee, class DerivedPolicies>
    typename ostream<Pointee, DerivedPolicies>::object_type
        ostream<Pointee, DerivedPolicies>::m_type =
    {
        PyVarObject_HEAD_INIT(NULL, 0)
        0 //const_cast<char*>("Boost.Python.OStream") // tp_name
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
        , 0                                         // tp_str
        , PyObject_GenericGetAttr                   // tp_getattro
        , 0                                         // tp_setattro
        , &m_buffer                                 // tp_as_buffer
        , Py_TPFLAGS_DEFAULT                        //< tp_flags
          | Py_TPFLAGS_BASETYPE  
# if PY_VERSION_HEX < 0x03000000
          | Py_TPFLAGS_HAVE_WEAKREFS
#   if PY_VERSION_HEX >= 0x02060000
          | Py_TPFLAGS_HAVE_NEWBUFFER
#   endif
# endif
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

}   }