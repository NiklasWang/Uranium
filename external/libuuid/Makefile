CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS) -Wno-cpp
ARFLAGS  = $(GLOBAL_ARFLAGS)

TARGET      = libuuid$(strip $(STLIB_EXT))
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

SHARED_LIBS =
STATIC_LIBS =

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

all: compile link $(MAKE_SUB_MODULES)

compile: $(objects) $(COMPILE_SUB_MODULES)

link: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^
	$(MAKE) $(LINK_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -rf $(TARGET) $(objects) $(dependence) *.d*

.PHONY: clean $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)
