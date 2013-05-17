#ifndef __BOOST_PYTHON_NEW_STYLE_BUFFER__
#define __BOOST_PYTHON_NEW_STYLE_BUFFER__

namespace boost { namespace python { 

    // RAII for Python's new-style buffers, aka. `Py_buffer`
    class BufferLocker
    {
    public:
        BufferLocker(void) : m_buf(NULL) {}
        BufferLocker(Py_buffer * buf) : m_buf(buf) {}

        void Lock(Py_buffer * buf)   { m_buf = buf; }
        void Unlock(Py_buffer * buf) { PyBuffer_Release(m_buf); }

        ~BufferLocker()
        {
            if (m_buf != NULL)
                Unlock(m_buf);
        }

    private:
        Py_buffer * m_buf;
    };

}   }


#endif