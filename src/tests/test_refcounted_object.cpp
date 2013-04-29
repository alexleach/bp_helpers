#include "boost_helpers/refcounted_object.hpp"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/module.hpp>
#include <boost/python/pure_virtual.hpp>


// Class with protected constructor / destructor
class A
{
public:
    virtual void setValue( bool val ) { m_value = val; }
    virtual bool getValue() const { return m_value; }
protected:
    A( bool val = false ){ m_value = val; }
    virtual ~A() {}
private:
    bool m_value;
};

// Abstract class with pure-virtual print method.
class APrinter
{
public:
    virtual void Print(const A& a) = 0; ///< Pure virtual method makes abstract class
};

// private class for Run_A(2) to generate instances of A
class A_Maker
    : public A
{
public:
    A_Maker(bool val = false): A(val) {}
};

// Main API entrance point.
void Run_A(APrinter* printer)
{
    A_Maker a(true);
    bool val = false;
    // Generate dummy data.
    for(int i=0; i<5; i++)
    {
        a.setValue((val=!val));
        printer->Print(a);
    }
}


// Like APrinter, but with non-pure virtual method (there's a default 
//  implementation)
class APrinter2
{
public:
    virtual void Print(const A& a)
    {
        printf("default Print implementation\n");
    }
};

// Just making sure that it makes no difference if the APrinter class is 
// abstract (has a "pure-virtual" method), or not.
void Run_A2(APrinter2* printer)
{
    bool value = true;
    A_Maker a(value);
    // Do some silly task.
    for(int i=0; i<5; i++)
    {
        value = !value;
        a.setValue(value);
        printer->Print(a);
    }
}



// ----------------------------------------------------------------------------
// Boost wrappers -- mylib.hpp

using namespace boost::python;

// Wrap the un-(de|con)structable object
class A_Wrap
    : public A,
      public ref_counted_object_base //< add ref-counting functionality
{
public:
    // public methods inherited from A
protected:
    // Like base class, can have a protected ctor
    A_Wrap(bool val = false): A(val) {}
};


// Wrapper for abstract printer class
class APrinter_Wrap
    : public APrinter,
      public wrapper<APrinter>
{
public:
    void Print(const A& a);
};


// Wrapper for printer class with default implementation
class APrinter2_Wrap
    : public APrinter2,
      public wrapper<APrinter2>
{
public:
    void Print(const A& a);
    void default_Print(APrinter2& inst, A& a);
};


// ----------------------------------------------------------------------------
// Wrapper method definitions

// Call method overridden in Python class, or use a default implementation
void APrinter_Wrap::Print(A const& a)
{
    // 'print' is a reserved word in Python. So, expose as something else.
    if (override python_printer = this->get_override("write"))
        python_printer(boost::ref(a));
}


// APrinter2_Wrap needs an extra method, wrapping the default implementation.
void APrinter2_Wrap::Print(A const& a)
{
    if (override python_printer = this->get_override("write"))
        python_printer(boost::ref(a));
    else
        this->APrinter2::Print(boost::ref(a));
}

void APrinter2_Wrap::default_Print(APrinter2& inst, A& a)
{
    if (dynamic_cast<APrinter2_Wrap *>(&inst) )
        inst.APrinter2::Print(boost::ref(a));
    else
        inst.Print(boost::ref(a));
}


// ----------------------------------------------------------------------------
// Boost Python module registration
//
BOOST_PYTHON_MODULE(refcounted_object)
{
    boost::python::def("run", &Run_A);
    boost::python::def("run", &Run_A2);

    //ref_counted_object_from_python<A, A_Wrap>();
    //register_ptr_to_python< boost::shared_ptr<A> >();

    class_< A, boost::noncopyable >("A", no_init)
    //class_< A_Wrap, ref_counted_object< A_Wrap >, boost::noncopyable >( "A", 
    //    no_init )
        .def( "setValue", &A_Wrap::setValue )
        .def( "getValue", &A_Wrap::getValue )
        ;

    {
        // APrinter_Wrap
        // Demonstrates how to wrap a class with pure-virtual methods, that
        // takes a non-constructable, non-destructable, object as argument.
        typedef class_< APrinter_Wrap, boost::noncopyable > APrinter_Wrap_t;
        APrinter_Wrap_t APrinter_exposer = APrinter_Wrap_t("APrinter", no_init)
            .def( init<>("documentation") );
        {
            // How does this expand?
            typedef void ( APrinter_Wrap::*default_execute_function_type )(
                    APrinter&, A& );

            // Expose pure-virtual method:-
            APrinter_exposer.def( "write", pure_virtual(
                    default_execute_function_type( &APrinter_Wrap::Print ) ),
                (arg("a")) );

        }
    }


    {
        // APrinter2_Wrap
        // Demonstrates wrapping a class with non-pure virtual method, that
        // takes a const reference to an un-(con|de)structable type.
        typedef class_< APrinter2_Wrap, boost::noncopyable > APrinter2_Wrap_t;
        APrinter2_Wrap_t APrinter2_exposer = APrinter2_Wrap_t("APrinter2", no_init)
            .def( init<>("documentation") );
        {
            // How does this expand?
            typedef void ( APrinter2_Wrap::*default_execute_function_type )(
                    APrinter2&, A& );

            // Expose virtual method:-
            APrinter2_exposer
                .def( "write", 
                    default_execute_function_type( &APrinter2_Wrap::Print ),
                    (arg("a")) )

                // There's a default implementation, overload : -
                .def( "write",
                    default_execute_function_type( &APrinter2_Wrap::default_Print ),
                    (arg("inst"), arg("a")),
                    "Default print implementation" );
        }
    }

}
