/// get_object_id.hpp
///
/// Utilities for working with type IDs, in both C++ and Python land.

#ifndef __BOOST_PYTHON_GET_OBJECT_ID__
#define __BOOST_PYTHON_GET_OBJECT_ID__

#include <boost/python/detail/wrap_python.hpp>

// Use the same macros as used in builtin_id(), which internally uses
//  PyLong_FromVoidPtr(), to define the return type.
#if SIZEOF_VOID_P <= SIZEOF_LONG
#   define PYOBJECT_ID_TYPE long
#else
#   define PYOBJECT_ID_TYPE unsigned PY_LONG_LONG
#endif

// Typedef alias for PYOBJECT_ID_TYPE
typedef PYOBJECT_ID_TYPE object_id_type;

namespace boost { namespace python {
    // forward declaration of object
    class object;

    // Get the internal ID of a PyObject, as returned by id(obj), in Python
    object_id_type get_object_id(boost::python::object& obj);

    // Get the last component of a fully-qualified C++ type id.
    // @param T - type of which to get the ID.
    template <class T>
    const char* unqualify_id(bool cut_template = false);

}   }

inline object_id_type boost::python::get_object_id(boost::python::object& obj)
{
    return (object_id_type) ( (void*)(obj.ptr()) );
}

template <class T>
inline const char * boost::python::unqualify_id(bool cut_template)
{
    // Get the fully qualified name.
    const char* full_name = type_id<T>().name();
    size_t idx = strlen(full_name) - 1;
    size_t i = 0, last_sep = 0;
    register char c;
    // Loop through the name, until we reach a '<' char, or the end.
    while (i < idx)
    {
        c = full_name[i];
        if (c == ':')
        {
            i++; // skip the next ':'
            last_sep = i + 1; // record the start position of next namespace
        }
        else if (c == '<' && cut_template == true)
        {
            // delete the template stuff
            char * name = new char[i+1];
            strncpy(name, full_name, i);
            full_name = const_cast<const char*>(name);
            break;
        }
        i++;
    }

    // Slice full_name to be an unqualified name:-
    return &full_name[last_sep];
}

#endif