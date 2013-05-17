#ifndef __BOOST_PYTHON_RETURN_BUFFER_OBJECT__
#define __BOOST_PYTHON_RETURN_BUFFER_OBJECT__

/// Thanks to Mark English for sharing this. Taken and modified from a post he
/// made to the cplusplus-sig mailing list, at:-
///    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767

/// \file custom_return_internal_reference.hpp
///


#include "boost_helpers/buffer.hpp"

# include <boost/python/return_internal_reference.hpp>
# include <boost/python/reference_existing_object.hpp>
# include <boost/python/to_python_indirect.hpp>
# include <boost/python/to_python_value.hpp>

# include <boost/mpl/if.hpp>
# include <boost/mpl/assert.hpp>
# include <boost/type_traits/is_same.hpp>

namespace boost { namespace python { namespace detail {


        //////////////////////////////////////////////////////////////////////
        /// make_buffer_object_holder
        ///
        /// A HolderGenerator, customised from  return_internal_reference,
        /// which allows delegation to some externally defined "MakeHolder"
        /// class (described as "A class whose static execute() creates an
        /// instance_holder")

        /// Default "MakeHolder" model based on
        /// boost/python/to_python_indirect.hpp - detail::make_reference_holder and
        /// boost/python/to_python_value.hpp    - detail::registry_to_python_value
        struct make_buffer_object_holder
        {
            /// Turns a pointer to a C++ type "T" into a "PyObject *" using registered
            /// type lookup. This means C++ type must be manually registered for
            /// conversion
            ///
            /// @param T  Parameterised C++ type to convert
            /// @param p  Pointer to instance of C++ type to convert
            /// @return Python object built from registered conversion code
            template <class T>
            static PyObject* execute(T* p)
            {
            // Shouldn't we only use this struct once, when the module is imported?
                printf("in make_buffer_object_holder::execute\n");
                // Typedef an 'instance holder'. Is this used anywhere?
                //typedef objects::pointer_holder<T*, T> holder_t;
                // Jump into conversion lookup mechanism
                typedef T argument_type;
    # if BOOST_WORKAROUND(__GNUC__, < 3)
                typedef converter::registered<argument_type> r;
                // suppresses an ICE, somehow
                (void)r::converters;
    # endif 
                return converter::registered<argument_type>::converters.to_python(
                                  const_cast<T*>(p) );
            }

        };

        ////////////////////////////////////////////////////////////////////////////
        /// ResultConverterGenerator

        /// reference_existing_object replacement allowing use of different
        /// "MakeHolder" model.
        ///   @param  Holder - Class modelling "MakeHolder"
        ///                    Defaults to make_buffer_object_holder

        template <class Holder = make_buffer_object_holder>
        struct reference_existing_buffer_impl
            : return_internal_reference<>
        {
            typedef reference_existing_buffer_impl<Holder> result_converter;
            /// Implicitly relies on "detail" namespace implementation, and falls back
            /// on that implementation if it changes
            template <class T>
            struct apply
            {
            private:
                typedef typename reference_existing_object::apply<T>::type basetype_;
            public:
                typedef typename mpl::if_<
                        is_same<basetype_
                                , to_python_indirect<T, make_reference_holder> >
                        , to_python_indirect<T, Holder>
                        , basetype_
                      >::type type;
            };
        };

        // This is how return_opaque_pointer creates an instance:
        template <class Pointee>
        static void buffer_pointee(Pointee const volatile*)
        {
            printf("buffer pointee\n");
            force_instantiate(buffer<Pointee>::instance);
        }

      // Typedef for result converter / CallPolicy
      typedef reference_existing_buffer_impl< make_buffer_object_holder >
          buffer_result_converter_t;

  } // * Ends namespace detail * //

  // reference_existing_buffer
  //  A call policy to refer to buffered objects.
  typedef detail::buffer_result_converter_t reference_existing_buffer;

  /// Custom return value policy, allowing use of different
  /// "ResultConverterGenerator"  model rather than
  /// \ref reference_existing_object.
  /// Falls back to Boost implementation if it ceases to use
  /// reference_existing_object.
  ///   @param ResultConverterGenerator  - "ResultConverterGenerator" model replacement
  ///                                       for "reference_existing_object"
  ///   @param  owner_arg                - \sa return_internal_reference
  ///   @param  BasePolicy_              - \sa return_internal_reference
  template <class  ResultConverterGenerator = reference_existing_buffer,
            size_t owner_arg = 1,
            class  BasePolicy_ = return_internal_reference<> >
  struct return_buffer_reference
      : BasePolicy_
  {
      /// Possible to test for Read-only or read-write buffer?
      //mpl::if_< is_same<

      typedef ResultConverterGenerator result_converter;

      template<class R>
      struct apply
      {
          BOOST_MPL_ASSERT_MSG( is_pointer<R>::value,
              RETURN_BUFFER_REFERENCE_EXPECTS_A_POINTER_TYPE, (R));

          struct type :
              boost::python::to_python_value<
                  typename detail::value_arg<R>::type
              >
          {
              type()
              {
                  printf("in return_buffer_reference::apply::type\n");
                  detail::buffer_pointee(R());
              }
          };
      };
  };

  // Register buffer_pytype_ptr
  //
  // Attempt to register each used buffer object, on module import, so 
  // we don't need to make a new HolderGenerator each time.
  template <class T>
  struct register_buffer_pytype
  {
      typedef buffer<T> buffer_t;
      register_buffer_pytype(void)
      {
          converter::registry::insert(
              (converter::to_python_function_t)&buffer_t::convert,
              boost::python::type_id<T>(),
              &buffer_t::get_pytype );
      }
  };

  // Make a PyTypeObject that follows the buffer protocol.
  //
  // @param Pointee - C++ class to expose to Python
  template <class Pointee>
  PyTypeObject make_buffer_type_object(const char * name = 0)
  { 
      printf("making Python version of IO stream-like object\n");
      //typedef typename boost::python::buffer<Pointee>   p_t;

      if (name == 0)
          buffer<Pointee>::object_t.tp_name = type_id<Pointee>().name();
      else
          buffer<Pointee>::object_t.tp_name = name;

      if (PyType_Ready(&buffer<Pointee>::object_t) < 0)
          boost::python::throw_error_already_set();
      boost::python::register_buffer_pytype<Pointee> x;
      //Py_INCREF(&buffer<Pointee>::object_t);
      return buffer<Pointee>::object_t;
  }

}  } // End boost::python namespace


#endif
