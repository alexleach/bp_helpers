#ifndef __BOOST_PYTHON_OSTREAM_INCLUDED__
#define __BOOST_PYTHON_OSTREAM_INCLUDED__

#include "ios_base.hpp"
#include <ostream>


namespace boost { namespace python { 

    // Forward declaration
    template <class Pointee, class DerivedPolicies>
    class ostream;

    namespace detail
    {
        template <class Pointee>
        class final_ostream_derived_policies
            : public ostream<Pointee,
                final_ostream_derived_policies<Pointee> > { };
    }

    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // ostream<> template.
    //
    // A Python object instance holder, for C++ objects with iostream-like
    // interfaces.
    template <
        class Pointee,
        class DerivedPolicies
            = boost::python::detail::final_ostream_derived_policies<Pointee> >
    class ostream
        : virtual public converter::iostream_base<Pointee, DerivedPolicies>
    {
    public:

        //@{
        /// OStream wrapper typedefs
        typedef converter::iostream_base<Pointee, DerivedPolicies> base_type;
        typedef ostream<Pointee, DerivedPolicies>              container_type;
        typedef typename base_type::value_type   value_type;
        //typedef typename base_type::object_type object_type;
        typedef PyTypeObject                                  object_type;
        //@}

        //@{ C++ Standard typedefs
        /// Pointee should have inherited from \ref std::basic_ios
        typedef typename Pointee::char_type      char_type;
        typedef typename Pointee::traits_type  traits_type;
        typedef typename Pointee::int_type        int_type;
        typedef typename Pointee::pos_type        pos_type;
        typedef typename Pointee::off_type        off_type;
        //@}

        //@{ C++ Standard typedefs
        /// Some typedefs Pointee should have inherited from \ref std::ios_base
        typedef typename Pointee::failure          failure;
        typedef typename Pointee::fmtflags        fmtflags;
        typedef typename Pointee::iostate          iostate;
        typedef typename Pointee::openmode        openmode;
        typedef typename Pointee::seekdir          seekdir;
        //@}

        // Objects providing support for Python ostream protocol:-
        static object_type            m_type;
        //static PyMemberDef  type_members[];
        static PyBufferProcs        m_buffer; 
        /// PyMappingMethods for the mapping protocol:-
        static PyMappingMethods    m_mapping;
        /// PySequenceMethods for the sequence protocol:-
        static PySequenceMethods  m_sequence;

        //@{
        /// ctor overloads, similar to `bp::str`.
        ostream();
        ostream(const char* s);
        ostream(char const* start, char const* finish);
        ostream(char const* start, std::size_t length);
        template <class T>
        explicit ostream(T const& other) ;
        //@}

        /// dtor
        virtual ~ostream() { }

        //@{
        // PyTypeObject members
        static int        p_init(value_type  * self,
                                 PyObject    * args,
                                 PyObject    * kwds);
        // PyBufferProcs functions enabling C++ iostream support in Python ostream
        // objects
        static PyObject * p_repr(value_type   * self);
        static PyObject *  p_str(value_type  * self);
        //@}

    # if PY_VERSION_HEX < 0x03000000
        //@{
        // PyBufferProcs member functions
        static Py_ssize_t p_writebuf(value_type* self, Py_ssize_t idx, void **pp);
        static Py_ssize_t p_segcount(value_type* self, Py_ssize_t * lenp);
        //@}
    # endif

        //@{
        /// PyMappingMethods functions:-
        static int   p_ass_subscript(value_type* self, PyObject* item,
                                     PyObject* value);
        //@}

        //@{
        /// PySequenceMethods functions:-
        static Py_ssize_t   p_length(value_type* self);
        static value_type * p_concat(value_type* self, value_type* other);
        static int        p_ass_item(value_type *self, Py_ssize_t idx,
                                     PyObject* other);
        static int       p_ass_slice(value_type* self, Py_ssize_t left,
                                     Py_ssize_t right, PyObject* other);
        //@}

        //@{
        /// Boost Python visitor requirements (register Python-side methods)
        void extension_def(void);
        //@}

        //@{
        /// PyMethodDef functions. i.e. PyObject * (value_type) methods
        /// These should be added to the object's namespace by the class
        /// constructor.
        static value_type * p_write_int(value_type * self, int value);
        value_type * write(value_type * self, std::string const & value);
        //@}

        //@{
        // Generic attribute getter and setter for the PyTypeObject.
        static object_type const * get_pytype(void);
        static object_type * set_pytype(void);
        //@}

        //@{
        /// PyObject (value_type) methods available from Python.
        //@}

    protected:
        static int p_del_slice(Pointee * self, Py_ssize_t left, Py_ssize_t right);
        static int_type p_size(Pointee * ptr);
        static pos_type p_tell(Pointee * ptr);
        static Pointee* p_seek(Pointee* ptr, pos_type pos);
        static Pointee* p_seek(Pointee* ptr, off_type offset, seekdir way);

    private:
        static void check_seek_state(Pointee * ptr);
    };

}  } // End Boost Python namespaces

# include "ostream.tcc"

#endif
