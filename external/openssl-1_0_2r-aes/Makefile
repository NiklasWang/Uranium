CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = libopenssl_aes$(strip $(STLIB_EXT))
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

build: all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES))

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule
 
include $(MAKE_RULE)/submodule.make.rule

all: lib $(MAKE_SUB_MODULES)

lib: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^

clean: $(CLEAN_SUB_MODULES)
	rm -rf $(TARGET) $(objects) $(dependence) *.d.*

.PHONY: all clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
