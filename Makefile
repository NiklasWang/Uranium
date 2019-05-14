GLOBAL_CROSS     =
GLOBAL_CC        = $(GLOBAL_CROSS)gcc
GLOBAL_CXX       = $(GLOBAL_CROSS)g++
GLOBAL_LD        = $(GLOBAL_CROSS)ld
GLOBAL_AR        = $(GLOBAL_CROSS)ar
GLOBAL_CFLAGS    = -std=c99 -Wall
GLOBAL_CXXFLAGS  = -std=c++11 -Wall
GLOBAL_LDFLAGS   =
GLOBAL_ARFLAGS   =
GLOBAL_MAKE      = make
GLOBAL_MAKEFLAGS = -j6

ROOT_DIR  = $(shell pwd)
MAKE_RULE = $(ROOT_DIR)/makerules
OUT_DIR   = $(ROOT_DIR)/out
OBJ_DIR   = $(OUT_DIR)/obj
BIN_DIR   = $(OUT_DIR)/bin

export

exclude_dirs = out

dirs := $(shell find . -maxdepth 1 -type d)
dirs := $(basename $(patsubst ./%, %, $(dirs)))
MAKE_SUB_MODULES  := $(filter-out $(exclude_dirs), $(dirs))
CLEAN_SUB_MODULES := $(addprefix clean_, $(MAKE_SUB_MODULES))

$(MAKE_SUB_MODULES):
	$(GLOBAL_MAKE) $(GLOBAL_MAKEFLAGS) -C $@ all

$(CLEAN_SUB_MODULES):
	$(GLOBAL_MAKE) $(GLOBAL_MAKEFLAGS) -C $@ clean

all: $(MAKE_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)

.PHONY: all clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)