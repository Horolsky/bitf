#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
ifndef GTEST_DIR
GTEST_DIR = ../googletest/googletest
endif

CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -g -std=gnu++14 -Wall -Wextra -pedantic -pthread -fsanitize=address,undefined

# add new tests here
TESTS = bitf_gTest

SOURCE_DIR = src
# LIB_DIR = lib
TEST_DIR = test

KERNEL_NAME = $(shell uname -s)
MACHINE_NAME = $(shell uname -m)

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

.PHONY: all clean build test

# House-keeping build targets.
all : test

format: .clang-format
	clang-format -i -style=file src/* test/*

CPPCHECK = cppcheck
CHECKFLAGS = -q --error-exitcode=1
CHECKLOG = cppcheck.out.xml

$(CHECKLOG): 
	$(CPPCHECK) $(CHECKFLAGS) src/* test/* --xml >$@

check:
	cppcheck --enable=all --verbose src/* test/* --xml  2>$(CHECKLOG)

tidy: .clang-tidy
	clang-tidy --extra-arg-before=-xc++ --format-style=file src/* test/* 

clean :
	rm -fr $(TESTS) gtest.a gtest_main.a *.o *.out.xml

# Builds gtest.a and gtest_main.a.

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

bitf_gTest.o : $(TEST_DIR)/bitf_gTest.cpp $(SOURCE_DIR)/*.hpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(SOURCE_DIR) $(CXXFLAGS) -c $(TEST_DIR)/bitf_gTest.cpp

$(TESTS) : bitf_gTest.o gtest_main.a
	@echo "Building $@ for $(KERNEL_NAME) $(MACHINE_NAME)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@ -L. 

test: $(TESTS)
	./$(TESTS)
