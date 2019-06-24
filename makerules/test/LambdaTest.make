CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = lambda_test$(strip $(EXE_EXT))
sources     = LambdaTest.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean