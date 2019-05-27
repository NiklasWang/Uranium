CC       = $(GLOBAL_CC)
AR       = $(GLOBAL_AR)
CFLAGS   = $(GLOBAL_CFLAGS)
ARFLAGS  = $(GLOBAL_ARFLAGS) rcs
INCLUDES = $(patsubst $(ROOT_DIR)/%, %, $(shell pwd)) $(GLOBAL_INCLUDES)

TARGET      = libuuid$(strip $(STLIB_EXT))
sources     = $(wildcard *.c)
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

build:all

CFLAGS += $(patsubst %, -I$(ROOT_DIR)/%, $(INCLUDES)) -I./
#LDFLAGS  += -L $(ROOT_DIR)/external/coreutils-5_0-md5sum_sha1sum/ -Wl,-Bstatic -lcrypto_md5_sha1
#LDFLAGS  += -L $(ROOT_DIR)/external/openssl-1_0_2r-aes -lopenssl_aes -Wl,-Bdynamic

-include $(dependence)
include $(MAKE_RULE)/dependency.make.rule
 include $(MAKE_RULE)/submodule.make.rule

all:lib $(MAKE_SUB_MODULES)

lib:$(objects)
	$(AR) $(ARFLAGS) $(TARGET) $^

clean:$(CLEAN_SUB_MODULES)
	rm -rf $(TARGET) $(objects) $(dependence) *.d.*

.PHONY: build clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
