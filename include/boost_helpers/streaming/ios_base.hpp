#ifndef __BOOST_PYTHON_IOS_BASE_INCLUDED__
#define __BOOST_PYTHON_IOS_BASE_INCLUDED__

//#include <boost/python/def_visitor.hpp>
//#include <boost/python/object.hpp>
//#include <boost/python/register_ptr_to_python.hpp>
#include <structmember.h>   ///< for T_OBJECT
#include <ios>

#include "boost_helpers/make_threadsafe.hpp"

namespace boost { namespace python { namespace converters {

    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // template class iostream_base

    /**
     * A Python object instance holder, for C++ objects with iostream-like
     * interfaces.
     *
     * Aims to follow indexing_suite semantics for exposing a C++ stream as a
     * PyObject. This is achieved by inheriting from def_visitor, and defining
     * a 'visit' instance method. Subclasses then define an 'extension_def'
     * method for attaching additional Python methods to the class.
     *
     * @param DerivedPolicies
     *
     * To register derived class methods in the Python object, this base class
     * is passed a `DerivedPolicies` class, which is used to call methods on
     * derived classes. This method for registering classes was originally taken
     * from boost's indexing_suite headers. See them and the other stream
     * headers for examples on how to initialise this base class.
    */
    template <
        class Pointee,
        class DerivedPolicies
    >
    class iostream_base
        //: public def_visitor<
        //    iostream_base<Pointee, DerivedPolicies> >
    {
    public:
        // Python object Typedefs
        typedef struct BoostPythonBuffer
        {
          PyObject_HEAD
          PyObject * m_weakrefs;
          Pointee  * m_stream;
        } value_type;

        typedef PyTypeObject                   object_type;

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


        //@{ ctor overloads, same as in `bp::str`.
        iostream_base(void);
        iostream_base(const char* s);
        iostream_base(char const* start, char const* finish);
        iostream_base(char const* start, std::size_t length);

        // Templated constructor
        template <class T>
        explicit iostream_base(T const& other) ;
        //@}
        // dtor
        ~iostream_base() { }

        // caller
        PyObject* operator()(void);
        PyObject* operator()(Pointee& other);

        //@{
        ///  Boost Python converter requirements
        static bool   convertible(void) { printf("checking iostream-base convertible\n"); return true; }
        static PyObject * convert(Pointee & value);

  #ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        static object_type const * get_pytype();
  #endif
        //@}

        //@{
        /// \brief Static members
        /// Objects providing support for Python iostream_base protocol:-
        static object_type         m_type;

    protected:
        /// Could be, or should be, defined in derived classes.
        static PyMemberDef       m_members[];
        /// PyBufferProcs for the old-style buffer protocol:-
        static PyBufferProcs        m_stream; 
        /// PyMappingMethods for the mapping protocol:-
        static PyMappingMethods    m_mapping;
        /// PySequenceMethods for the sequence protocol:-
        static PySequenceMethods  m_sequence;
        //@}

        //@{
        /// Generic PyTypeObject member functions:
        static PyObject* p_new(object_type * subtype,
                               PyObject    * args,
                               PyObject    * kwds);
        static void  p_dealloc(value_type  * obj);
        static PyObject* p_repr(value_type * self);
        //@}

    };


} } } // End Boost Python namespaces

# include "ios_base.tcc"

#endif
