CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = test_uuid
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

build:all

CFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES)) -I./ -I$(ROOT_DIR)/external/libuuid
LDFLAGS  += -L$(ROOT_DIR)/external/libuuid -luuid

-include $(dependence)
include $(MAKE_RULE)/dependency.make.rule


all:$(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET) $(objects) $(dependence) *.d.*

.PHONY: build clean
