
all: tests

tests: $(TESTS)
	for f in `ls tests/test_*.py` ; do	\
		echo   "$${f}" ;				\
		python "$${f}" || exit 1 ;		\
	done

clean: 
	rm -r build
	-rm ./*.so

.PHONY: all clean tests

