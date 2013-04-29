/// Thanks to Mark English for sharing this. Taken and modified from a post he
/// made to the cplusplus-sig mailing list, at:-
///    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767

#include "boost_helpers/return_buffer_object.hpp"

// New/Dealloc added to support weakref
template<class Pointee>
PyObject * boost::python::Buffer_New(PyTypeObject * type,
                                     PyObject     * args,
                                     PyObject     * kwds)
{
  printf("In Buffer_New\n");
  PyObject * obj = PyType_GenericNew(type, args, kwds);
  if (obj != NULL && !PyErr_Occurred()) // If created object ok
  {
    Pointee * wrapped = reinterpret_cast<Pointee*>(obj);
    wrapped->p_weakrefs = NULL;
  }
  return obj;
}

template <class Pointee>
void boost::python::Buffer_Dealloc(Pointee * obj)
{
  printf("In Buffer_Dealloc\n");
  // Allocate temporaries if needed, but do not begin destruction just yet
  if (obj->p_weakrefs != NULL)
  {
    PyObject_ClearWeakRefs(reinterpret_cast<PyObject *>(obj));
  }
  obj->ob_type->tp_free(obj);
}

// Currently not used. Could potentially use this to do registrations for us..
template <class Pointee>
PyTypeObject boost::python::make_buffer_type_object(void)
{ 
    printf("typedef'ing buffer type object\n");
    typedef typename 
        buffer<Pointee>::instance_type instance_t; 

    typedef typename 
        buffer<Pointee>::type_object object_t; 
    printf("making buffer type object\n");

    object_t::tp_name = const_cast<char*>(type_id<Pointee*>().name());

    if (PyType_Ready(&buffer<Pointee>::type_object) < 0)
        boost::python::throw_error_already_set();
    Py_INCREF(&buffer<Pointee>::type_object);
    return buffer<Pointee>::type_object;
}

