#ifndef __BOOST_PYTHON_CONVERTER_HELPERS_INCLUDED__
#define __BOOST_PYTHON_CONVERTER_HELPERS_INCLUDED__

/*
 * This file contains methods that convert a specific Python type to a C type
 * expected by components of NCBI's Blast+ API: 'const char* const*'
 *
 * These converter functions allocate memory for strings on the heap, and saves
 * their pointers in pargv, which must be big enough to store all pointers to
 * those strings plus one, for a NULL pointer.
 *
 * So after calling these functions, you must free each pointer in the returned
 * const char** array.
 */

namespace boost { namespace python {
    class dict;
    class list;
    class tuple;
}   }

namespace pyblast {

    /// ConvertPyDictToCharArrayPointer
    ///
    /// Convert Python dict into a char* array.
    /// @param dict  - Python dictionary. [in]
    /// @param penvp - Array of strings containing "key=value" strings. [out]
    void ConvertPyDictToCharArrayPointer(
        const boost::python::dict& dict, const char** penvp);

    /// ConvertPyListToCharArrayPointer
    /// Convert Python list into a char* array.
    /// @param list  - Python list. [in]
    /// @param penvp - Array of strings containing argument strings. [out]
    void ConvertPyListToCharArrayPointer(
        int argc, const boost::python::list& list, const char** pargv);

    /// ConvertTupleToCharArrayPointer
    /// Convert Python tuple into a char* array.
    /// @param list  - Python tuple. [in]
    /// @param penvp - Array of strings containing argument strings. [out]
    void ConvertTupleToCharArrayPointer(
        int argc, const boost::python::tuple& tuple, const char** pargv);

    /// DelCharArrayPointer
    ///
    /// Utility function to free the elements malloc'd by the above converter
    ///  functions.
    /// @param p_strs - An output variable from one of the above functions.
    void DelCharArrayPointer(const char** p_strs);

}

#endif