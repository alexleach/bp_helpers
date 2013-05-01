
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>

#include "boost_helpers/return_istream.hpp"
#include "boost_helpers/return_ostream.hpp"

#include "boost_helpers/istream.hpp"
#include "boost_helpers/ostream.hpp"

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
    };

    void register_test_iostreams(void);

}


namespace test_io_streams {
    namespace bpl = boost::python;

    void register_test_iostreams(void)
    {

        bpl::make_istream_type_object<IStream>();
        bpl::make_ostream_type_object<OStream>();
        //bpl::make_iostream_type_object<IOStream>();

        boost::python::to_python_converter<OStream, 
            boost::python::converters::ostream<OStream>, 
                true >();

        bpl::class_<test_io_streams::Container, boost::noncopyable>
            ("Container", bpl::init<>() )
            .add_property("istream",
                boost::python::make_getter(
                    &test_io_streams::Container::m_istream,
                    boost::python::return_istream_object<>() ) )

            .add_property("ostream",
                boost::python::make_getter(
                    &test_io_streams::Container::m_ostream,
                    boost::python::return_ostream_object<>() ) )

            .def("get_ostream",
                &test_io_streams::Container::GetOStream,
                boost::python::return_ostream_object<>() )

            //.def_readonly("iostream",
            //    boost::python::make_getter(
            //        &test_io_streams::Container::m_iostream,
            //        boost::python::return_iostream_object<>() ) )
            ;
    }

}

namespace {
    BOOST_PYTHON_MODULE(test_iostreams)
    {
      test_io_streams::register_test_iostreams();
    }
} // Ends anonymous namespace
