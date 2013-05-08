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
    const char* unqualify_id(void);

}   }

inline object_id_type boost::python::get_object_id(boost::python::object& obj)
{
    return (object_id_type) ( (void*)(obj.ptr()) );
}

template <class T>
inline const char * boost::python::unqualify_id(void)
{
    // Get the fully qualified name.
    const char* full_name = type_id<T>().name();
    size_t idx = strlen(full_name) - 1;
    // Loop backwards through the name, until we reach a ':'
    while ((full_name[idx-1] != ':') && (idx > 0))
        idx--;
    // Slice full_name to be an unqualified name:-
    return &full_name[idx];
}

#endif