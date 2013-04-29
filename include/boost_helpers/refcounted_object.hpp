/*
 * Taken from the Boost.Python mail archives, at:-
 *  http://mail.python.org/pipermail/cplusplus-sig/2002-December/002812.html
 * 
 * Thanks to the author, Daniel Paull, for sharing it!
 * 
 * This gets around the issue of classes that have protected destructors.
 * e.g. the LogMessage class in corelib/ncbi_toolkit.hpp
 *
 */

#include <boost/ref.hpp>

#include <boost/python/converter/from_python.hpp>
#include <boost/python/converter/rvalue_from_python_data.hpp>
#include <boost/python/converter/registered.hpp>
#include <boost/python/extract.hpp>

namespace boost { namespace python {

    //! ref_counted_object_base
    //
    // Classes with private destructors should inherit from this. This will
    // add reference counting functionality to whatever object has a private
    // destructor
    class ref_counted_object_base
    {
    public:
        explicit ref_counted_object_base(void): m_refCount(0) {}
        virtual ~ref_counted_object_base(void) {}

        /* Refcounting methods to add to derived classes */
        virtual void addRef(void)     { ++m_refCount; }
        virtual void releaseRef(void) { if( !--m_refCount ) delete this; }
        virtual int getRefCount(void) { return 0; }    
    private:
        int m_refCount;
    };


    //! ref_counted_object
    //
    // This should be used in \ref boost::python::class_ definitions, so that
    // Boost Python temporary objects don't get created / destroyed for the
    // object with protected dtors.
    template< class T >
    class ref_counted_object
//        : public boost::reference_wrapper<T>
    {
    public:
        typedef T element_type;
        explicit ref_counted_object( T* t ) : m_pPtr( t )
        {
            if ( m_pPtr ) m_pPtr->addRef();
        }
        ~ref_counted_object()
        {
            if ( m_pPtr ) m_pPtr->releaseRef();
        }
        T& operator*() const { return *get(); }
        T* operator->() const { return get(); }
        T* get() const { return m_pPtr; }

    private:
        T*  m_pPtr;
    };

    template<class T>
    T * get_pointer( ref_counted_object<T> const& p )
    {
        return p.get();
    }


    template <class T>
    struct ref_counted_rvalue_from_python_data : converter::rvalue_from_python_data<T>
    {
        // Destroys any object constructed in the storage.
        ~ref_counted_rvalue_from_python_data();
    private:
        typedef typename add_reference<typename add_cv<T>::type>::type ref_type;
    };

    template <class T>
    inline ref_counted_rvalue_from_python_data<T>::~ref_counted_rvalue_from_python_data()
    {
        if (this->stage1.convertible == this->storage.bytes)
            python::detail::destroy_referent<ref_type>(this->storage.bytes);
    }

    using namespace boost::python;
    // from_python converter for Objects with protected destructors.
    // Needs to be instantiated with two classes:
    //   @param T - The base class type which has protected destructor.
    //   @param D - A class derived from 'T' and 'ref_counted_object_base'
    template<class T, class D>
    class ref_counted_object_from_python
    {
    public:
        ref_counted_object_from_python()
        {
            converter::registry::insert(
                &convertible,
                &construct,
                type_id<T>() );

            converter::registry::insert(
                &convertible,
                &construct,
                type_id<D>() );
        }

        // determine if object can be converted from a Python object
        // Object has no_init method..
        static void* convertible(PyObject * p)
        {
            if (p == Py_None)
                return p;
            return converter::get_lvalue_from_python(p,
                converter::registered< ref_counted_object<T> >::converters);
        }

        // from python converter
        static void construct(PyObject* obj_ptr,
            converter::rvalue_from_python_stage1_data* data)
        {
            D * value = boost::python::extract<D*>(obj_ptr);
            void * const storage = ( 
                (converter::rvalue_from_python_storage<
                    ref_counted_object<T> >*)
                data)->storage.bytes;

            new (storage) ref_counted_object<D>(value);
            data->convertible = storage;
        }
    };

}   } /* namespaces */

/*
// needed for latest CVS version of boost.python
namespace boost {
    template<class T> T * get_pointer( python::ref_counted_object<T> const& p )
    {
        return p.get();
    }
}
*/


/*

// Demo example. \sa refcounted_object.cpp
// N.B. Class definition has protected destructor

class A
{
public:
    A( bool val = false ) : m_refCount( 0 ) { m_value = val; }
    virtual void setValue( bool val ) { m_value = val; }
    virtual bool getValue() const { return m_value; }
    virtual void addRef () { ++m_refCount; }
    virtual void releaseRef () { if( !--m_refCount ) delete this; }
    virtual int getRefCount () { return 0; }    
protected:
    virtual ~A() {}
    bool m_value;
    int m_refCount;
};

#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(refcounted_object)
{
    class_< A, RefCountedObject< A >, boost::noncopyable >( "A", 
        //init< bool >() )
        no_init )
        .def( "setValue", &A::setValue )
        .def( "getValue", &A::getValue )
        ;
}
*/
