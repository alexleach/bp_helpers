/// make_submodules.hpp
//
// Preprocessor macros for defining sub-package namespaces. These are used to
// organise the Blast+ wrapper classes into a better arranged package namespace.

#include <boost/python/borrowed.hpp>
#include <boost/python/object.hpp>

// The only difference between the macros is the number of registry
// functions they can call from the same macro. If more Boost.Python 
// registration functions need to be added to a single namespace, then
// create another macro supporting that number of arguments.

//! BLAST_PYTHON_SUBMODULE
// @param NAME. 
// The name of submodule to create within the module-level namespace
// @param INIT
// Function to call, which contains Boost.Python definitions
#define BLAST_PYTHON_SUBMODULE(NAME, INIT)                          \
{                                                                   \
    std::string NAME ## _name = module_name + "." #NAME ;           \
    boost::python::object NAME(boost::python::borrowed(             \
        PyImport_AddModule(NAME ## _name.c_str() )) );              \
    module.attr(#NAME) = NAME;                                      \
    {                                                               \
        boost::python::scope NAME ## _scope = NAME ;                \
        INIT();                                                     \
    }                                                               \
}

//! BLAST_PYTHON_SUBMODULE2
// Same as above, but calls two Boost.Python registry functions.
#define BLAST_PYTHON_SUBMODULE2(NAME, INIT, INIT2)                  \
{                                                                   \
    std::string NAME ## _name = module_name + "." #NAME ;           \
    boost::python::object NAME(boost::python::borrowed(             \
        PyImport_AddModule(NAME ## _name.c_str() )) );              \
    module.attr(#NAME) = NAME;                                      \
    {                                                               \
        boost::python::scope NAME ## _scope = NAME ;                \
        INIT();                                                     \
        INIT2();                                                    \
    }                                                               \
}

//! BLAST_PYTHON_SUBMODULE3
// Same as above, but calls three Boost.Python registry functions.
#define BLAST_PYTHON_SUBMODULE3(NAME, INIT, INIT2, INIT3)           \
{                                                                   \
    std::string NAME ## _name = module_name + "." #NAME ;           \
    boost::python::object NAME(boost::python::borrowed(             \
        PyImport_AddModule(NAME ## _name.c_str() )) );              \
    module.attr(#NAME) = NAME;                                      \
    {                                                               \
        boost::python::scope NAME ## _scope = NAME ;                \
        INIT();                                                     \
        INIT2();                                                    \
        INIT3();                                                    \
    }                                                               \
}


//! BLAST_PYTHON_SUBMODULE4
// Same as above, but calls four Boost.Python registry functions.
#define BLAST_PYTHON_SUBMODULE4(NAME, INIT, INIT2, INIT3, INIT4)    \
{                                                                   \
    std::string NAME ## _name = module_name + "." #NAME ;           \
    boost::python::object NAME(boost::python::borrowed(             \
        PyImport_AddModule(NAME ## _name.c_str() )) );              \
    module.attr(#NAME) = NAME;                                      \
    {                                                               \
        boost::python::scope NAME ## _scope = NAME ;                \
        INIT();                                                     \
        INIT2();                                                    \
        INIT3();                                                    \
        INIT4();                                                    \
    }                                                               \
}

//! BLAST_PYTHON_SUBMODULE5
// Same as above, but calls five Boost.Python registry functions.
#define BLAST_PYTHON_SUBMODULE5(NAME, INIT, INIT2, INIT3, INIT4,    \
                                INIT5)                              \
{                                                                   \
    std::string NAME ## _name = module_name + "." #NAME ;           \
    boost::python::object NAME(boost::python::borrowed(             \
        PyImport_AddModule(NAME ## _name.c_str() )) );              \
    module.attr(#NAME) = NAME;                                      \
    {                                                               \
        boost::python::scope NAME ## _scope = NAME ;                \
        INIT();                                                     \
        INIT2();                                                    \
        INIT3();                                                    \
        INIT4();                                                    \
        INIT5();                                                    \
    }                                                               \
}
