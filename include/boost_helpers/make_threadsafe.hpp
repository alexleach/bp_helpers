#ifndef __BOOST_PYTHON_MAKE_THREADSAFE__
#define __BOOST_PYTHON_MAKE_THREADSAFE__

// File: boost_helpers/make_threadsafe.hpp
//
// Utility classes to give threading control to Blast+ libraries.
//
// Each class gives RAII semantics to control Python threading functions, which
// fall into a small number of categories:-
//
//      1 - To initialise threading, use boost::python::threads()
//      2 - To perform blocking I/O operations, use boost::python::threadstate()
//      3 - When doing callbacks into Python code from C++ threads, use
//           boost::python::gilstate()
//      4 - When performing thread-safe functions, that acquire the global mutex
//          lock, also use boost::python::gilstate()
//
// \sa {boost::python::threads,
//      boost::python::threadstate, 
//      boost::python::gilstate}
// 

#include <ceval.h>

namespace boost { namespace python {

    // From http://docs.python.org/release/2.7.4/c-api/init.html :-
    // Initialize and acquire the global interpreter lock. It should be called 
    // in the main thread before creating a second thread or engaging in any 
    // other thread operations such as PyEval_ReleaseLock() or 
    // PyEval_ReleaseThread(tstate). It is not needed before calling 
    // boost::python::threadstate().
    class init_threads
    {
    public:
        init_threads(void)
        {
            PyEval_InitThreads();
        }

        ~init_threads() {}

    };

    // threadstate
    //
    // Initialisation releases the global interpreter lock (if it has been 
    // created) and resets Python's thread state to NULL, saving the old state.
    // The destructor makes Python re-acquire the GIL (if it has been created)
    // and resets Python's threadstate to the original value.
    class threadstate
    {
    public:
        threadstate(void)
        {
            // From [1]
            //Py_UNBLOCK_THREADS; // Equivalent to Py_BEGIN_ALLOW_THREADS and:-
            thread_state = PyEval_SaveThread();
        }

        ~threadstate()
        {
            // From [1]
            //Py_BLOCK_THREADS; // Equivalent to Py_END_ALLOW_THREADS and:-
            PyEval_RestoreThread(thread_state);
        }

    private:
        // from [1]
        PyThreadState * thread_state; //< Used by Py_..BLOCK_THREADS macros
    };

    // gilstate
    //
    // When C++ functions lock the system's threading mutex (e.g. by calling
    // pthread_mutex_lock on Linux), Python's Global Interpreter Lock (GIL) will
    // usually block the call. This class will release the Global Interpreter
    // Lock and allow C++ functions to use the system's threading API.
    class gilstate
    {
    public:
        gilstate(void)
        {
            // From [2]
            m_threadstate = threadstate();
            gil_state = PyGILState_Ensure();
        }

        ~gilstate()
        {
            // From [2] 
            PyGILState_Release(gil_state);
            //delete m_threadstate;
        }

    private:
        // from [2]
        PyGILState_STATE gil_state;
        threadstate m_threadstate;
    };

}   }


/// [1] - http://wiki.python.org/moin/boost.python/HowTo#Multithreading_Support_for_my_function
/// [2] - http://docs.python.org/2.7/c-api/init.html

#endif