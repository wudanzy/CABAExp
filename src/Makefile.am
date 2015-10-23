#USER_DIR = $(top_srcdir)/src
#TEST_DIR = $(top_srcdir)/test
#GTEST_DIR = $(top_srcdir)/gtest
#LIB_DIR = $(top_srcdir)/lib

CC = g++
VPATH = $(top_srcdir):$(top_srcdir)/wudanzy/trie:$(top_srcdir)/wudanzy/measure
#VPATH = $(top_srcdir)/include:$(top_srcdir)/include/wudanzy/trie:$(top_srcdir)/include/wudanzy/measure:$(USER_DIR):$(TEST_DIR):$(GTEST_DIR):$(LIB_DIR)
#TESTS = allocation_unittest.cc prefix_unittest.cc segment_unittest.cc
AM_CXXFLAGS = -I$(top_srcdir)/ -std=c++11 

#AM_CXXFLAGS += -g -lsqlite3 -Wall -Wextra 

noinst_PROGRAMS = aggregate
aggregate_SOURCES = aggregate.cpp prefix.cpp
include_HEADERS = iterator.h trie.h error.h node.h prefix.h 

