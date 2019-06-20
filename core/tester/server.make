CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)

TARGET      = server_tester
sources     = server_tester.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libthreads libtransmission 
STATIC_LIBS = $(GLOBAL_STATIC_LIBS) libcore libencrypt libmonitor libmd5sum_sha1sum libuuid libfswatch libutils 

LDFLAGS+=-L../../monitor/lib  -pthread

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean

