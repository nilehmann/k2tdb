CXX = clang++

K2TREE = ../k2tree

HEADERS = $(shell find include tests -name *.h)

INCLUDE = -Iinclude/ -I$(K2TREE)/include

TESTS_SRC = $(shell find tests -name *.cc)
TESTS_OBJ = $(TESTS_SRC:%.cc=obj/%.o) 

SRC = $(shell find src/ -name *.cc)
OBJ = $(SRC:%.cc=obj/%.o)


//FLAGS = -std=c++11 -O3 -Wall -Wextra -Wpedantic -DNDEBUG -Wno-deprecated-register -ftemplate-backtrace-limit=0 
FLAGS = -std=c++11 -O3  -g -Wall -Wextra -Wpedantic -Wno-deprecated-register
LIBRARIES = -L$(K2TREE)/lib -L$(K2TREE)/dacs -lcds -lboost_filesystem\
            -lboost_system -lk2tree\
						-ldacs -lboost_program_options


.PHONY: clean style test all

all: bison flex $(OBJ) test 

bison: parsing/parser.yy
	@echo " [GEN] Generating Bison files"
	@bison -o src/parser.cc --defines=include/parser.h parsing/parser.yy
	@mv src/location.hh include/location.h
	@mv src/position.hh include/position.h
	@mv src/stack.hh include/stack.h
	@sed -i -r 's/"(stack|location|position).hh"/<\1.h>/g'\
				include/parser.h include/position.h include/stack.h include/location.h

flex: parsing/scanner.ll
	@echo " [GEN] Generating scanner.cc"
	@flex -o src/scanner.cc parsing/scanner.ll

# TEST
test: bin/test

bin/test: $(TESTS_OBJ) $(OBJ)
	@echo " [LNK] Linking test"
	@$(CXX) -isystem $(GTEST)/include $(TESTS_OBJ) -lpthread $(OBJ) $(LIBRARIES)\
					-lgtest -o bin/test

obj/tests/%.o: tests/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -isystem $(GTEST)/include $(INCLUDE) $(FLAGS) -c $< -o $@
# END TEST



# STYLE
style:
	@echo " [CHK] Cheking Code Style"
	@./cpplint.py --filter=-legal,-runtime/threadsafe_fn $(TESTS_SRC) $(HEADERS)
# END STYLE

# SRC
obj/src/%.o: src/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@
# END SRC


# CLEAN
clean : clean_regular clean_test

clean_test:
	@echo " [CLN] Cleaning test"
	@touch .dummy
	@rm $(shell find obj/tests -name *.o) .dummy
	@rm bin/test -f

clean_regular:
	@echo " [CLN] Cleaning src"
	@touch .dummy
	@rm $(shell find obj/src -name *.o) .dummy
# END CLEAN
