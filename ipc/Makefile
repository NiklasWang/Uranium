CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)

TARGET      = libipc$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libthreads libwebsocket libcore
STATIC_LIBS = $(GLOBAL_STATIC_LIBS)

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean