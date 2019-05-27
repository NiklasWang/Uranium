SHELL=/bin/bash

GLOBAL_CROSS     =
GLOBAL_CC        = $(GLOBAL_CROSS)gcc
GLOBAL_CXX       = $(GLOBAL_CROSS)g++
GLOBAL_LD        = $(GLOBAL_CROSS)ld
GLOBAL_AR        = $(GLOBAL_CROSS)ar
GLOBAL_CFLAGS    = -std=c99 -Wall -fPIC
GLOBAL_CXXFLAGS  = -std=gnu++11 -Wall -fPIC

GLOBAL_LDFLAGS   =
GLOBAL_ARFLAGS   =
GLOBAL_MAKE      = make
GLOBAL_MAKEFLAGS =
GLOBAL_INCLUDES  = . memory threads log $(shell find utils -type d)

ROOT_DIR  = $(shell pwd)
MAKE_RULE = $(ROOT_DIR)/makerules
OUT_DIR   = $(ROOT_DIR)/out
OBJ_DIR   = $(OUT_DIR)/obj
BIN_DIR   = $(OUT_DIR)/bin

DYLIB_EXT = .so
STLIB_EXT = .a
EXE_EXT   =

include $(MAKE_RULE)/precheck.make.rule
include $(MAKE_RULE)/project.info.make.rule

ifeq ($(strip $(ISCYGWIN)),y)
  DYLIB_EXT = .dll
  STLIB_EXT = .a
  EXE_EXT   = .exe
endif

build: all

exclude_dirs  = out
compile_order = log utils memory threads external encrypt

export
unexport exclude_dirs compile_order

include $(MAKE_RULE)/submodule.make.rule

include $(MAKE_RULE)/color.print.make.rule

all: $(MAKE_SUB_MODULES)
	@echo -e $(SUCCEED_COLOR)"Project $(PROJNAME) $(VERSION) build on $(PLATFORM) succeed."$(RESTORE_COLOR)

clean: $(CLEAN_SUB_MODULES)
	@echo -e $(FINISH_COLOR)"Project $(PROJNAME) $(VERSION) all cleaned."$(RESTORE_COLOR)

.PHONY: build all clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
