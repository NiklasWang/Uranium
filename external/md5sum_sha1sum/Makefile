CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs

TARGET      = libmd5sum_sha1sum$(strip $(STLIB_EXT))
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS)
STATIC_LIBS = $(GLOBAL_STATIC_LIBS)

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^ 

clean:
	rm -rf $(TARGET) $(objects) $(dependence)

.PHONY: all clean
