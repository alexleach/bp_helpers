#ifndef __BOOST_PYTHON_RETURN_OSTREAM_INCLUDED__
#define __BOOST_PYTHON_RETURN_OSTREAM_INCLUDED__

/// \file return_ostream.hpp
///

#include "return_iostream_base.hpp"
#include "ostream.hpp"

namespace boost { namespace python {

    namespace objects {

        // register_ostream_pytype
        //
        // Register a C++ class that conforms to the C++ STL ostream interface,
        // on module import.
        // This is a multi-step process:-
        //  0. Register the iostream_base class.
        //     \sa `detail::register_iostream_base_pytype`
        //  1. Add the type_id of the wrapped class to the Boost Python registry.
        //  2. Add a to_python converter.
        //  3. Register safe pointer conversions between the wrapped class and
        //     its bases.
        template <class T,
                  class DerivedPolicies
                      = detail::final_ostream_derived_policies<T> >
        struct register_ostream_pytype
            : virtual detail::register_iostream_base_pytype<T, DerivedPolicies>
        {
            typedef ostream<T, DerivedPolicies>              stream_t;
            typedef typename stream_t::object_type        object_type;

            register_ostream_pytype(void)
                : detail::register_iostream_base_pytype<T, DerivedPolicies>()
            {
                printf("  Looking up OStream: %s \n", typeid(T).name() );

                converter::registration& ostream_converter
                    = const_cast<converter::registration&>(
                        converter::registry::lookup(type_id<stream_t>()) );

                if (ostream_converter.m_class_object == 0)
                {
                    printf("  Registering OStream: %s \n", typeid(T).name() );
                    // Put the new PyTypeObject in to the registry.
                    ostream_converter.m_class_object = &stream_t::m_type;

                    // Register base class pointer compatibility.
                    register_conversion<stream_t,
                                        typename stream_t::base_type>(false);
                }
            }
        };
    }   // -- End objects namespace -- //

    // Make a PyTypeObject that follows the ostream protocol.
    //
    // @param Pointee - C++ class to expose to Python
    template <class Pointee,
              class DerivedPolicies 
                  = detail::final_ostream_derived_policies<Pointee> >
    struct make_ostream_type_object
    { 
        typedef typename
            boost::python::ostream<Pointee, DerivedPolicies> stream_t;

        make_ostream_type_object(const char* name = NULL)
            : m_stream_wrapper()
        {
            printf("make_ostream_type_object<%s>()\n", stream_t::m_type.tp_name);
            add_type_to_module<Pointee>(&stream_t::m_type, name);
            //ostream_t::m_type.tp_name = type_id<Pointee>().name();
        }

        stream_t& operator()(void)
        {
            printf("make_ostream_type_object::operator()\n");
            objects::register_ostream_pytype<Pointee, DerivedPolicies> reg;
            return m_stream_wrapper;
        }
    private:
        stream_t m_stream_wrapper;
    };

}  } // End boost::python namespace


#endif
