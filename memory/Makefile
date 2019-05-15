CC = g++
CPPFLAGS =
LDFLAGS =
TARGET = tester
sources = $(wildcard *.cpp)
objects = $(sources:.cpp=.o)
dependence:=$(sources:.cpp=.d)

all: $(objects)
	$(CC) $^ -o $(TARGET)

include $(dependence)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean

clean:
	rm -f $(TARGET) $(objects) $(dependence)
