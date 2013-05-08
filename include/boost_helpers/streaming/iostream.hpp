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
      typedef converters::iostream_base<Pointee, DerivedPolicies> base_type;
      typedef istream<Pointee, DerivedPolicies>                istream_base;
      typedef ostream<Pointee, DerivedPolicies>                ostream_base;
      typedef iostream<Pointee, DerivedPolicies>             container_type;

      // PyObject being managed
      typedef typename base_type::value_type    value_type;
      // PyTypeObject being managed
      typedef typename base_type::object_type  object_type;
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

      //@{
      // PyTypeObject functions
      static int      p_init(value_type  * self,
                             PyObject    * args,
                             PyObject    * kwds);
      static PyObject* p_new(PyTypeObject * subtype,
                             PyObject     * args,
                             PyObject     * kwds);

      // Custom dealloc method for the buffered class.
      static void p_dealloc(value_type * obj);
      static void p_free(value_type * obj);

      static PyObject* p_repr(value_type * self);
      //@}

    };

}  } // End Boost Python namespaces

# include "iostream.tcc"

#endif
