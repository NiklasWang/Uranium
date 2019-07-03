CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS)

TARGET      = libutils$(strip $(STLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = liblog
STATIC_LIBS = libsp

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

all: compile link $(MAKE_SUB_MODULES)

compile: $(objects) $(COMPILE_SUB_MODULES)

link: $(objects)
	$(AR) $(ARFLAGS) $(TARGET) $(objects)
	$(MAKE) $(LINK_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)