
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

    // IStream. An istream-like class
    class IStream
        : public std::istringstream
    { };

    // OStream. An istream-like class
    class OStream
        : public std::ostringstream
    { };

    class IOStream
        : public std::stringstream
    { };

    class Container
    {
    public:
        OStream& GetOStream(void) { return m_ostream; }
        IStream  m_istream;
        OStream  m_ostream;
        IOStream m_iostream;
        Container(void)
        {
            m_iostream << "Foo bar baz";
        }
    };

    class DerivedIOStream
        : public IOStream
    { };

    void register_test_iostreams(void);

}

# include <boost/python/converter/pytype_object_mgr_traits.hpp>

namespace boost { namespace python { 

    // TODO.
    // Make this register the object!! Preferably elsewhere.
    namespace converter
    {

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

        printf("Making IStream, OStream and IOStream PyTypeObjects\n");
        bpl::istream<IStream>   m_istream  = bpl::make_istream_type_object<IStream>();
        bpl::ostream<OStream>   m_ostream  = bpl::make_ostream_type_object<OStream>();
        bpl::iostream<IOStream> m_iostream = bpl::make_iostream_type_object<IOStream>();

        // Add object to current namespace. 
        bpl::add_type_to_module<IStream>(&m_istream.m_type,   "IStream");
        bpl::add_type_to_module<OStream>(&m_ostream.m_type,   "OStream");
        bpl::add_type_to_module<IOStream>(&m_iostream.m_type, "IOStream");

        // The class_ template mechanisms don't allow the use of a custom
        // PyTypeObject, so unfortunately it can't be used if we want to get any
        // performance benefits.
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

        //boost::python::to_python_converter<OStream, 
        //    boost::python::ostream<OStream>, 
        //        true >();

        printf("Registering Container class\n");
        bpl::class_<test_io_streams::Container, boost::noncopyable>
            ("Container", bpl::init<>() )
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

            .add_property("iostream",
                bpl::make_getter(
                    &test_io_streams::Container::m_iostream,
                    bpl::return_iostream() ) )
            ;

        bpl::class_<DerivedIOStream, boost::noncopyable, bpl::bases<IOStream> >
            ("DerivedIOStream" )
            ;

        printf("Test Module imported\n");
    }

}

namespace {
    BOOST_PYTHON_MODULE(iostreams)
    {
      test_io_streams::register_test_iostreams();
    }
} // Ends anonymous namespace
