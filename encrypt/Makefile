CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = crypto_aes_tester
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

build:all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES)) -I./
CXXFLAGS += -I$(ROOT_DIR)/external/md5sum_sha1sum -I$(ROOT_DIR)/external/aes
LDFLAGS  += -L $(ROOT_DIR)/external/md5sum_sha1sum/ -Wl,-Bstatic -lmd5sum_sha1sum
LDFLAGS  += -L $(ROOT_DIR)/external/aes -laes 
LDFLAGS  += -L$(ROOT_DIR)/log -llog -L$(ROOT_DIR)/utils -lutils -Wl,-Bdynamic

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all:$(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET) $(objects) $(dependence) *.d.* *.o

.PHONY: build clean
