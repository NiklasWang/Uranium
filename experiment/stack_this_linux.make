CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = stack_this$(strip $(EXE_EXT))
sources     = stack_this.cpp stack_this_main.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)
