#ifndef __BOOST_PYTHON_RETURN_IOSTREAM_INCLUDED__
#define __BOOST_PYTHON_RETURN_IOSTREAM_INCLUDED__

/// \file return_iostream.hpp
///

#include "return_istream.hpp"
#include "return_ostream.hpp"
#include "iostream.hpp"

namespace boost { namespace python {

    namespace objects {
        // register_iostream_pytype
        //
        // Add iostream<T> to the Boost Python Registry. This should be done on 
        // module  import, so we don't need to make a new HolderGenerator each
        // time. This puts the PyTypeObjects for iostream_base, istream, ostream
        // and finally iostream into the registry, assuming they are not there
        // already. It also registers to_python converters at the same time, and
        // registers the full type hierarchy whilst it's at it.
        template <class T>
        struct register_iostream_pytype
            : register_istream_pytype<T>, register_ostream_pytype<T>
        {
            typedef iostream<T> stream_t;

            register_iostream_pytype(void)
                : register_istream_pytype<T>(), register_ostream_pytype<T>()
            {
                printf("  Looking up IOStream: %s \n", typeid(T).name());
                // Insert type_id into registry
                converter::registration& iostream_converter
                    = const_cast<converter::registration&>(
                        converter::registry::lookup(type_id<T>()) );

                if (iostream_converter.m_class_object == 0)
                {
                    printf("  Registering IOStream: %s \n", typeid(T).name());
                    // Put the new PyTypeObject in to the registry.
                    iostream_converter.m_class_object = &stream_t::m_type;

                    // Insert iostream to_python converter into registry
                    converter::registry::insert(
                        (converter::to_python_function_t)&stream_t::convert,
                        boost::python::type_id<T>(),
                        &stream_t::get_pytype );
                }
                else
                {
                    iostream_converter.m_class_object = &stream_t::m_type;
                    iostream_converter.m_to_python
                        = (converter::to_python_function_t)&stream_t::convert;
                }
                // Register istream as a base class.
                register_istream_pytype<T>();
                typedef typename stream_t::istream_base istream_base;
                objects::register_dynamic_id<istream_base>();
                objects::register_conversion<stream_t, istream_base>(false);

                // Register ostream as a base class.
                register_ostream_pytype<T>();
                typedef typename stream_t::ostream_base ostream_base;
                objects::register_dynamic_id<ostream_base>();
                objects::register_conversion<stream_t, ostream_base>(false);
            }
        };
    }

    // Make a PyTypeObject that follows the C++ STL istream protocol.
    //
    // @param Pointee - C++ class to expose to Python
    template <class Pointee>
    iostream<Pointee> make_iostream_type_object(void)
    { 
        printf("making Python version of iostream type object %s\n",
            type_id<Pointee>().name());
        typedef typename boost::python::iostream<Pointee> iostream_t;

        iostream_t::m_type.tp_name
            = const_cast<char*>(type_id<Pointee>().name());
        iostream_t::m_type.tp_base = iostream_t::base_type::m_type.tp_base;

        if (PyType_Ready(&iostream_t::m_type) < 0)
            boost::python::throw_error_already_set();

        Py_INCREF(&iostream_t::m_type);
        objects::register_iostream_pytype<Pointee>();
        return iostream_t();
    }

}  } // End boost::python namespace


#endif
