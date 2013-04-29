/// get_object_id.hpp
///
/// Get the internal ID of a boost::python::object, as returned by id(obj) from
///  Python.

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
typedef PYOBJECT_ID_TYPE PyObj_Id_t;

namespace boost { namespace python {
    // forward declaration of object
    class object;

    PyObj_Id_t get_object_id(boost::python::object& ptr);

}   }

inline PyObj_Id_t boost::python::get_object_id(boost::python::object& ptr)
{
    return (PyObj_Id_t)((void*)ptr.ptr());
}

#endif