PRGGCC=testgcc
PRGCLANG=testclang
TESTARGS=100
GCC=g++
GCCFLAGS=-O2 -pedantic -Wall -Wextra -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

CLANG=clang++
CLANGFLAGS=-O2 -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines
VALGRIND_OPTIONS=-q --leak-check=full --tool=memcheck

OBJECTS0=
DRIVER0=Spline/spline_test.cpp

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif
all: build test pass
build: gcc clang
gcc:
	$(GCC) -o $(PRGGCC) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
clang:
	$(CLANG) -o $(PRGCLANG) $(DRIVER0) $(OBJECTS0) $(CLANGFLAGS) 
	
test: difftest memtest
difftest:
	echo "running gcc test"
	 ./$(PRGGCC) $(TESTARGS) > gccout
	 ./$(PRGCLANG) $(TESTARGS) > clangout
	diff clangout gccout $(DIFF_OPTIONS) > difference
memtest:
	echo "running valgring memory test"
	 valgrind $(VALGRIND_OPTIONS) ./$(PRGGCC) $(TESTARGS) $(subst mem,,$@) 1>/dev/null 2>difference$@
	 valgrind $(VALGRIND_OPTIONS) ./$(PRGCLANG) $(TESTARGS) $(subst mem,,$@) 1>/dev/null 2>difference$@
pass: clean
	echo "Passed all tests. Cleaning up."
clean:
	rm -f test* *.o gccout clangout difference*
	
