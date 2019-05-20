CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = libsp.a
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build: all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES))

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^ 

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean