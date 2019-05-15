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
GLOBAL_MAKEFLAGS =
GLOBAL_INCLUDES  = memory threads log utils

ROOT_DIR  = $(shell pwd)
MAKE_RULE = $(ROOT_DIR)/makerules
OUT_DIR   = $(ROOT_DIR)/out
OBJ_DIR   = $(OUT_DIR)/obj
BIN_DIR   = $(OUT_DIR)/bin

export

build: all

exclude_dirs = out

include $(MAKE_RULE)/submodule.make.rule

all: $(MAKE_SUB_MODULES)

clean: $(CLEAN_SUB_MODULES)

.PHONY: build all clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)