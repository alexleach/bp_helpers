#ifndef __BOOST_PYTHON_RETURN_ISTREAM_INCLUDED__
#define __BOOST_PYTHON_RETURN_ISTREAM_INCLUDED__

/// \file custom_return_internal_reference.hpp
///


# include <boost/python/to_python_value.hpp>
# include <boost/python/converter/registry.hpp>

# include <boost/mpl/assert.hpp>

#include "return_iostream_base.hpp"
#include "istream.hpp"

namespace boost { namespace python {

  // register_istream_pytype
  //
  // Attempt to register each used buffer object, on module import, so 
  // we don't need to make a new HolderGenerator each time.
  template <class T>
  struct register_istream_pytype
      : detail::register_iostream_base_pytype<T,
          detail::final_istream_derived_policies<T> >
  {
      typedef istream<T> stream_t;
      register_istream_pytype(void)
          : detail::register_iostream_base_pytype<T,
              detail::final_istream_derived_policies<T> >()
      {
          printf("  Looking up IStream %s \n", typeid(T).name());

          // Insert type T into the registry.
          converter::registration& istream_converter
              = const_cast<converter::registration&>(
                  converter::registry::lookup(type_id<T>()) );

          if (istream_converter.m_class_object == 0)
          {
              printf("  Registering IStream %s \n", typeid(T).name());
              // Put the new PyTypeObject in to the registry.
              istream_converter.m_class_object = &stream_t::m_type;

              // register a to_python converter
              converter::registry::insert(
                  (converter::to_python_function_t)&stream_t::convert,
                  type_id<T>(),
                  &stream_t::get_pytype );

              // Register base class conversions.
              objects::register_conversion<stream_t,
                                           typename stream_t::base_type>(false);
          }
      }
  };

  // Make a PyTypeObject that follows the C++ STL istream protocol.
  //
  // @param Pointee - C++ class to expose to Python
  template <class Pointee>
  istream<Pointee> make_istream_type_object(void)
  { 
      printf("making Python version of istream-like object\n");
      typedef typename boost::python::istream<Pointee> istream_t;

      istream_t::m_type.tp_name = const_cast<char*>(type_id<Pointee>().name());

      if (PyType_Ready(&istream_t::m_type) < 0)
          boost::python::throw_error_already_set();

      Py_INCREF(&istream_t::m_type);
      register_istream_pytype<Pointee>();
      return istream_t();
      //return istream_t::m_type;
  }

}  } // End boost::python namespace

#endif
