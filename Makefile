# Have debug builds of python2 and gdb libraries in /usr/local/lib:-
LD_LIBRARY_PATH = /usr/local/lib
PYTHON 			= python2-dbg

TESTS = tests/test_buffer_object.py 	tests/test_fstream.py 	\
		tests/test_iostreams.py 		tests/test_make_list.py \
		tests/test_refcounted_object.py

all: tests

$(TESTS):
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(PYTHON) $@ -vvv

tests: $(TESTS)
	#for f in $(TESTS)  ; do    \
	#	echo   "$${f}" ;       \
	#	python "$${f}" ;       \
	#done

clean: 
	if [ -d "build" ] ; then \
		rm -r build   ;      \
	fi
	rm ./*.so

.PHONY: all clean tests $(TESTS)
