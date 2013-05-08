#ifndef __BOOST_PYTHON_RETURN_IOS_BASE_INCLUDED__
#define __BOOST_PYTHON_RETURN_IOS_BASE_INCLUDED__

/// \file return_iostream_base.hpp
///

# include <boost/python/handle.hpp>
# include <boost/python/object/add_to_namespace.hpp>
# include <boost/python/return_internal_reference.hpp>
# include <boost/python/reference_existing_object.hpp>
# include <boost/python/scope.hpp>
# include <boost/python/str.hpp>
# include <boost/python/to_python_indirect.hpp>

# include <boost/type_traits/is_same.hpp>
# include <boost/mpl/if.hpp>

#include "boost_helpers/get_object_id.hpp"
#include "ios_base.hpp"


namespace boost { namespace python { namespace detail {

        /// register_iostream_base_pytype
        ///
        /// Used to register the PyTypeObject in the Boost Python registry.
        /// Derived buffer classes need this so the registry has a map of the
        /// full PyTypeObject inheritance chain.
        template <class T, class DerivedPolicies>
        struct register_iostream_base_pytype
        {
            typedef converters::iostream_base<T, DerivedPolicies> stream_t;
            register_iostream_base_pytype(void)
            {
                typedef typename stream_t::object_type object_type;
                printf("  Looking up iostream_base: %s \n", typeid(object_type).name());
                converter::registration& iostream_base_converter
                    = const_cast<converter::registration&>(
                        converter::registry::lookup(type_id<object_type>()) );

                if (iostream_base_converter.m_class_object == 0)
                {
                    printf("  Registering iostream_base %s \n", typeid(object_type).name());
                    iostream_base_converter.m_class_object = &stream_t::m_type;
                    // Register to_python conversion for the base-type.
                    converter::registry::insert(
                        (converter::to_python_function_t)&stream_t::convert,
                        type_id<object_type>(),
                        &stream_t::get_pytype );
                    // This is needed so Boost Python knows that derived classes
                    // can be passed, where pointers to stream_t are expected.
                    objects::register_dynamic_id<stream_t>();
                }
            }
        };

        /// make_buffer_object_holder
        ///
        /// A HolderGenerator, customised from  return_internal_reference,
        /// which allows delegation to some externally defined "MakeHolder".
        /// It is described as "A class whose static execute() creates an
        /// instance_holder")
        ///
        /// HolderGenerator model based on:-
        ///   boost/python/to_python_indirect.hpp - detail::make_reference_holder and
        ///   boost/python/to_python_value.hpp    - detail::registry_to_python_value
        struct make_iostream_object_holder
        {
            /// Turns a pointer to a C++ type "T" into a "PyObject *" using registered
            /// type lookup. This means C++ type must be manually registered for
            /// conversion
            ///
            /// @param T  Parameterised C++ type to convert
            /// @param p  Pointer to instance of C++ type to convert
            /// @return   Python object built from registered conversion code
            template <class T>
            static PyObject* execute(T* p)
            {
                printf("in make_iostream_object_holder::execute!!!\n");
                typedef converter::registered<T> registration;
        # if BOOST_WORKAROUND(__GNUC__, < 3)
                // suppresses an ICE, somehow
                (void)registration::converters;
        # endif 
                // Jump into conversion lookup mechanism
                return registration::converters.to_python(p);
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
            reference_iostream_base_impl()
            {
                // don't initialise this..
                printf("In reference_iostream_base_impl()\n");
            }
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

      // return_iostream_object
      //  A ResultConverterGenerator for buffered streams.
      typedef reference_iostream_base_impl< make_iostream_object_holder >
          reference_iostream_base;

      /// Custom return value policy, allowing use of different
      /// "ResultConverterGenerator"  model rather than
      /// \ref reference_existing_object.
      /// Falls back to Boost implementation if it ceases to use
      /// reference_existing_object.
      ///   @param ResultConverterGenerator  - "ResultConverterGenerator" model replacement
      ///                                       for "reference_existing_object"
      ///   @param  owner_arg                - \sa return_internal_reference
      ///   @param  BasePolicy_              - \sa return_internal_reference
      template <class  ResultConverterGenerator = reference_iostream_base >
      struct return_iostream_base_object
          : return_internal_reference<>
      {
          typedef ResultConverterGenerator result_converter;
          return_iostream_base_object()
          {
              printf("In return_iostream_base_object()\n");
          }
      };

  } // * Ends namespace detail * //

  // add_type_to_module
  //
  // Add type T's unqualified name to the current global level scope, 
  // registering it to the given PyTypeObject `value`.
  // If name is given, register the PyTypeObject `value` under that name.
  // This must be called, directly or indirectly, from within a
  // BOOST_PYTHON_MODULE block.
  template <class T>
  void add_type_to_module(PyTypeObject * value, const char* name = NULL)
  {
      printf(" adding %s - %s to module namespace\n", name, value->tp_name);
      // Get the current (module) scope
      scope module;
      if (name == NULL)
      {
          // unqualified name of C++ class.
          name = unqualify_id<T>();
      }
      // Add object to current namespace.
      PyModule_AddObject(module.ptr(), name, (PyObject*)value);
  }

  // Make a PyTypeObject that follows the ios_base protocol.
  //
  // @param Pointee - C++ class to expose to Python
  template <class Pointee, class DerivedPolicies>
  converters::iostream_base<Pointee, DerivedPolicies>
      make_iostream_base_type_object(void)
  { 
      printf("making Python version of IOStream_base type object\n");
      typedef typename converters::iostream_base<Pointee,
                                                 DerivedPolicies> base_object;

      base_object::object_type.tp_name
          = const_cast<char*>(type_id<Pointee>().name());

      if (PyType_Ready(&base_object::object_type) < 0)
          boost::python::throw_error_already_set();
      Py_INCREF(&base_object::object_type);
      return base_object();
  }

  // Bring public types into boost::python namespace.
  // return_iostream_base
  //  A CallPolicy to refer to buffered streams.
  typedef detail::return_iostream_base_object<> return_iostream;

  //   It's a ResultConverterGenerator
  typedef detail::reference_iostream_base reference_iostream_base;


}  } // End boost::python namespace

#endif
