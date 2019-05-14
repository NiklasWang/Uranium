CXX     = $(GLOBAL_CXX)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)

TARGET      = lambda_test
sources     = LambdaTest.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean