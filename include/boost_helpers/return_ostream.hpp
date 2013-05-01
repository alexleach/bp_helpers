#ifndef __BOOST_PYTHON_RETURN_OSTREAM_INCLUDED__
#define __BOOST_PYTHON_RETURN_OSTREAM_INCLUDED__

/// Thanks to Mark English for sharing this. Taken and modified from a post he
/// made to the cplusplus-sig mailing list, at:-
///    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767

/// \file custom_return_internal_reference.hpp
///


#include <boost/python/to_python_value.hpp>

#include "boost_helpers/return_iostream_base.hpp"
#include "boost_helpers/ostream.hpp"

#include <boost/mpl/assert.hpp>


namespace boost { namespace python { namespace detail {

        // This is how return_opaque_pointer creates an instance:
        template <class Pointee>
        static void ostream_pointee(Pointee const volatile*)
        {
            printf("buffer pointee\n");
            force_instantiate(converters::ostream<Pointee>::instance);
        }

  } // * Ends namespace detail * //

  /// Custom return value policy, allowing use of different
  /// "ResultConverterGenerator"  model rather than
  /// \ref reference_existing_object.
  /// Falls back to Boost implementation if it ceases to use
  /// reference_existing_object.
  ///   @param ResultConverterGenerator  - "ResultConverterGenerator" model replacement
  ///                                       for "reference_existing_object"
  ///   @param  owner_arg                - \sa return_internal_reference
  ///   @param  BasePolicy_              - \sa return_internal_reference
  template <class  ResultConverterGenerator = return_iostream_base,
            size_t owner_arg = 1,
            class  BasePolicy_ = return_internal_reference<> >
  struct return_ostream_object
      : BasePolicy_
  {
      /// Possible to test for Read-only or read-write buffer?
      //mpl::if_< is_same<

      typedef ResultConverterGenerator result_converter;

      template<class R>
      struct apply
      {
          BOOST_MPL_ASSERT_MSG( is_pointer<R>::value,
              RETURN_OSTREAM_OBJECT_EXPECTS_A_POINTER_TYPE, (R));

          struct type :
              boost::python::to_python_value<
                  typename detail::value_arg<R>::type
              >
          {
              type()
              {
                  printf("in return_buffer_reference::apply::type\n");
                  detail::ostream_pointee(R());
              }
          };
      };
  };

  // register_ostream_pytype
  //
  // Register a C++ class that conforms to the C++ STL ostream interface, on
  // module import.
  template <class T>
  struct register_ostream_pytype
  {
      typedef converters::ostream<T> buffer_t;
      register_ostream_pytype(void)
      {
          printf("Registering %s \n", typeid(buffer_t).name() );
          converter::registry::insert(
              (converter::to_python_function_t)&buffer_t::convert,
              boost::python::type_id<T>(),
              &buffer_t::get_pytype );
      }
  };

  // Make a PyTypeObject that follows the ostream protocol.
  //
  // @param Pointee - C++ class to expose to Python
  template <class Pointee>
  PyTypeObject make_ostream_type_object(void)
  { 
      printf("making Python version of ostream-like object\n");
      converters::ostream<Pointee>::object_type.tp_name
          = const_cast<char*>(type_id<Pointee*>().name());

      if (PyType_Ready(&converters::ostream<Pointee>::object_type) < 0)
          boost::python::throw_error_already_set();

      boost::python::register_ostream_pytype<Pointee> x;
      Py_INCREF(&converters::ostream<Pointee>::object_type);
      return converters::ostream<Pointee>::object_type;
  }

}  } // End boost::python namespace


#endif
