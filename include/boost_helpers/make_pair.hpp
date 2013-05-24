#ifndef __BOOST_PYTHON_MAKE_PAIR__
#define __BOOST_PYTHON_MAKE_PAIR__

#include <boost/python/to_python_converter.hpp>
#include <boost/python/tuple.hpp>

namespace boost { namespace python { 

    template <class T1, class T2>
    struct make_pair
        : to_python_converter< std::pair<T1, T2>, make_pair<T1, T2> >
    {
        static PyObject* convert(const std::pair<T1, T2>& pair)
        {
            return incref(make_tuple(pair.first, pair.second).ptr());
        }
    };

}  }

#endif