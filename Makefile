SHELL=/bin/bash

GLOBAL_CROSS     =
GLOBAL_CC        = $(GLOBAL_CROSS)gcc
GLOBAL_CXX       = $(GLOBAL_CROSS)g++
GLOBAL_LD        = $(GLOBAL_CROSS)ld
GLOBAL_AR        = $(GLOBAL_CROSS)ar
GLOBAL_CFLAGS    = -std=c99 -Wall -fPIC
GLOBAL_CXXFLAGS  = -std=c++11 -Wall -fPIC

GLOBAL_LDFLAGS   =
GLOBAL_ARFLAGS   = rcs
GLOBAL_MAKE      = $(MAKE)
GLOBAL_MAKEFLAGS = SHELL=$(SHELL)
GLOBAL_STATIC_LIBS = libutils libsp
GLOBAL_SHARED_LIBS = liblog libmemory

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
include $(MAKE_RULE)/multithread.compile.make.rule

GLOBAL_MAKEFLAGS +=

ifeq ($(strip $(ISCYGWIN)), y)
  DYLIB_EXT = .dll
  STLIB_EXT = .a
  EXE_EXT   = .exe
  GLOBAL_CFLAGS   += -D_CYGWIN_COMPILE_
  GLOBAL_CXXFLAGS += -D_CYGWIN_COMPILE_
  GLOBAL_CXXFLAGS := -std=gnu++11 $(filter-out -std=c++11, $(GLOBAL_CXXFLAGS))
endif

build: all

exclude_dirs  = cygwin gui out release linux_release

link_order  = log utils memory threads external encrypt ipcsocket websocket
link_order += monitor transmission makerules experiment cmdline core ipc main

export
unexport link_order

include $(MAKE_RULE)/submodule.make.rule

include $(MAKE_RULE)/prepare.env.make.rule
include $(MAKE_RULE)/color.print.make.rule

all: prepare $(COMPILE_SUB_MODULES)
	$(MAKE) all $(GLOBAL_MAKEFLAGS) -C gui
	make $(LINK_SUB_MODULES)
	$(MAKE) release
	@echo -e $(SUCCEED_COLOR)"Project $(PROJNAME) $(VERSION) build on $(PLATFORM) succeed."$(RESTORE_COLOR)

clean: $(CLEAN_SUB_MODULES)
	rm -f `find $(ROOT_DIR) -type f -name "*.d.*"`
	@echo -e $(FINISH_COLOR)"Project $(PROJNAME) $(VERSION) all cleaned."$(RESTORE_COLOR)

clean-cache:
	rm -f `find $(ROOT_DIR) -type f -name "*.make.cache"`


include $(MAKE_RULE)/find.all.modules.make.rule

rel: release

release:
	if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi;
	fileNum=$$(find $(MODULE_DIRS) -type f -name "*$(strip $(DYLIB_EXT))" | wc -l);  \
  if [ "$$fileNum" -ne "0" ]; then                                                 \
    cp -f `find $(MODULE_DIRS) -type f -name "*$(strip $(DYLIB_EXT))"` $(BIN_DIR); \
  fi;
	cp -f `find $(MODULE_DIRS) -type f -name "$(PROJNAME)$(EXE_EXT)"` $(BIN_DIR);
	@echo -e $(FINISH_COLOR)"All libraries $(DYLIB_EXT) have been copied to $(BIN_DIR)."$(RESTORE_COLOR)

linux_release:
	find . -path './linux_release' -name "*.so" -exec cp -ar {} linux_release/ \;
	cp -ar external/fswatch/lib* linux_release/
	cp -ar core/tester/server_tester linux_release/

.PHONY: build all clean install rel release linux_release $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)
