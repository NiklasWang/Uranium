CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = libtransmission$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libthreads
STATIC_LIBS = $(GLOBAL_STATIC_LIBS)

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

lib: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

all: lib $(MAKE_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -rf $(TARGET) $(objects) $(dependence) *.d.*

.PHONY: clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
