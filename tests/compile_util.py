"""Generic functions useful for the Boost helper unit tests."""

from distutils.core import Extension, setup
import os, sys

__all__ = ["BASEDIR", "INCDIR", "SRCDIR",
           "adjust_path", "clean_test", "compile_test"]

## Create some global variables for useful paths:-
#  BASEDIR - The top-level directory with setup.py, etc.
#  SRCDIR  - C++ top-level source directory
#  INCDIR  - Include directory when compiling SoS C++ code.

BASEDIR = os.path.abspath(
    os.path.join(
        os.path.dirname(__file__),
        '..' )
    )

SRCDIR = os.path.join(BASEDIR, "src")
INCDIR = os.path.join(BASEDIR, "include")
BUILDDIR = os.getcwd()

def adjust_path():
    """After running compile_test, call this function so that Python can find
    the shared library, which was compiled in-place. This inserts the current
    working directory into PYTHONPATH."""
    sys.path.insert(0, BUILDDIR)

def clean_test(build_dir = None):
    """Do post-build clean-up from here. Checks for a build/ directory, and 
    recursively deletes all file and folders within it."""
    if build_dir is None:
        build_dir = os.path.join( os.getcwd(), 'build')
    abspath = os.path.abspath
    remove = os.remove
    join = os.path.join
    walk = os.walk
    if os.path.exists(build_dir):
        for path, dirs, files in walk(build_dir, topdown=False):
            full_path = abspath(join(build_dir, path))
            for f in files:
                remove(join(full_path, f))
            if not dirs: # empty folder.
                os.removedirs(full_path)

def compile_test(test_name, src_filename=None):
    """Compile extension module named test_name and in the file:-
        ${SRCDIR}/tests/${src_filename}
    The test_name must match the module name given to the BOOST_PYTHON_MODULE
    macro in the C++ file."""
    if src_filename is None:
        src_filename = test_name + '.cpp'
    source = os.path.join( SRCDIR, 'tests', src_filename )
    test_ext = Extension(
        test_name,
        sources = [source],
        include_dirs = [INCDIR],
        #library_dirs = ["/usr/lib"],
        #runtime_library_dirs = ["/usr/lib"],
        language = 'c++',
        libraries = ['boost_python']
        )
    sys.argv += ["build", "build_ext", "--inplace"]
    try:
        setup(
            name=test_name,
            ext_modules=[test_ext]
            )
    finally:
        del(sys.argv[-2:])
