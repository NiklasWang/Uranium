CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS)

TARGET      = libsp$(strip $(STLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = liblog
STATIC_LIBS = libutils

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean