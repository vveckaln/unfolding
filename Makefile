SRCS = $(wildcard *.cc)
EXECS = $(SRCS:.cc=)

all: $(EXECS)

%: %.cc
	g++ -g -std=c++11 -I$(ROOTSYS)/include `root-config --glibs` $< -o $@