#ifndef __BOOST_PYTHON_IOS_BASE_INCLUDED__
#define __BOOST_PYTHON_IOS_BASE_INCLUDED__

//#include <boost/python/def_visitor.hpp>
//#include <boost/python/object.hpp>
//#include <boost/python/register_ptr_to_python.hpp>
#include <structmember.h>   ///< for T_OBJECT
#include <iosfwd>

//#include <boost/python/extract.hpp>
//#include <boost/python/to_python_converter.hpp>
#include <boost/mpl/always.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/if.hpp>
#include <boost/python/converter/rvalue_from_python_data.hpp>
#include <boost/python/def.hpp>
#include <boost/python/detail/unwrap_wrapper.hpp>
#include <boost/python/borrowed.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/object.hpp>
#include <boost/python/refcount.hpp>

#include "boost_helpers/make_threadsafe.hpp"
#include "new_buffer.hpp"

namespace boost { namespace python { namespace converter {

    /////////////////////////////////////////////////////////////////////////
    // ResultConverter for a C++ IO stream

    // template class iostream_base

    /**
     * A Python object instance holder, for C++ objects with iostream-like
     * interfaces.
     *
     * Aims to follow indexing_suite semantics for exposing a C++ stream as a
     * PyObject. This is achieved by inheriting from def_visitor, and defining
     * a 'visit' instance method. Subclasses then define an 'extension_def'
     * method for attaching additional Python methods to the class.
     *
     * @param DerivedPolicies
     *
     * To register derived class methods in the Python object, this base class
     * is passed a `DerivedPolicies` class, which is used to call methods on
     * derived classes. This method for registering classes was originally taken
     * from boost's indexing_suite headers. See them and the other stream
     * headers for examples on how to initialise this base class.
    */
    template <
        class Pointee,
        class DerivedPolicies
    >
    class iostream_base
        : public boost::python::api::object
    {
    public:

        //@{
        /// Python object Typedefs
        typedef struct BoostPythonBuffer
        {
          PyObject_HEAD
          void      * buf;
          PyObject  * m_weakrefs;
          PyObject  * m_dict;    //< Instance method dictionary
          Pointee   * m_stream;  //< Pointer to I/O stream
          bool        m_new;     //< True if instantiated from Python
          Py_buffer * m_view;    //< view to maybe release
        } value_type;

        typedef PyTypeObject                                  object_type;
        //@}

        /// 
        typedef iostream_base<Pointee, DerivedPolicies>    container_type;

        //@{
        /// C++ Standard typedefs
        //@{
        /// Pointee should have inherited these from \ref std::basic_ios
        typedef typename Pointee::char_type      char_type;
        typedef typename Pointee::traits_type  traits_type;
        typedef typename Pointee::int_type        int_type;
        typedef typename Pointee::pos_type        pos_type;
        typedef typename Pointee::off_type        off_type;
        //@}

        //@{
        /// Pointee should have inherited these from \ref std::ios_base
        typedef typename Pointee::failure          failure;
        typedef typename Pointee::fmtflags        fmtflags;
        typedef typename Pointee::iostate          iostate;
        typedef typename Pointee::openmode        openmode;
        typedef typename Pointee::seekdir          seekdir;
        //@}
        //@}

        //@{
        /// \brief Static members
        /// Objects providing support for Python iostream_base protocol:-
        static object_type         m_type;

    protected:
        /// Could be, or should be, defined in derived classes.
        static PyMemberDef       m_members[];
        /// PyBufferProcs for the old-style buffer protocol:-
        static PyBufferProcs        m_stream; 
        /// PyMappingMethods for the mapping protocol:-
        static PyMappingMethods    m_mapping;
        /// PySequenceMethods for the sequence protocol:-
        static PySequenceMethods  m_sequence;
        /// Instance variable attribute holder:-
        static PyGetSetDef      ios_getsets[];
        //@}

    public:
        //@{ ctor overloads, same as in `bp::str`.
        iostream_base(void);
        //iostream_base(const char* s);
        //iostream_base(char const* start, char const* finish);
        //iostream_base(char const* start, std::size_t length);

        // Templated constructor
        template <class T>
        explicit iostream_base(T const& other) ;
        //@}

        // dtor
        ~iostream_base() { }

        //@{
        ///  Boost Python converter functions
        static void     construct(PyObject* self,
                                  rvalue_from_python_stage1_data* data);
        static PyObject * convert(Pointee const & value);
        static void*   convertible(PyObject* ptr);
        static object_type const * get_pytype(void);
        static object_type * set_pytype(void);
        //@}

        //@{
        /// Helpers to add methods to the class instances
        /// Taken from boost::python::class_<>
        template <class T, class Fn, class Helper>
        void * def_impl(
                T*
                , char const* name
                , Fn fn
                , Helper const& helper
                , ... );

        template <class Fn, class A1>
        void def(
                char const* name
                , Fn fn
                , A1 const& a1
                , ... );
        //@}

    protected:
        //@{
        /// Generic PyTypeObject member functions:
        static value_type * p_new(object_type * subtype,
                                  PyObject    * args,
                                  PyObject    * kwds);
        static void     p_dealloc(value_type  * self);
        static void        p_free(value_type  * self);
        static PyObject *  p_repr(value_type  * self);
        static int   p_type_is_gc(object_type * py_type);
        static int class_setattro(PyObject    * self,
                                  PyObject    * name,
                                  PyObject    * value);
        // Don Beaudry hook functions
        static PyObject *  p_call(PyObject    * other,
                                  PyObject    * args,
                                  PyObject* kwds);
        //value_type * operator()(void);
        //value_type * operator()(Pointee& other);
        //PyObject * operator()(value_type * other, PyObject * args, PyObject* kwds);
        //@}

        //@{
        /// PyGetSet member functions
        static PyObject* ios_get_dict(PyObject* op, void*);
        static int ios_set_dict(PyObject* op, PyObject* dict, void*);
        //@}

        //@{
        /// helper functions

        /// get_buf
        ///
        /// Given a Python object `obj`, check if it is convertible to a buffer 
        /// type, (e.g. strings, buffers, memoryviews, etc.), and return the raw
        /// buffer in buf, and its size in `size`.
        /// A couple of "features" to be aware of:-
        ///   If `obj` is a new-type Py_buffer, then it will need to be manually
        /// released by the caller, with the function `PyBuffer_Release(...`.
        ///   If `obj` is a class derived from one of the wrapped clases, then
        /// we can't get the buffer here, as this assumes that `obj` is an
        /// `istream`. The best we can do is to `return 0` and not set an error,
        /// but leave both `buf` and `size` untouched.
        /// \return - `0` if successful, otherwise sets a Python error and
        ///           returns `-1`.
        static int get_buf(PyObject* obj, void ** buf, Py_ssize_t * size);
        //@}

        //@{
        /// PyBufferProcs members
# if PY_VERSION_HEX >= 0x02060000
        static int  p_getbuf(value_type * self, Py_buffer * view, int flags);
        static void p_releasebuf(value_type * self, Py_buffer * view);
# endif
        //@}
    };


} } } // End Boost Python namespaces

# include "ios_base.tcc"

#endif
