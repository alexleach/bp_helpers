
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>

#include "boost_helpers/streaming/return_istream.hpp"
#include "boost_helpers/streaming/return_ostream.hpp"
#include "boost_helpers/streaming/return_iostream.hpp"

#include "boost_helpers/streaming/istream.hpp"
#include "boost_helpers/streaming/ostream.hpp"
#include "boost_helpers/streaming/iostream.hpp"

#include <iosfwd>

namespace test_io_streams {

    // Is testing only stringstreams sufficient? probably not..

    // IStream. An istream-like class
    class IStream
        : public std::istringstream
    { };

    // OStream. An istream-like class
    class OStream
        : public std::ostringstream
    { };

    // IOStream. An iostream-like class
    class IOStream
        : public std::stringstream
    { };

    // Test return value policies from a container class.
    class Container
    {
    public:
        IStream  m_istream;
        OStream  m_ostream;
        IOStream m_iostream;
        Container(void)
        {
            m_iostream << "Foo bar baz. Initialised from C++";
        }
        const OStream& GetOStream(void) { return m_ostream; }
    };

    class DerivedIOStream
        : public IOStream
    { };

    void register_test_iostreams(void);

}

# include <boost/python/converter/pytype_object_mgr_traits.hpp>

namespace boost { namespace python { 

    namespace converter
    {
        // Does this help at all??? Doesn't seem to make a difference...
        // Until I find a use for this, I won't bother automating it in the
        // public headers.

        template <>
        struct object_manager_traits< istream<test_io_streams::IStream> >
            : pytype_object_manager_traits<
                &istream<test_io_streams::IStream>::m_type
                , istream<test_io_streams::IStream>
              >
        { };

        template <>
        struct object_manager_traits< ostream<test_io_streams::OStream> >
            : pytype_object_manager_traits<
                &ostream<test_io_streams::OStream>::m_type
                , ostream<test_io_streams::OStream>
              >
        { };

        template <>
        struct object_manager_traits< iostream<test_io_streams::IOStream> >
            : pytype_object_manager_traits<
                &iostream<test_io_streams::IOStream>::m_type
                , iostream<test_io_streams::IOStream>
              >
        { };

    } /* -- end converter namespace */

}   } 


namespace test_io_streams {
    namespace bpl = boost::python;

    void register_test_iostreams(void)
    {
        // Register to_python conversions and add the object type IDs to the
        // Boost Python registry.
        bpl::istream<IStream>   m_istream  = bpl::make_istream_type_object<IStream>("IStream")();
        bpl::ostream<OStream>   m_ostream  = bpl::make_ostream_type_object<OStream>("OStream")();
        bpl::iostream<IOStream> m_iostream = bpl::make_iostream_type_object<IOStream>("IOStream")();

        // Add object to current namespace. This makes the class instantiable
        // from within Python.
        //bpl::add_type_to_module<IStream>(&m_istream.m_type,   "IStream");
        //bpl::add_type_to_module<OStream>(&m_ostream.m_type,   "OStream");
        //bpl::add_type_to_module<IOStream>(&m_iostream.m_type, "IOStream");

        // The class_<> template mechanisms don't allow the use of a custom
        // PyTypeObject, so unfortunately it can't be used if we want to get any
        // performance benefits. Shame...

        /*
        typedef bpl::istream<IStream> istream_converter;
        typedef bpl::ostream<OStream> ostream_converter;
        bpl::class_<test_io_streams::IOStream,
                    boost::noncopyable>
                    //boost::python::bases<IStream, OStream> >
            ("IOStream", bpl::init<>()[boost::python::return_iostream()] )
            .def( bpl::iostream<test_io_streams::IOStream>() )
            //.def("peek", &test_io_streams::IOStream::peek )
            ;
        */

        // For registering a to_python conversion, all that is really needed,
        // is this:
    //    boost::python::to_python_converter<OStream, 
    //        boost::python::ostream<OStream>
    //# ifdef BOOST_PYTHON_SUPPORTS_PY_SIGNATURES
    //            , true
    //# endif
    //        >();

        printf("Registering Container class\n");
        bpl::class_<test_io_streams::Container, boost::noncopyable>
            ("Container",
             "Container class for testing return_iostream()",
             bpl::init<>() )
            .add_property("istream",
                bpl::make_getter(
                    &test_io_streams::Container::m_istream,
                    bpl::return_iostream() ) )

            .add_property("ostream",
                bpl::make_getter(
                    &test_io_streams::Container::m_ostream,
                    bpl::return_iostream() ) )

            .def("get_ostream",
                &test_io_streams::Container::GetOStream ,
                bpl::return_iostream() )

            //.def("get_internal_reference_ostream",
            //    "Return an ostream object with the default Call Policy",
            //    &test_io_streams::Container::GetOStream )

            .add_property("iostream",
                bpl::make_getter(
                    &test_io_streams::Container::m_iostream,
                    bpl::return_iostream() ) )
            ;

        bpl::class_<DerivedIOStream, boost::noncopyable >
            ("DerivedIOStream", "This should fail to use the right PyTypeObject.")
            ;

        bpl::class_<DerivedIOStream, boost::noncopyable, bpl::bases<IOStream> >
            ("DerivedBaseIOStream",
             "Using bases<> works to override the class metatype.")
            ;
    }

}

namespace {
    BOOST_PYTHON_MODULE(iostreams)
    {
      test_io_streams::register_test_iostreams();
    }
} // Ends anonymous namespace
