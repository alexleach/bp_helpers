/// Thanks to Mark English for sharing this. Taken and modified from a post he
/// made to the cplusplus-sig mailing list, at:-
///    http://thread.gmane.org/gmane.comp.python.c++/11763/focus=11767

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python/converter/pytype_object_mgr_traits.hpp>

#include "boost_helpers/return_buffer_object.hpp"

//#include "boost_helpers/return_buffer_object.cpp"
//#include "boost_helpers/buffer_pointer_converter.cpp"

#include <streambuf>

namespace BufferObjectExample {

    class Stream
        : public std::stringstream
    { };

    class Foo { } ;

    struct Container
    {
        // Populate the iostream with some data. Test getting it back out.
        Container(void) //: m_buf()
        {
            m_buf << "foo bar baz";
            m_buf.seekp(0); //< Move back to start of buffer.
        }
        Stream m_buf;
        Foo    m_obj; // = Foo();
    };

}


namespace boost { namespace python { 

    // TODO.
    // Make this register the object!! Preferably template-ised in:
    //    buffer_pointer_object.hpp
    //
    // Converter Specializations
    //
/*
    namespace converter
    {
        template <>
        struct object_manager_traits<BufferObjectExample::Stream>
            : pytype_object_manager_traits<
                &buffer<BufferObjectExample::Stream>::type_object
                , BufferObjectExample::Stream
            >
      {
      };
    }
*/
}   } 

namespace BufferObjectExample {

    namespace bpl = boost::python;

    void register_test_buffer_object(void)
    {

        // How can we remove the need to use a to_python_converter? str.hpp
        // doesn't use one...

        //boost::python::to_python_converter<BufferObjectExample::Stream, 
        //    boost::python::buffer<BufferObjectExample::Stream>, 
        //        true >();
        boost::python::make_buffer_type_object<BufferObjectExample::Stream>();

        // Expose Foo object, either via to_python_converter(how?), or class_
        // is easier.
        //boost::python::to_python_converter<Foo,
        //        boost::python::to_python_value<Foo>,  true>();
        boost::python::class_<BufferObjectExample::Foo>("Foo");

        boost::python::class_<BufferObjectExample::Container,
                              boost::noncopyable>
            ("Container", boost::python::init<>() )
            // This works but object lifetime is incorrect
            .def_readwrite("obj",
                &BufferObjectExample::Container::m_obj )

            // The stringstream assignment and copy ctors are private, so the
            // property has to be read-only.

            // def_readonly doesn't work. Fixing it should allow removal of
            // above to_python_converter call.
            //.def_readonly("buf", &BufferObjectExample::Container::m_buf)

            // Readonly property works, with to_python_converter.
            .add_property("buf",
                bpl::make_getter(
                    &BufferObjectExample::Container::m_buf, // ) )
                    bpl::return_buffer_reference<>() ) )

            // Fails to compile, because stringstream::operator= is private.
            //    bpl::make_setter(
            //        &BufferObjectExample::Container::m_buf,
            //        bpl::reference_existing_buffer() ) )
            ;


    }
} // End namespace ConversionExample

//namespace {
    BOOST_PYTHON_MODULE(test_buffer_object)
    {
      BufferObjectExample::register_test_buffer_object();
    }
//} // Ends anonymous namespace
