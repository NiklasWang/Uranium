CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)

TARGET      = lambda_test
sources     = LambdaTest.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build: all

include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean