CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = memory_tester
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build: all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES))
LDFLAGS  += -pthread
LDFLAGS  += -L $(ROOT_DIR)/log -Wl,-Bstatic -llog
LDFLAGS  += -L $(ROOT_DIR)/memory -Wl,-Bdynamic -lmemory

include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean