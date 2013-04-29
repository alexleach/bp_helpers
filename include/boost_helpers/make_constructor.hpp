#ifndef __BOOST_PYTHON_MAKE_CONSTRUCTOR_INCLUDED__
#define __BOOST_PYTHON_MAKE_CONSTRUCTOR_INCLUDED__

#include <boost/python/dict.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>

#include <boost/python/raw_function.hpp>

namespace boost{ namespace python {

/// pyblast::raw_method_dispatcher
///
/// Helper class for exposing class methods accepting *args and **kwargs.
template <class F>
class raw_constructor_dispatcher
{
public:
    raw_constructor_dispatcher(F f)
        : f(boost::python::make_constructor(f)) {}

    PyObject* operator()(PyObject* args, PyObject* kwds)
    {
        boost::python::detail::borrowed_reference_t* args_ref
            = boost::python::detail::borrowed_reference(args);
        boost::python::object args_obj(args_ref);
        return boost::python::incref(
            boost::python::object(
                f(
                    boost::python::object(args_obj[0]),
                    boost::python::tuple(
                        args_obj.slice(1, boost::python::len(args_obj)) ),
                    kwds
                        ? boost::python::dict(
                            boost::python::detail::borrowed_reference(kwds))
                        : boost::python::dict()
                )
            ).ptr()
        )
        ;
    }
private:
    boost::python::object f;
};


template <class F>
boost::python::object raw_constructor(F f, std::size_t min_args = 0)
{
    return boost::python::detail::make_raw_function(
        boost::python::objects::py_function(
            raw_constructor_dispatcher<F>(f), 
            boost::mpl::vector2<void, boost::python::object>(),
            min_args + 1,
            (std::numeric_limits<unsigned>::max)()
            ) );

}


/* If can get raw method's working, that'd be sweet.. */

template <class F>
class raw_method_dispatcher
{
public:
    raw_method_dispatcher(F f)
        : f(boost::python::make_constructor(f)) {}

    PyObject* operator()(PyObject* args, PyObject* kwds)
    {
        boost::python::detail::borrowed_reference_t* args_ref
            = boost::python::detail::borrowed_reference(args);
        boost::python::object args_obj(args_ref);
        return boost::python::incref(
            boost::python::object(
                f(
                    boost::python::object(args_obj[0]),
                    boost::python::tuple(
                        args_obj.slice(1, boost::python::len(args_obj)) ),
                    kwds
                        ? boost::python::dict(
                            boost::python::detail::borrowed_reference(kwds))
                        : boost::python::dict()
                )
            ).ptr()
        )
        ;
    }
private:
    boost::python::object f;
};

template <class F>
boost::python::object raw_method(F f, std::size_t min_args = 0)
{
    return boost::python::detail::make_raw_function(
        boost::python::objects::py_function(
            raw_method_dispatcher<F>(f),
            boost::python::default_call_policies(),
            boost::mpl::vector2<void, boost::python::object>(),
            min_args + 1,
            (std::numeric_limits<unsigned>::max)()
            )
        );
}

}}

#endif