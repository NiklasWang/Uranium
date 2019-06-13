include $(MAKE_RULE)/64bit.32bit.check.make.rule

ifeq ($(BUILD_64BIT), y)

  CXX      = $(GLOBAL_CXX)
  CXXFLAGS = $(GLOBAL_CXXFLAGS) -m64
  LDFLAGS  = $(GLOBAL_LDFLAGS)

  TARGET      = stack_rsp_rbp_64bit
  sources     = stack_rsp_rbp.cpp
  objects     = $(sources:.cpp=.o)
  dependence := $(sources:.cpp=.d)

  -include $(dependence)

  include $(MAKE_RULE)/dependency.make.rule

  all: $(objects)
	$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

  clean:
	rm -f $(TARGET) $(objects) $(dependence)

else

  all:

  clean:

endif

.PHONY: clean