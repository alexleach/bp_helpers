/// Templates to ease exposing std::list types.

/// Taken from http://stackoverflow.com/a/6778254/566098

#include <list>
#include <algorithm>

#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/suite/indexing/container_utils.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/list.hpp>

#include "boost_helpers/make_callback.hpp"

namespace boost { namespace python {

    // Forward declarations
    template <class Container, bool NoProxy, class DerivedPolicies>
        class list_indexing_suite;

    namespace detail {
        template <class Container, bool NoProxy>
        class final_list_derived_policies
            : public list_indexing_suite<Container,
                NoProxy, final_list_derived_policies<Container, NoProxy> > {};
    }

    // list_indexing_suite class template
    template <
        class Container, 
        bool NoProxy = false,
        class DerivedPolicies 
            = detail::final_list_derived_policies<Container, NoProxy> >
    class list_indexing_suite 
        : public indexing_suite<Container, DerivedPolicies, NoProxy>
    {
    public:
        typedef typename Container::value_type             value_type;
        typedef typename Container::iterator                iter_type;
        typedef typename Container::const_iterator    const_iter_type;
        typedef typename Container::size_type               size_type;
        typedef typename Container::size_type              index_type;

        //BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(list_indexing_suite, list);

        template <class Class>
        static void 
        extension_def(Class& cl)
        {
            cl
                .def("__repr__", &repr,         (arg("self")))
                .def("append"  , &append, with_custodian_and_ward<1,2>(),
                    (arg("value")))
                .def("count"   , &count,        (arg("self"), arg("value")))
                .def("extend"  , &base_extend,  (arg("self"), arg("iterable")))
                .def("index"   , &index,        (arg("self"), arg("value")))
                .def("insert"  , &insert,       (arg("self"), arg("index"),
                                                 arg("item")))
                .def("pop"     , &pop1,         (arg("self")))
                .def("pop"     , &pop2,         (arg("self"), arg("index")))
                .def("remove"  , &remove_item,  (arg("self"), arg("value")))
                .def("reverse" , &reverse,      (arg("self")))
                .def("sort"    , &sort1,        (arg("self")))
                .def("sort"    , &sort2,        (arg("self"), arg("cmp")),
                    "EXPERIMENTAL!")
            ;
        }

        static index_type
        adjust_index(index_type current, index_type from,
            index_type to, size_type len
        )
        {
            if (from > to)
            {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                throw_error_already_set();
            }
            if (current < from)
                return current; // changed slice doesn't affect pointer
            if (current > to)
                return current - len; // pointer shifted by number of del'd els.
            // item pointed at has been deleted.
            return NULL; // return null pointer
        }

        static void append(Container& x, value_type const& v)
        {
            x.push_back(v);
        }

        template <class T>
        static bool contains(Container& container, T const& val)
        {
            return std::find(container.begin(), container.end(), val)
                != container.end();
        }

        static index_type
        convert_index(Container& container, PyObject* i_)
        {
            extract<long> i(i_);
            if (i.check())
            {
                long index = i();
                if (index < 0)
                    index += DerivedPolicies::size(container);
                if (index >= long(container.size()) || index < 0)
                {
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    throw_error_already_set();
                }
                return index;
            }
            
            PyErr_SetString(PyExc_TypeError, "Invalid index type");
            throw_error_already_set();
            return index_type();
        }

        static size_type count(Container& x, value_type const& v)
        {
            size_type total = 0;
            for (iter_type it=x.begin(); it!=x.end(); it++)
                if (*it == v) ++total;
            return total;
        }

        static void delete_item(Container& x, index_type i)
        {
            if( i < 0 ) 
                i += x.size();

            iter_type pos = x.begin();
            advance(pos, i);

            if( i >= 0 && i < x.size() ) {
                x.erase(pos);
            } else {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                boost::python::throw_error_already_set();
            }
        }

        static void delete_slice(Container& container, index_type from, 
                                 index_type to)
        {
            if (from > to) {
                // A null-op.
                return;
            }
            iter_type pos1, pos2;
            pos1 = pos2 = container.begin();
            advance(pos1, from);
            advance(pos2, to);
            container.erase(pos1, pos2);
        }

        template <class Iter>
        static void extend(Container& x, Iter first, Iter last)
        {
            x.insert(x.end(), first, last);
        }

        static void base_extend(Container& x, boost::python::object v)
        {
            std::list<value_type> temp;
            container_utils::extend_container(temp, v);
            DerivedPolicies::extend(x, temp.begin(), temp.end());
        }

        static value_type& get_item(Container& container, index_type i)
        {
            if( i < 0 ) 
                i += container.size();

            if( i >= 0 && i < container.size() ) {
                iter_type it = container.begin(); 
                for(int pos = 0; pos < i; ++pos)
                    ++it;
                return *it;                             
            } else {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                throw boost::python::error_already_set();
            }
        }

        static index_type get_min_index(Container& /*container*/)
        {
            return 0;
        }

        static index_type get_max_index(Container& container)
        {
            return container.size();
        }

        static boost::python::object
        get_slice(Container& container, index_type from, index_type to)
        {
            if (from > to) // return empty list
                return boost::python::object(Container());
            iter_type start, end;
            start = end = container.begin();
            advance(start, from);
            advance(end, to);
            return boost::python::object(Container(start, end));
        }

        static void insert(Container& container, size_type ind, value_type v)
        {
            if (ind > container.size()) // Then put it on the end.
                ind = container.size();
            else if (ind < 0)
                ind += container.size();
            iter_type pos = container.begin();
            advance(pos, ind);
            container.insert(pos, v);
        }
        static size_type index(Container const& x, value_type const& v)
        {
            const_iter_type cur_pos = x.begin(), end = x.end();
            //iter_type cur_pos = x.begin(), end = x.end();
            index_type i = 0;
            for(; cur_pos != end; ++cur_pos, ++i)
                if( *cur_pos == v )
                    return i;

            PyErr_SetString(PyExc_ValueError, "Value not in list");
            throw boost::python::error_already_set();
        }

        // Return and delete the last element of container
        static value_type pop1(Container& container)
        {
            size_type size = container.size();
            if (size == 0)
            {
                PyErr_SetString(PyExc_IndexError, "pop from empty list");
                throw boost::python::error_already_set();
            }
            value_type val = container.back();
            container.pop_back();
            return val;
        }

        static value_type pop2(Container& container, size_type ind)
        {
            size_type size = container.size();
            if (size == 0)
            {
                PyErr_SetString(PyExc_IndexError, "pop from empty list");
                throw boost::python::error_already_set();
            }
            else if (ind > size)
            {
                PyErr_SetString(PyExc_IndexError, "pop index out of range");
                throw boost::python::error_already_set();
            }
            //else
            //    ind = size;
            iter_type pos = container.begin();
            advance(pos, ind);
            value_type value = *pos;
            container.erase(pos);
            return value;
        }

        static void remove_item(Container& container, value_type const& v)
        {
            size_type ind = index(container, v);           
            iter_type pos = container.begin();
            advance(pos, ind);
            container.erase(pos);
        }

        static std::string repr(Container& x)
        {
            std::string repr("[");
            iter_type cur_ptr = x.begin(), end = x.end();
            boost::python::object item;
            //size_type ind = 0, size = x.size();
            while (cur_ptr != end)
            {
                item = boost::python::object( *cur_ptr++ );
                repr += boost::python::extract<std::string>(item.attr("__repr__")())();
                repr += ", ";
            }
            repr.resize(repr.length()-2);
            repr += "]";
            return repr;
        }

        // The Python reverse algorithm is much more flexible, but just expose
        // the C++ STL one for now.
        static void reverse(Container& container)
        {
            container.reverse();
        }

        static void sort1(Container& container)
        {
            container.sort();
        }

        static void sort2(Container& container, PyObject* cmp)
        {
#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
            // Can use C++11 variadic templates.
            make_callback<bool, value_type&, value_type&> callback(cmp);
#else
            make_callback2<bool, value_type&, value_type&> callback(cmp);
            // Try to use a generic C++98 template. Massive fail.
            //typedef typename 
            //    boost::python::make_callback<bool, value_type&>::CallBackType
            //        callback_t;

            // Pointer to member function
            //callback_t gen_cb = &make_callback<bool, value_type&>::call_va_args;
            //callback_t callback = (&callback_class)->*gen_cb;
#endif
            container.sort( callback );

        }

        static size_t size(Container& container)
        {
            return container.size();
        }

        static void set_slice(Container& container, index_type from,
                              index_type to,    value_type const& v )
        {
            index_type tot_len = container.size();
            if (from < 0)
                from += tot_len;
            if (to < 0)
                to += tot_len;
            iter_type start, end;
            start = end = container.begin();
            advance(start, from);
            advance(end, to);
            start = container.erase(start, end);
            container.insert(start, v);
        }

        static void set_item(Container& x, int i, value_type const& v)
        {
            if( i < 0 ) 
                i += x.size();

            if( i >= 0 && i < (int)x.size() ) {
                iter_type it = x.begin(); 
                for(int pos = 0; pos < i; ++pos)
                    ++it;
                *it = v;
            } else {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                boost::python::throw_error_already_set();
            }
        }

        template <class Iter>
        static void
        set_slice(Container& container, index_type from,
                  index_type to, Iter first, Iter last)
        {
            iter_type start, end;
            start = end = container.begin();
            advance( start, from );
            advance( end, to );
            if (from > to) {
                container.insert(start, first, last);
            }
            else {
                start = container.erase(start, end);
                container.insert(start, first, last);
            }
        }

    }; /* end list_indexing_suite */


} /* python ns */ } /* boost ns */
