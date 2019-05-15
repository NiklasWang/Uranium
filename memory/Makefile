CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)
INCLUDES = . $(ROOT_DIR) $(GLOBAL_INCLUDES)

TARGET      = libmemory.so
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build: all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES))
LDFLAGS  += -fPIC -shared

include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

all: $(objects) $(MAKE_SUB_MODULES)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean: $(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)