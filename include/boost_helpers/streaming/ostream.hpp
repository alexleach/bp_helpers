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
        : virtual public converters::iostream_base<Pointee, DerivedPolicies>
    {
    public:

        //@{
        /// OStream wrapper typedefs
        typedef converters::iostream_base<Pointee, DerivedPolicies> base_type;
        typedef ostream<Pointee, DerivedPolicies>              container_type;
        typedef typename base_type::value_type   value_type;
        typedef typename base_type::object_type object_type;
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
        ~ostream() { }

        //@{
        /// Call operators
        PyObject* operator()(void);
        PyObject* operator()(Pointee& other);
        //@}

        //@{
        // PyTypeObject members
        static int        p_init(value_type  * self,
                                 PyObject    * args,
                                 PyObject    * kwds);
        static PyObject *  p_new(PyTypeObject * subtype,
                                 PyObject     * args,
                                 PyObject     * kwds);
        // Custom dealloc method for the ostreamed class.
        static void    p_dealloc(value_type   * obj);
        // PyBufferProcs functions enabling C++ iostream support in Python ostream
        // objects
        static PyObject * p_repr(value_type   * self);
        //@}

        //@{
        // PyBufferProcs member functions
        // TO TEST:
        static Py_ssize_t p_writebuf(value_type* self, Py_ssize_t idx, void **pp);

        // TODO:
        static Py_ssize_t p_segcount(value_type* self, Py_ssize_t * lenp);
        //@}

        //@{
        /// PyMappingMethods functions:-
        static PyObject* p_subscript(value_type* self, PyObject* item);
        static int   p_ass_subscript(value_type* self, PyObject* item,
                                     PyObject* value);
        //@}

        //@{
        /// PySequenceMethods functions:-
        static Py_ssize_t p_length(value_type* self);
        static PyObject * p_concat(value_type* self, PyObject* other);
        static PyObject *   p_item(value_type* self, Py_ssize_t idx);
        static PyObject *  p_slice(value_type* self, Py_ssize_t left,
                                   Py_ssize_t right);
        static int      p_ass_item(value_type *self, Py_ssize_t idx,
                                   PyObject* other);
        static int     p_ass_slice(value_type* self, Py_ssize_t left,
                                   Py_ssize_t right, PyObject* other);
        //@}

        // Boost Python visitor requirements (register Python-side methods)
        /*
        template <class Class>
        static void extension_def(Class &cl);
        */

        // PyMethodDef functions. i.e. `value_type` instance methods
        static Pointee & p_write_int(Pointee&, int value);

    private:
        static size_t p_size(Pointee * ptr);
        static size_t p_tell(Pointee * ptr);
        static Pointee* p_seek(Pointee* ptr, pos_type pos);
        static Pointee* p_seek(Pointee* ptr, off_type offset, seekdir way);
        static void check_seek_state(Pointee * ptr);
    };

}  } // End Boost Python namespaces

# include "ostream.tcc"

#endif
