CXX      = $(GLOBAL_CXX)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES) socket

TARGET      = client_tester
sources     = client_tester.cpp
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build: all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES))
LDFLAGS  += -L $(ROOT_DIR)/log -Wl,-Bstatic -llog
LDFLAGS  += -L $(ROOT_DIR)/utils -lutils -Wl,-Bdynamic
LDFLAGS  += -L $(ROOT_DIR)/socket -lsocket

include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean

