#ifndef __BOOST_PYTHON_MAKE_CALLBACK__
#define __BOOST_PYTHON_MAKE_CALLBACK__

#include <boost/python/call.hpp>

#include "boost_helpers/make_threadsafe.hpp"

namespace boost { namespace python { 

    class make_callback_base
    {
    public:
        make_callback_base(PyObject* callback) : m_callback(callback) {}
    protected:
        PyObject*   m_callback;
    };

#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES

    // When Variadic templates are supported (C++0x feature):-
    template <typename ReturnType, typename ... ArgTypes>
    class make_callback
        : public make_callback_base
    {
    public:
        make_callback(PyObject* callback) : make_callback_base(callback) {}

        ReturnType operator() (ArgTypes ... args)
        {
            return boost::python::call<ReturnType>(this->m_callback, args...);
        }
    };

#else
// Without variadic templates, need to create multiple make_callback templates.
// There must be a better way...


    // Tried to implement this generic class, but couldn't get it to work.. :(
    /*
    template <typename ReturnType, typename ArgType = void>
    class make_callback
        : public make_callback_base
    {
    public:
        make_callback(PyObject* callback) : make_callback_base(callback) {}

        ReturnType operator() (void)
        {
            threadstate safe;
            return boost::python::call<ReturnType>(this->m_callback);
        }

        ReturnType operator() (ArgType arg)
        {
            threadstate safe;
            return boost::python::call<ReturnType>(this->m_callback, arg);
        }

        // When called with more than two arguments, can use varidic function
        // in order to do so.
        ReturnType operator() (ArgType arg, va_list * args)
        {
            threadstate safe;
            return boost::python::call<ReturnType>(this->m_callback, arg, args);
        }

        typedef ReturnType  (make_callback<ReturnType, ArgType>::* CallBackType)
                                (ArgType arg, ...);

        // The ellipsis syntax does not work with overloaded function
        // operators, however, so needs to be explicitly called.
        ReturnType call_va_args(ArgType arg, ...)
        {
            va_list ap;
            va_start(ap, arg);
            ReturnType ret = operator()(&ap);
            va_end(ap);
            return ret;
        }

        typedef typename
            make_callback<ReturnType, ArgType>::CallBackType callback_t;

        callback_t get_callback(void)
        {
            callback_t callback = (this->*call_va_args);
            return callback;
        }

    };
    */

    // callback that accepts no arguments.
    template <typename ReturnType>
    class make_callback0
        : public make_callback_base
    {
    public:
        make_callback0(PyObject* callback) : make_callback_base(callback) {}

        ReturnType operator() (void)
        {
            return boost::python::call<ReturnType>(this->m_callback);
        }

    };

    // callback that accepts one argument.
    template <typename ReturnType, typename ArgType>
    class make_callback1
        : public make_callback_base
    {
    public:
        make_callback1(PyObject* callback) : make_callback_base(callback) {}

        ReturnType operator() (ArgType arg)
        {
            return boost::python::call<ReturnType>(this->m_callback, arg);
        }
    };

    // callback that accepts two arguments.
    template <typename ReturnType, typename Arg1Type, typename Arg2Type>
    class make_callback2
        : public make_callback_base
    {
    public:
        make_callback2(PyObject* callback) : make_callback_base(callback) {}

        ReturnType operator() (Arg1Type arg1, Arg2Type arg2)
        {
            return boost::python::call<ReturnType>(this->m_callback, arg1, arg2);
        }
    };


#endif

}  }

#endif
