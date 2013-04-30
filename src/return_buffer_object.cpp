#ifndef __BOOST_PYTHON_IOSTREAM_BUFFER__
#define __BOOST_PYTHON_IOSTREAM_BUFFER__

/// Thanks to Mark English for sharing this. Taken and modified from a post he
/// made to the cplusplus-sig mailing list, at:-
///    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767

#include "boost_helpers/return_buffer_object.hpp"

//#include <structmember.h>   ///< for T_OBJECT

// Currently not used. Could potentially use this to do registrations for us..
/*
template <class Pointee>
PyTypeObject boost::python::make_buffer_type_object(void)
{ 
    printf("making Python version of IO stream-like object\n");
    typedef typename boost::python::buffer<Pointee>   p_t;
    typedef typename p_t::value_type           instance_t; 

    buffer<Pointee>::type_object.tp_name
        = const_cast<char*>(type_id<Pointee*>().name());
    if (PyType_Ready(&buffer<Pointee>::type_object) < 0)
        boost::python::throw_error_already_set();
    boost::python::register_buffer_pytype<Pointee> x;
    //Py_INCREF(&buffer<Pointee>::type_object);
    return buffer<Pointee>::type_object;
}
*/

#endif
