#include "boost_helpers/make_list.hpp"

#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python/module.hpp>

typedef std::list<int>          int_list;
typedef std::list<std::string>  str_list;

BOOST_PYTHON_MODULE(test_make_list)
{
    boost::python::class_<int_list>
    ("IntList", "Wrapps a std::list of int's.")
        .def(boost::python::list_indexing_suite< std::list<int> >())
    ;

    boost::python::class_<str_list>
    ("StrList", "Wrapps a std::list of std::string's.")
        .def(boost::python::list_indexing_suite< std::list<std::string> >())
    ;
}