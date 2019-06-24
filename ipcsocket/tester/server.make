CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS) -pthread

TARGET      = server_tester$(strip $(EXE_EXT))
sources     = server_tester.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libipcsocket
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

