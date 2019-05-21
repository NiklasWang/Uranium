CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = libsocket$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build: all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES))
LDFLAGS  += -fPIC -shared -pthread
LDFLAGS  += -L $(ROOT_DIR)/log -Wl,-Bstatic -llog
LDFLAGS  += -L $(ROOT_DIR)/utils -lutils
LDFLAGS  += -L $(ROOT_DIR)/utils/sp -lsp -Wl,-Bdynamic

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

lib: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

all: lib $(MAKE_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)