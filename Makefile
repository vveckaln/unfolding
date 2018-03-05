CC_FILES    := $(wildcard src/*.cc)
BIN_FILES   := $(wildcard bin/*.cc)
EXECUTABLES := $(notdir $(BIN_FILES:.cc=))
OBJ_FILES   := $(addprefix obj/,$(notdir $(CC_FILES:.cc=.o)))
CC_FLAGS    := -g

all: $(EXECUTABLES) 

obj/%.o: src/%.cc
	g++ `root-config --libs --cflags` $(CC_FLAGS) -c -o $@ $< -I include


%: bin/%.cc $(OBJ_FILES)
	g++ `root-config --libs --cflags` -g -o $@ $^ -I include


.PHONY: clean

clean:
	rm obj/*