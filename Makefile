CXX = g++
CXXFLAGS = -Wall -g -std=c++0x
INCLUDES=-I$(MESOS_INSTALL)/include
LIBRARY_PATH=$(MESOS_INSTALL)/lib
LDFLAGS += -L$(LIBRARY_PATH) -lmesos -lpthread 
CXXCOMPILE = $(CXX) $(INCLUDES) $(CXXFLAGS) -c -o $@
CXXLINK = $(CXX) $(INCLUDES) $(CXXFLAGS) -o $@
default: all
all: hello_mesos

hello_mesos: hello_framework.cpp
	$(CXXLINK) $< $(LDFLAGS) -lboost_regex -lcurl
%: %.cpp $(HEADERS)
	$(CXXLINK) $< $(LDFLAGS)
