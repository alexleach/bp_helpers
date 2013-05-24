#include "boost_helpers/make_pair.hpp"
#include "boost/python/def.hpp"
#include "boost/python/module.hpp"

std::pair<std::string, std::string> make_a_pair(void)
{
    std::pair<std::string, std::string> p("foo", "bar");
    return p;
}

BOOST_PYTHON_MODULE(test_pair)
{
    boost::python::make_pair<std::string, std::string>();
    boost::python::def("make_a_pair", &make_a_pair);
}