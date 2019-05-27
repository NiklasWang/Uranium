CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = uuid_test
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

build:all

CXXFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES)) -I./  -DHAVE_CONFIG_H
#LDFLAGS  += -L $(ROOT_DIR)/external/coreutils-5_0-md5sum_sha1sum/ -Wl,-Bstatic -lcrypto_md5_sha1
#LDFLAGS  += -L $(ROOT_DIR)/external/openssl-1_0_2r-aes -lopenssl_aes -Wl,-Bdynamic

-include $(dependence)
include $(MAKE_RULE)/dependency.make.rule


all:$(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET) $(objects) $(dependence) *.d.* *.o *.d

.PHONY: build clean
