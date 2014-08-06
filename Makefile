CXX = clang++

K2TREE = ../k2tree

HEADERS = $(shell find include tests -name *.h)

INCLUDE = -Iinclude/ -I$(K2TREE)/include

BISON_SRC = src/parsing/parser.cc
FLEX_SRC = src/parsing/scanner.cc

TESTS_SRC = $(shell find tests -name *.cc)
TESTS_OBJ = $(TESTS_SRC:%.cc=obj/%.o) 

SRC = $(sort $(shell find src/ -name *.cc) $(BISON_SRC) $(FLEX_SRC))
OBJ = $(patsubst %.cc,obj/%.o,$(SRC))

EXE = $(shell find exe/ -name *.cc -exec basename {} \;)
EXE_OBJ = $(patsubst %.cc,obj/exe/%.o,$(EXE))
BIN = $(EXE:%.cc=bin/%)


FLAGS = -std=c++11 -O3 -Wall -Wextra -Wpedantic -DNDEBUG -Wno-deprecated-register
#FLAGS = -std=c++11 -O3  -g -Wall -Wextra -Wpedantic -Wno-deprecated-register

LIBRARIES = -L$(K2TREE)/lib -L$(K2TREE)/dacs\
						-lcds\
						-lk2tree\
						-ldacs\
            -lboost_system \
						-lboost_filesystem\
						-lboost_program_options\
						-lboost_timer\
						-lkyotocabinet

PROFILE=-lprofiler


.PHONY: clean style test all bin
.SECONDARY:

all: test bin

bin: $(BIN)

# EXE
bin/%: obj/exe/%.o $(OBJ)
	@echo " [LNK] Linking $@"
	@$(CXX) $(FLAGS)  $< $(OBJ) $(LIBRARIES) $(PROFILE) -o $@
# END EXE

bison: $(BISON_SRC)
$(BISON_SRC): parsing/parser.yy
	@echo " [GEN] Generating Bison files"
	@bison -o src/parsing/parser.cc --defines=include/parsing/parser.h parsing/parser.yy
	@mv src/parsing/location.hh include/parsing/location.h
	@mv src/parsing/position.hh include/parsing/position.h
	@mv src/parsing/stack.hh include/parsing/stack.h
	@sed -i -r 's/"(parser|stack|location|position).(hh|h)"/<parsing\/\1.h>/g'\
				include/parsing/parser.h include/parsing/position.h\
				include/parsing/stack.h include/parsing/location.h\
				$(BISON_SRC)

flex: $(FLEX_SRC)
$(FLEX_SRC): parsing/scanner.ll
	@echo " [GEN] Generating scanner.cc"
	@flex -o $(FLEX_SRC) parsing/scanner.ll

# TEST
test: bin/test
bin/test: $(TESTS_OBJ) $(OBJ)
	@echo " [LNK] Linking test"
	@$(CXX) $(TESTS_OBJ) -lpthread $(OBJ) $(LIBRARIES)\
					-lgtest -o bin/test
# END TEST



# STYLE
style:
	@echo " [CHK] Cheking Code Style"
	@./cpplint.py --filter=-legal,-runtime/threadsafe_fn $(TESTS_SRC) $(HEADERS)
# END STYLE

# SRC
obj/%.o: %.cc bison
	@echo " [C++] Compiling $<"
	@$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@
# END SRC


# CLEAN
clean : clean_regular clean_test clean_bin clean_bison clean_flex

clean_bin:
	@echo " [CLN] Cleaning binaries"
	@touch .dummy
	@rm -f bin/* .dummy

clean_test:
	@echo " [CLN] Cleaning test"
	@touch .dummy
	@rm $(shell find obj/tests -name *.o) .dummy
	@rm bin/test -f

clean_regular:
	@echo " [CLN] Cleaning src"
	@touch .dummy
	@rm $(shell find obj/src -name *.o) .dummy

clean_bison:
	@echo " [CLN] Cleaning parsing"
	@touch .dummy
	@rm -f include/parsing/location.h\
			include/parsing/position.h\
		  include/parsing/stack.h\
		  include/parsing/parser.h\
			src/parsing/parser.cc\
			.dummy
clean_flex:
	@echo " [CLN] Cleaning flex"
	@rm -f $(FLEX_SRC)
# END CLEAN
