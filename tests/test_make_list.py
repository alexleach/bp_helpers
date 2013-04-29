""" Test Boost code in boost_helpers/make_list.hpp.
As this is a template-only C++ module, need to compile and test some example 
specialisations."""

import unittest
from compile_util import *

def setUpModule():
    compile_test("test_make_list")
    # We've just compiled the extension module inplace. Insert cwd into
    #  PYTHONPATH.
    import os, sys
    sys.path.insert(0, os.getcwd())
    # import it and insert into global namespace
    try:
        from test_make_list import IntList, StrList
    except ImportError:
        raise unittest.SkipTest("Can't import test_make_list compiled module. "\
            "Why?! Try running tests individually and they should run fine...")
    global IntList, StrList

def tearDownModule():
    clean_test()

class TestIntList(unittest.TestCase):
    def setUp(self):
        self.cmp = [2, 1, 3, 5, 1, 1]
        try:
            self.x = IntList()
        except NameError:
            return 1
        self.x[:] = self.cmp

    def test_init(self):
        x = IntList()

    def test_append(self):
        self.x.append(3)

    def test_contains(self):
        assert 2 in self.x

    def test_count(self):
        self.assertEqual(self.x.count(1), 3)

    def test_delitem(self):
        del(self.x[0])
        del(self.cmp[0])
        self.assertSequenceEqual(self.x, self.cmp)

    def test_extend(self):
        self.x.extend([3, 4, 5])
        self.cmp.extend([3, 4, 5])
        self.assertSequenceEqual(self.x, self.cmp)

    def test_getitem(self):
        self.assertEqual(self.x[0], 2)

    def test_index(self):
        ind = self.x.index(3)
        self.assertEqual(ind, 2)

    def test_insert(self):
        self.x.insert(0, 0)
        self.assertEqual(self.x[0], 0)

    def test_iter(self):
        i = 0
        for x_item in self.x:
            self.assertEqual(x_item, self.cmp[i])
            i += 1

    def test_len(self):
        self.assertEqual(len(self.x), len(self.cmp))

    def test_pop(self):
        self.assertEqual(self.x.pop() , 1) # pop the last item
        self.assertEqual(self.x.pop(2), 3) # pop the second item

    def test_remove(self):
        self.x.remove(2)
        self.assertEqual(self.x[0], 1)

    def test_reverse(self):
        self.cmp.reverse()
        self.x.reverse()
        self.assertSequenceEqual(self.x, self.cmp)

    def test_setitem(self):
        self.x[1] = 2048
        self.assertEqual(self.x[1], 2048)

    def test_slice(self):
        x = IntList()
        x[:] = [1, 2, 3]

    def test_sort(self):
        self.cmp.sort()
        self.x.sort()
        self.assertSequenceEqual(self.x, self.cmp)

class TestStrList(unittest.TestCase):
    def setUp(self):
        try:
            self.x = StrList()
        except NameError:
            return 1
        self.cmp =  ["foo", "bar", "baz"]
        self.x[:] = ["foo", "bar", "baz"]

    def test_init(self):
        y = StrList()
        try:
            y2 = StrList(["foo", "bar", "baz"])
        except Exception:
            pass # How to overload ctor?

    def test_append(self):
        self.x.append("foo")

    def test_contains(self):
        assert "bar" in self.x

    def test_count(self):
        #x = StrList(["foo", "bar", "baz"])
        self.assertEqual(self.x.count("foo"), 1)

    def test_delitem(self):
        del(self.x[0])
        del(self.cmp[0])
        self.assertSequenceEqual(self.x, self.cmp)

    def test_extend(self):
        ex = ["bar", "fubar", "bam!"]
        self.x.extend(ex)
        self.cmp.extend(ex)
        self.assertSequenceEqual(self.x, self.cmp)

    def test_getitem(self):
        self.assertEqual(self.x[0], "foo")

    def test_index(self):
        ind = self.x.index("bar")
        self.assertEqual(ind, 1)

    def test_insert(self):
        self.x.insert(0, "foobar")

    def test_iter(self):
        y = ["foo", "bar", "baz"]
        i = 0
        for x_value in self.x:
            self.assertEqual(x_value, y[i])
            i += 1

    def test_len(self):
        self.assertEqual(len(self.x), 3)

    def test_pop(self):
        self.x.pop()

    def test_remove(self):
        self.x.remove("bar")

    def test_reverse(self):
        self.cmp.reverse()
        self.x.reverse() # --> "baz", "bar", "foo"
        self.assertSequenceEqual(self.x, self.cmp)

    def test_setitem(self):
        self.x[1] = "foo_bar"

    def test_slice(self):
        self.x[1:]   = "foobar" # this inserts a single string.
        self.cmp[1:] = "foobar" # this inserts 6 separate characters.
        del(self.cmp[1:]) # to make them agree, need 2 operations in Python:-
        self.cmp.append("foobar")

        self.assertSequenceEqual(self.x, self.cmp)

    def test_sort(self):
        """Test C++ STL list::sort algorithm."""
        self.x.sort() # --> "foo", "bar", "baz"
        self.cmp.sort()
        self.assertSequenceEqual(self.x, self.cmp)

    def test_sort2(self):
        """Test list::sort, with a binary predicate callback as argument."""
        def py_compare(x, y):
            """Can return -1, 0, or +1"""
            if x > y:
                return -1
            elif x == y:
                return 0
            else:
                return 1
        def c_compare(x, y):
            """Must return bool's!"""
            if x > y:
                return True
            else:
                return False
        self.cmp.sort(py_compare)
        self.x.sort(c_compare)
        self.assertSequenceEqual(self.x, self.cmp)

if __name__ == '__main__':
    unittest.main()
