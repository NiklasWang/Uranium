include $(MAKE_RULE)/64bit.32bit.check.make.rule

ifeq ($(BUILD_32BIT), y)

  CXX      = $(GLOBAL_CXX)
  CXXFLAGS = $(GLOBAL_CXXFLAGS) -m32
  LDFLAGS  = $(GLOBAL_LDFLAGS)

  TARGET      = stack_esp_ebp_32bit
  sources     = stack_esp_ebp.cpp
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

.PHONY: all clean