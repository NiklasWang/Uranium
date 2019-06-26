CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS)

TARGET      = libaes$(strip $(STLIB_EXT))
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

SHARED_LIBS =
STATIC_LIBS =

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^

clean:
	rm -rf $(TARGET) $(objects) $(dependence) *.d*

.PHONY: all clean
