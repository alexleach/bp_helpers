#ifndef __BOOST_PYTHON_BUFFER_INCLUDED__
#define __BOOST_PYTHON_BUFFER_INCLUDED__

#include "ios_base.hpp"
#include <istream>

namespace boost { namespace python {

    // Forward declaration
    template <class Pointee, class DerivedPolicies>
    class istream;

    namespace detail
    {
        // Implementation detail.
        template <class Pointee>
        class final_istream_derived_policies
            : public istream<Pointee,
                final_istream_derived_policies<Pointee> > {};
    }


    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // buffer<> template.
    //
    // A Python object instance holder, for C++ objects with iostream-like
    // interfaces.
    template <
        class Pointee,
        class DerivedPolicies 
            = detail::final_istream_derived_policies<Pointee> >
    class istream
        : virtual public converters::iostream_base<Pointee, DerivedPolicies>
    {
    public:

        //@{
        /// IStream wrapper typedefs
        // Shortcut typedef for the base class template.
        typedef converters::iostream_base<Pointee, DerivedPolicies> base_type;
        // Shortcut typedef for this class template
        typedef istream<Pointee, DerivedPolicies>              container_type;
        // PyObject being managed
        typedef typename base_type::value_type    value_type;
        // PyTypeObject being managed
        typedef typename base_type::object_type  object_type;
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

        //@{
        /// Static objects
        /// A custom PyTypeObject.
        static object_type            m_type;
        //static PyMemberDef  type_members[];
        /// PyBufferProcs for the buffer protocol:-
        static PyBufferProcs        m_buffer; 
        /// PyMappingMethods for the mapping protocol:-
        static PyMappingMethods    m_mapping;
        /// PySequenceMethods for the sequence protocol:-
        static PySequenceMethods  m_sequence;
        //@}

        //@{
        /// ctor overloads, similar to \ref `boost::python::string`
        istream() ;
        istream(const char* s);
        istream(char const* start, char const* finish);
        istream(char const* start, std::size_t length);
        template <class T>
        explicit istream(T const& other) ;
        //@}

        // dtor
        ~istream() { }

        // caller.. Needed?
        PyObject* operator()(void);
        PyObject* operator()(Pointee& other);

        //@{
        /// PyTypeObject member functions
        static int       p_init(value_type  * self,
                                PyObject    * args,
                                PyObject    * kwds);
        static PyObject * p_new(PyTypeObject * subtype,
                                PyObject    * args,
                                PyObject    * kwds);
        static void   p_dealloc(value_type  * obj);
        static long int  p_hash(value_type  * self);
        static PyObject* p_repr(value_type  * self);
        static PyObject*  p_str(value_type  * self);
        //@}

        //@{
        /// \brief PyBufferProcs member functions
        ///   PyBufferProcs functions to enable C++ iostream support in Python
        ///   buffer objects
        static Py_ssize_t p_readbuf(value_type* self, Py_ssize_t idx, void **pp);
        // TODO:
        static Py_ssize_t p_segcount(value_type* self, Py_ssize_t * lenp);
        static Py_ssize_t p_charbuf(value_type* self, Py_ssize_t idx, void **pp);
        //@}

        //@{
        /// PyMappingMethods functions:-
        static PyObject* p_subscript(value_type* self, PyObject* item);
        //@}

        //@{
        /// PySequenceMethods functions:-
        static Py_ssize_t p_length(value_type* self);
        static PyObject * p_concat(value_type* self, PyObject* other);
        static PyObject * p_repeat(value_type* self, Py_ssize_t count);
        static PyObject *   p_item(value_type* self, Py_ssize_t idx);
        static PyObject *  p_slice(value_type* self, Py_ssize_t left,
                                   Py_ssize_t right);
        static int      p_ass_item(value_type *self, Py_ssize_t idx,
                                   PyObject* other);
        static int     p_ass_slice(value_type* self, Py_ssize_t left,
                                   Py_ssize_t right, PyObject* other);
        //@}

    private:
        // get buffer size
        static size_t p_size(Pointee * ptr);
        static size_t p_tell(Pointee * ptr);
        // seekg() helpers
        static Pointee* p_seek(Pointee* ptr, pos_type pos);
        static Pointee* p_seek(Pointee* ptr, off_type offset, seekdir way);
        // Called by p_seek methods, to throw an appropriate exception if an
        // operation failed.
        static void check_seek_state(Pointee * ptr);

    };

}  } // End Boost Python Converter namespaces

# include "istream.tcc"

#endif
