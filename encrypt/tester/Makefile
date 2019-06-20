CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)

TARGET      = crypto_aes_tester
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

SHARED_LIBS = $(GLOBAL_SHARED_LIBS) libthreads
STATIC_LIBS = $(GLOBAL_STATIC_LIBS) libaes libmd5sum_sha1sum libuuid

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET) $(objects) $(dependence) *.d.*

.PHONY: clean
