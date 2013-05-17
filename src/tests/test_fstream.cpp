#include "boost_helpers/streaming/return_iostream.hpp"
#include "boost_helpers/streaming/iostream.hpp"

#include <boost/python/bases.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>

#include <fstream>

namespace test_fstream {
    using namespace std;

    class wrap_fstream
        : public fstream
    {
    public:
        // Manually expose constructors:
        wrap_fstream(void) : fstream()
        {
            printf("Initialising wrap_fstream(void)\n");
        }

        wrap_fstream(string const& name,
                     ios_base::openmode mode = ios_base::in | ios_base::out)
            : fstream(name.c_str(), mode)
        {
            printf("Initialising wrap_fstream(name, mode)\n");
            if (!this->good())
                PyErr_SetString(PyExc_Exception, "file could not be opened.");
        }

        void close(void)
        {
            this->close();
        }

    };
}

namespace boost { namespace python { 

    namespace converter
    {
        // Does this help at all??? Doesn't seem to make a difference...
        // Until I find a use for this, I won't bother automating it in the
        // public headers.

        typedef iostream<test_fstream::wrap_fstream> fstream_wrapper;

        template <>
        struct object_manager_traits< fstream_wrapper >
            : pytype_object_manager_traits<
                &fstream_wrapper::m_type
                , fstream_wrapper
              >
        {
            typedef fstream_wrapper::object_type type;
        };

    } /* -- end converter namespace */

}   } 

namespace test_fstream {

    namespace bpl = boost::python;
    void register_test_iostreams(void)
    {

        printf("make_iostream_type_object\n");
        /// Generate to_python_converter for `std::fstream`, and insert it into
        /// Boost Python registry. This allows the Python interpreter to hold
        /// and manipulate an instance returned from a function.
        bpl::make_iostream_type_object<fstream>("my_fstream")();

        // Add fstream to module namespace. Similar to :-
        //   `bpl::class_<std::fstream>("fstream");`,
        // but this uses a PyTypeObject that supports the Python Buffer
        // protocol.
        // We expose it as `_fstream`, because std::fstream has a unique
        // constructor that `make_iostream_type_object` can't figure out.
        //printf("add_type_to_module\n");
        //bpl::add_type_to_module<fstream>(&m_fstream.m_type, "_fstream");

        //bpl::add_type_to_module<fstream>(&m_fstream.istream_base::m_type, "istream");
        //bpl::add_type_to_module<fstream>(&m_fstream.ostream_base::m_type, "ostream");
        //bpl::add_type_to_module<fstream>(&m_fstream.base_type::m_type, "iostream_base");
        printf("\nConstructing subclass\n\n");

        // Put the C++ class constructors in a derived class, to allow
        // initialisation from Python. Using `bases<>`, we can use our custom
        // PyTypeObject in derived classes :)
        // Making `ios_base::openmode` easy to use is left as an exercise for
        // the reader. (Probably want to use the same function Python's
        // file object uses, to determine its open mode).
        bpl::class_<wrap_fstream,
                    boost::noncopyable,
                    bpl::bases<fstream> >
            ("fstream", "std::fstream constructor",
             bpl::init<>() )
            .def(bpl::init<std::string const&, bpl::optional<ios_base::openmode> >
                (bpl::args("file_name", "openmode")))
            .def("close", &wrap_fstream::close)
            //.def("write", &wrap_fstream::write)
            ;

        printf("\nModule imported\n");
    }
}


BOOST_PYTHON_MODULE(fstream)
{
  test_fstream::register_test_iostreams();
}
