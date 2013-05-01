#ifndef __BOOST_PYTHON_RETURN_IOS_BASE_INCLUDED__
#define __BOOST_PYTHON_RETURN_IOS_BASE_INCLUDED__

/// \file return_iostream_base.hpp
///

# include <boost/python/return_internal_reference.hpp>
# include <boost/python/reference_existing_object.hpp>
# include <boost/python/to_python_indirect.hpp>

# include <boost/type_traits/is_same.hpp>
# include <boost/mpl/if.hpp>

#include "boost_helpers/ios_base.hpp"


namespace boost { namespace python { namespace detail {


        //////////////////////////////////////////////////////////////////////
        /// make_buffer_object_holder
        ///
        /// A HolderGenerator, customised from  return_internal_reference,
        /// which allows delegation to some externally defined "MakeHolder".
        /// It is described as "A class whose static execute() creates an
        /// instance_holder")

        /// Default "MakeHolder" model based on
        /// boost/python/to_python_indirect.hpp - detail::make_reference_holder and
        /// boost/python/to_python_value.hpp    - detail::registry_to_python_value
        struct make_iostream_object_holder
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
                printf("in make_iostream_object_holder::execute\n");
                // Typedef an 'instance holder'. Is this used anywhere?
                //typedef objects::pointer_holder<T*, T> holder_t;
                // Jump into conversion lookup mechanism
                //typedef T argument_type;
    # if BOOST_WORKAROUND(__GNUC__, < 3)
                //typedef converter::registered<argument_type> r;
                typedef converter::registered<T> r;
                // suppresses an ICE, somehow
                (void)r::converters;
    # endif 
                return converter::registered<T>::converters.to_python(
                                  const_cast<T*>(p) );
            }
        };

        ////////////////////////////////////////////////////////////////////////////
        /// ResultConverterGenerator
        ///
        /// reference_existing_object replacement allowing use of different
        /// "MakeHolder" model.
        ///   @param  Holder - Class modelling "MakeHolder"
        ///                    Defaults to make_iostream_object_holder

        template <class Holder = make_iostream_object_holder>
        struct reference_iostream_base_impl
            : return_internal_reference<>
        {
            typedef reference_iostream_base_impl<Holder> result_converter;
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

      // Typedef for result converter / CallPolicy
      typedef reference_iostream_base_impl< make_iostream_object_holder >
          reference_iostream_base;

  } // * Ends namespace detail * //

  // return_iostream_base
  //  A CallPolicy to refer to buffered objects.
  typedef detail::reference_iostream_base return_iostream_base;



  // -----    REGISTRATION FUNCTIONS    ------ //


  // register_iostream_base_pytype
  //
  // Register a C++ class that conforms to the C++ STL ios_base interface, on
  // module import.
  template <class T>
  struct register_iostream_base_pytype
  {
      typedef converters::iostream_base<T> buffer_t;
      register_iostream_base_pytype(void)
      {
          printf("Registering %s \n", typeid(buffer_t).name());
          converter::registry::insert(
              (converter::to_python_function_t)&buffer_t::convert,
              boost::python::type_id<T>(),
              &buffer_t::get_pytype );
      }
  };

  // Make a PyTypeObject that follows the ios_base protocol.
  //
  // @param Pointee - C++ class to expose to Python

  template <class Pointee>
  PyTypeObject make_iostream_type_object(void)
  { 
      printf("making Python version of IO stream-like object\n");
      converters::iostream_base<Pointee>::object_type.tp_name
          = const_cast<char*>(type_id<Pointee*>().name());
      if (PyType_Ready(&converters::iostream_base<Pointee>::object_type) < 0)
          boost::python::throw_error_already_set();
      boost::python::register_iostream_base_pytype<Pointee> x;
      //Py_INCREF(&iostream_base<Pointee>::object_type);
      return converters::iostream_base<Pointee>::object_type;
  }

}  } // End boost::python namespace

#endif
