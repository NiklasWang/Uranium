CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)

TARGET      = test_uuid$(strip $(EXE_EXT))
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) 
STATIC_LIBS = $(GLOBAL_STATIC_LIBS) libuuid

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

