SHELL=/bin/bash

GLOBAL_CROSS     =
GLOBAL_CC        = $(GLOBAL_CROSS)gcc
GLOBAL_CXX       = $(GLOBAL_CROSS)g++
GLOBAL_LD        = $(GLOBAL_CROSS)ld
GLOBAL_AR        = $(GLOBAL_CROSS)ar
GLOBAL_CFLAGS    = -std=c99 -Wall -fPIC
GLOBAL_CXXFLAGS  = -std=c++11 -Wall -fPIC

GLOBAL_LDFLAGS   = -pthread
GLOBAL_ARFLAGS   =
GLOBAL_MAKE      = make
GLOBAL_MAKEFLAGS =
GLOBAL_STATIC_LIBS = liblog libutils libsp
GLOBAL_SHARED_LIBS = libmemory

ROOT_DIR  = $(shell pwd)
MAKE_RULE = $(ROOT_DIR)/makerules
OUT_DIR   = $(ROOT_DIR)/out
OBJ_DIR   = $(OUT_DIR)/obj
BIN_DIR   = $(ROOT_DIR)/release

DYLIB_EXT = .so
STLIB_EXT = .a
EXE_EXT   =

include $(MAKE_RULE)/precheck.make.rule
include $(MAKE_RULE)/project.info.make.rule

ifeq ($(strip $(ISCYGWIN)),y)
  DYLIB_EXT = .dll
  STLIB_EXT = .a
  EXE_EXT   = .exe
  GLOBAL_CFLAGS   += -D_CYGWIN_COMPILE_
  GLOBAL_CXXFLAGS += -D_CYGWIN_COMPILE_
  GLOBAL_CXXFLAGS := -std=gnu++11 $(filter-out -std=c++11, $(GLOBAL_CXXFLAGS))
endif

build: all

exclude_dirs  = cygwin gui out release
compile_order = log utils memory threads external encrypt transmission monitor core

export
unexport exclude_dirs compile_order

include $(MAKE_RULE)/submodule.make.rule

include $(MAKE_RULE)/color.print.make.rule

all: $(MAKE_SUB_MODULES)
	$(GLOBAL_MAKE) $(GLOBAL_MAKEFLAGS) -C gui all
	@echo -e $(SUCCEED_COLOR)"Project $(PROJNAME) $(VERSION) build on $(PLATFORM) succeed."$(RESTORE_COLOR)

clean: $(CLEAN_SUB_MODULES)
	rm -f `find $(ROOT_DIR) -type f -name "*.d*"`
	@echo -e $(FINISH_COLOR)"Project $(PROJNAME) $(VERSION) all cleaned."$(RESTORE_COLOR)

install:
	if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi;
	fileNum=$$(find $(ROOT_DIR) -path $(BIN_DIR) -prune -type f -o -name "*$(strip $(DYLIB_EXT))" | wc -l);  \
  if [ "$$fileNum" -ne "0" ]; then                                                                         \
    cp -f `find $(ROOT_DIR) -path $(BIN_DIR) -prune -type f -o -name "*$(strip $(DYLIB_EXT))"` $(BIN_DIR); \
  fi;
	@echo -e $(FINISH_COLOR)"All libraries $(DYLIB_EXT) $(STLIB_EXT) have been copied to $(BIN_DIR)."$(RESTORE_COLOR)

.PHONY: build all clean install $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
