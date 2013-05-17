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

    namespace objects {

        // register_istream_pytype
        //
        // Attempt to register each used buffer object, on module import, so 
        // we don't need to make a new HolderGenerator each time.
        template <class T,
              class DerivedPolicies 
                  = detail::final_istream_derived_policies<T> >
        struct register_istream_pytype
            : virtual detail::register_iostream_base_pytype<T, DerivedPolicies>
        {
            typedef istream<T, DerivedPolicies>              stream_t;
            typedef typename stream_t::object_type        object_type;

            register_istream_pytype(void)
                : detail::register_iostream_base_pytype<T, DerivedPolicies>()
            {
                printf("  Looking up IStream %s \n", typeid(stream_t).name());

                // Insert type T into the registry.
                converter::registration& istream_converter
                    = const_cast<converter::registration&>(
                        converter::registry::lookup(type_id<stream_t>()) );

                if (istream_converter.m_class_object == 0)
                {
                    printf("  Registering IStream %s \n", typeid(stream_t).name());
                    // Put the new PyTypeObject in to the registry.
                    istream_converter.m_class_object = &stream_t::m_type;

                    // register a to_python converter
                    /*
                    converter::registry::insert(
                        (converter::to_python_function_t)&stream_t::convert,
                        type_id<T>(),
                        &stream_t::get_pytype );
                    */

                    // Register base class conversions.
                    //register_conversion<stream_t,
                    //                    typename stream_t::base_type>(false);
                }
            }
        };
    }

    // Make a PyTypeObject that follows the C++ STL istream protocol.
    //
    // @param Pointee - C++ class to expose to Python
    template <class Pointee,
              class DerivedPolicies 
                  = detail::final_istream_derived_policies<Pointee> >
    struct make_istream_type_object
    { 
        typedef typename
            boost::python::istream<Pointee, DerivedPolicies> stream_t;

        make_istream_type_object(const char * name = NULL)
            : m_stream_wrapper()
        {
            printf("make_istream_type_object\n");
            add_type_to_module<Pointee>(&stream_t::m_type, name);
        }

        stream_t& operator()(void)
        {
            printf("make_istream_type_object::operator()\n");
            //stream_t::m_type.tp_name = type_id<Pointee>().name();

            objects::register_istream_pytype<Pointee, DerivedPolicies> reg;
            return m_stream_wrapper;
        }

    private:
        stream_t m_stream_wrapper;
    };

}  } // End boost::python namespace

#endif
