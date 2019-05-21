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
LIB_EXT   = $(if $(findstring CYGWIN, $(shell uname)), .dll, .so)
EXE_EXT   = $(if $(findstring CYGWIN, $(shell uname)), .exe, )

build: all

exclude_dirs  = out
compile_order = log utils memory threads

export
unexport exclude_dirs compile_order

include $(MAKE_RULE)/submodule.make.rule

include $(MAKE_RULE)/color.print.make.rule

all: $(MAKE_SUB_MODULES)
	@echo -e $(SUCCEED_COLOR)"Compile done."$(RESTORE_COLOR)

clean: $(CLEAN_SUB_MODULES)
	@echo -e $(FINISH_COLOR)"Clean done."$(RESTORE_COLOR)

.PHONY: build all clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)