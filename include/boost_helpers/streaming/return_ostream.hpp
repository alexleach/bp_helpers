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
        template <class T>
        struct register_ostream_pytype
            : detail::register_iostream_base_pytype<T,
                detail::final_ostream_derived_policies<T> >
        {
            typedef ostream<T> stream_t;

            register_ostream_pytype(void)
                : detail::register_iostream_base_pytype<T,
                    detail::final_ostream_derived_policies<T> >()
            {
                printf("  Looking up OStream: %s \n", typeid(T).name() );
                // Insert type_id into registry
                converter::registration& ostream_converter
                    = const_cast<converter::registration&>(
                        converter::registry::lookup(type_id<T>()) );

                if (ostream_converter.m_class_object == 0)
                {
                    printf("  Registering OStream: %s \n", typeid(T).name() );
                    // Put the new PyTypeObject in to the registry.
                    ostream_converter.m_class_object = &stream_t::m_type;

                    // Insert to_python converter into registry
                    converter::registry::insert(
                        (converter::to_python_function_t)&stream_t::convert,
                        boost::python::type_id<T>(),
                        &stream_t::get_pytype );

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
    template <class Pointee>
    ostream<Pointee> make_ostream_type_object(void)
    { 
        typedef typename boost::python::ostream<Pointee> ostream_t;
        printf("making Python version of ostream-like object: %s\n", ostream_t::m_type.tp_name);

        ostream_t::m_type.tp_name
            = const_cast<char*>(type_id<Pointee>().name());

        if (PyType_Ready(&ostream_t::m_type) < 0)
            boost::python::throw_error_already_set();

        Py_INCREF(&ostream_t::m_type);
        objects::register_ostream_pytype<Pointee>();
        return ostream_t();
    }

}  } // End boost::python namespace


#endif
