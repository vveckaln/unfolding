CC_FILES    := $(wildcard src/*.cc)
BIN_FILES   := $(wildcard bin/*.cc)
EXECUTABLES := $(notdir $(BIN_FILES:.cc=))
OBJ_FILES   := $(addprefix obj/,$(notdir $(CC_FILES:.cc=.o)))
CC_FLAGS    := -g
REPO        := /afs/cern.ch/work/v/vveckaln/private/CompoundHistoLib
ROOUNFOLD   := /afs/cern.ch/work/v/vveckaln/private/RooUnfold
#libRooUnfold.so
all: $(EXECUTABLES) 

# is -pthread -lm -ldl -rdynamic needed?

$(EXECUTABLES): %: bin/%.cc $(OBJ_FILES)
	g++ -Wall  `root-config --libs --cflags` -g -o $@ $^ -I include -I $(REPO)/interface  -L${REPO}/lib -lCompoundHisto  -Wl,-rpath,${REPO}/lib -I $(ROOUNFOLD)/src -L${ROOUNFOLD} -lRooUnfold  -Wl,-rpath,${ROOUNFOLD} -I$(CFAT)/interface -L$(CFAT)/lib -lcfat -Wl,-rpath,$(CFAT)/lib -L`root-config --libdir` -lUnfold -pthread -lm -ldl -rdynamic  -Wl,-rpath,`root-config --libdir`

obj/%.o: src/%.cc
	g++ -Wall `root-config --libs --cflags` $(CC_FLAGS) -std=c++14 -c -o $@ $< -I include -I $(REPO)/interface -I $(ROOUNFOLD)/src -I$(CFAT)/interface


.PHONY: clean

clean:
	for f in $(wildcard obj/*); do rm $$f; done
	find . -maxdepth 1 -type f -perm +a=x -print0 | xargs -0 -I {} rm {}
