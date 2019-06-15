include $(MAKE_RULE)/precheck.make.rule

ifeq ($(strip $(ISCYGWIN)), y)

  CXX      = $(GLOBAL_CXX)
  CXXFLAGS = $(GLOBAL_CXXFLAGS)
  LDFLAGS  = $(GLOBAL_LDFLAGS) -shared

  TARGET      = libstack_this$(strip $(DYLIB_EXT))
  sources     = stack_this.cpp
  objects     = $(sources:.cpp=.o)
  dependence := $(sources:.cpp=.d)

  -include $(dependence)

  include $(MAKE_RULE)/dependency.make.rule

  all: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

  clean:
	rm -f $(TARGET) $(objects) $(dependence)

else

  all:

  clean:

endif

.PHONY: clean