CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs

TARGET      = test_uuid
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS)
STATIC_LIBS = $(GLOBAL_STATIC_LIBS) libuuid

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all:$(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET) $(objects) $(dependence) *.d.*

.PHONY: clean
