
all: tests

tests: $(TESTS)
	for f in `ls tests/test_*.py` ; do      \
		echo   "$${f}" ;                \
		python "$${f}"  ;      \
	done

clean: 
	-rm -r build
	-rm ./*.so

.PHONY: all clean tests

