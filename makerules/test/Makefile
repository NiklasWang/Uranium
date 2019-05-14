build: all

include $(MAKE_RULE)/module.make.rule

include $(MAKE_RULE)/submodule.make.rule

all: MAKE_MODULES $(MAKE_SUB_MODULES)

clean: CLEAN_MODULES $(CLEAN_SUB_MODULES)

.PHONY: build all clean MAKE_MODULES CLEAN_MODULES $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)