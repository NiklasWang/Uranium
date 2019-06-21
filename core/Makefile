CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = libcore$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libtransmission libthreads libwebsocket
STATIC_LIBS = $(GLOBAL_STATIC_LIBS) libencrypt libmonitor libmd5sum_sha1sum libuuid libfswatch libutils 

LDFLAGS+=-L../monitor/lib  

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

include $(MAKE_RULE)/submodule.make.rule

lib: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

all: lib $(MAKE_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
