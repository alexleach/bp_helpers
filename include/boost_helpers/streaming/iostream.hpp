#ifndef __BOOST_PYTHON_IOSTREAM_INCLUDED__
#define __BOOST_PYTHON_IOSTREAM_INCLUDED__

#include "istream.hpp"
#include "ostream.hpp"

namespace boost { namespace python { 

    // Forward declaration
    template <class Pointee, class DerivedPolicies>
    class iostream;

    namespace detail
    {
        template <class Pointee>
        class final_iostream_derived_policies
            : public iostream<Pointee,
                              final_iostream_derived_policies<Pointee> > {};
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
            = detail::final_iostream_derived_policies<Pointee> >
    class iostream
        : public istream<Pointee, DerivedPolicies>
        , public ostream<Pointee, DerivedPolicies>
    {
    public:

        //@{
        /// Wrapper class typedefs
        typedef converter::iostream_base<Pointee, DerivedPolicies>  base_type;
        typedef istream<Pointee, DerivedPolicies>                istream_base;
        typedef ostream<Pointee, DerivedPolicies>                ostream_base;
        typedef iostream<Pointee, DerivedPolicies>             container_type;

        // PyObject being managed
        typedef typename base_type::value_type    value_type;
        // PyTypeObject being managed
        //typedef typename base_type::object_type  object_type;
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

        //@{
        /// Static members
        static object_type          m_type;
        static PyBufferProcs      m_buffer; 
        /// PyMappingMethods for the mapping protocol:-
        static PyMappingMethods    m_mapping;
        /// PySequenceMethods for the sequence protocol:-
        static PySequenceMethods  m_sequence;
        //@}

        //@{
        /// ctor overloads, same as those offered by \ref `boost::python::string`
        iostream(void);
        iostream(const char* s);
        iostream(char const* start, char const* finish);
        iostream(char const* start, std::size_t length);
        explicit iostream(object const& other){}
        template <class T> explicit iostream(T const& other);
        //@}
        // dtor
        ~iostream() { }

        // caller
        PyObject* operator()(void);
        PyObject* operator()(Pointee& other);

        // Getter for the PyTypeObject.
        static object_type const * get_pytype(void);
        static object_type * set_pytype(void);

    private:
        //@{
        // PyTypeObject functions
        static int      p_init(value_type  * self,
                               PyObject    * args,
                               PyObject    * kwds);

        static PyObject* p_repr(value_type * self);
        static PyObject*  p_str(value_type  * self);
        //@}

        //@{
        /// PyMappingMethods functions:-
        static int   p_ass_subscript(value_type* self, PyObject* item,
                                     PyObject* value);
        //@}

        //@{
        /// PySequenceMethods functions:-
        static int        p_ass_item(value_type *self, Py_ssize_t idx,
                                     PyObject* other);
        static int       p_ass_slice(value_type* self, Py_ssize_t left,
                                     Py_ssize_t right, PyObject* other);
        //@}
    };

}  } // End Boost Python namespaces

# include "iostream.tcc"

#endif
