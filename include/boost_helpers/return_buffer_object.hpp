/// Thanks to Mark English for sharing this. Taken and modified from a post he
/// made to the cplusplus-sig mailing list, at:-
///    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767

/// \file custom_return_internal_reference.hpp
///

#ifndef __BOOST_PYTHON_RETURN_BUFFER_OBJECT__
#define __BOOST_PYTHON_RETURN_BUFFER_OBJECT__

# include <boost/python/default_call_policies.hpp>
# include <boost/python/return_internal_reference.hpp>
# include <boost/python/reference_existing_object.hpp>
# include <boost/python/instance_holder.hpp>
# include <boost/python/to_python_indirect.hpp>
# include <boost/python/to_python_value.hpp>

# include <structmember.h>   ///< for T_OBJECT

# include <boost/mpl/if.hpp>
# include <boost/mpl/assert.hpp>
# include <boost/type_traits/is_same.hpp>

#include "boost_helpers/buffer_pointer_converter.hpp"

namespace boost { namespace python { namespace detail {

        // Register buffer_pytype_ptr
        //
        // Attempt to register each used buffer object, on module import, so 
        // we don't need to make a new HolderGenerator each time.
        template <class T>
        struct register_buffer_pytype_ptr
        {
            register_buffer_pytype_ptr(T* p)
            {
                printf("register_self with type object: %s\n", typeid(p).name());
                const_cast<converter::registration &>(
                    converter::registry::lookup(boost::python::type_id<T>())
                    )
                .m_class_object = &buffer<T>::type_object;
            }
        };


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
                typedef objects::pointer_holder<T*, T> holder_t;
                // Jump into conversion lookup mechanism
                typedef T argument_type;
    # if BOOST_WORKAROUND(__GNUC__, < 3)
                typedef converter::registered<argument_type> r;
                // suppresses an ICE, somehow
                (void)r::converters;
    # endif 
                register_buffer_pytype_ptr<T> x(p);
                return converter::registered<argument_type>::converters.to_python(
                                  const_cast<T*>(p) );
            }

        };

        // For testing different holder_types...
        struct make_buffer_holder
        {
            typedef instance_holder* result_t;

            template <class T>
            static result_t execute(T* p)
            {
                return objects::pointer_holder<T*, T>(p);
            }
        };

        ////////////////////////////////////////////////////////////////////////////
        /// Return value converter

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
  template <class ResultConverterGenerator = detail::buffer_result_converter_t,
            std::size_t owner_arg = 1,
            class BasePolicy = return_internal_reference<> >
  struct return_buffer_reference
      : BasePolicy
  {
      /// Possible to test for Read-only or read-write buffer?
      //mpl::if_< is_same<

      typedef ResultConverterGenerator result_converter;

      template<class R>
      struct apply
      {

          BOOST_MPL_ASSERT_MSG( is_pointer<R>::value, RETURN_BUFFER_REFERENCE_EXPECTS_A_POINTER_TYPE, (R));

          struct type :
              boost::python::to_python_value<
                  typename detail::value_arg<R>::type
              >
          {
              type()
              {
                  printf("apply::type\n");
                  detail::buffer_pointee(R());
              }
          };
      };
  };

  // In place of a typedef template
  /// Call policy to create internal references to registered types
  //template <class BasePolicy = default_call_policies>
  //struct return_buffer_reference
  //    : detail::return_buffer_reference_impl< reference_existing_buffer
  //                                          , BasePolicy > { };


}  } // End boost::python namespace


#endif
