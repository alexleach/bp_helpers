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
        template <class T,
                  class DerivedPolicies 
                      = detail::final_iostream_derived_policies<T> >
        struct register_iostream_pytype
            : register_istream_pytype<T, DerivedPolicies>,
            register_ostream_pytype<T, DerivedPolicies>
        {
            typedef iostream<T, DerivedPolicies>      stream_t;
            typedef typename stream_t::object_type object_type;

            register_iostream_pytype(void)
                : register_istream_pytype<T, DerivedPolicies>(),
                register_ostream_pytype<T, DerivedPolicies>()
            {

                printf("  Looking up IOStream: %s \n", typeid(T).name());
                /*
                converter::registration& iostream_converter
                    = const_cast<converter::registration&>(
                        converter::registry::lookup(type_id<stream_t>()) );

                if (iostream_converter.m_class_object == 0)
                    iostream_converter.m_class_object = &DerivedPolicies::m_type;
                */

                // Register istream as a base class.
                //typedef typename stream_t::istream_base istream_base;
                //objects::register_dynamic_id<istream_base>();
                //objects::register_conversion<stream_t, istream_base>(false);

                // Register ostream as a base class.
                //typedef typename stream_t::ostream_base ostream_base;
                //objects::register_dynamic_id<ostream_base>();
                //objects::register_conversion<stream_t, ostream_base>(false);
            }
        };
    }

    // Make a PyTypeObject that follows the C++ STL istream protocol.
    //
    // @param Pointee - C++ class to expose to Python
    template <class Pointee,
              class DerivedPolicies 
                  = detail::final_iostream_derived_policies<Pointee> >
    struct make_iostream_type_object
    {
        typedef typename
            boost::python::iostream<Pointee, DerivedPolicies> stream_t;

        make_iostream_type_object(const char * name = NULL)
            : m_stream_wrapper()
        { 
            printf("make_iostream_type object<%s>(%s)\n",
                type_id<Pointee>().name(), name);

            objects::register_iostream_pytype<Pointee, DerivedPolicies> reg;

            add_type_to_module<Pointee>(&stream_t::m_type, name);

            // need to get into the class scope!
            //scope ob_scope = object(module.attr(stream_t::m_type.tp_name));

            //printf("class scope name : %s\n",
            //    PyString_AsString(object(ob_scope.attr("__name__")).ptr());
            // then add the functions
            //m_stream_wrapper.extension_def();


            //stream_t::m_type.tp_name = type_id<Pointee>().name();

            //typename iostream_t::istream_base istream_base
            //    = make_istream_type_object<Pointee, DerivedPolicies>();

            //typename iostream_t::ostream_base ostream_base
            //    = make_ostream_type_object<Pointee, DerivedPolicies>();
        }

        stream_t& operator()(void)
        {
            printf("make_iostream_type_object::operator()\n");
            //stream_t iostream_holder;


            // The constructor, which adds method definitions, needs to be in
            // right scope to work.. Best way?
            return m_stream_wrapper;
        }

    private:
        stream_t m_stream_wrapper;
    };

}  } // End boost::python namespace


#endif
