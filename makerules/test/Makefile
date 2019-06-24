include $(MAKE_RULE)/module.make.rule

include $(MAKE_RULE)/submodule.make.rule

all: $(MAKE_MODULES) $(MAKE_SUB_MODULES)

compile: $(COMPILE_MODULES) $(COMPILE_SUB_MODULES)

link: $(LINK_MODULES) $(LINK_SUB_MODULES)

clean: $(CLEAN_MODULES) $(CLEAN_SUB_MODULES)

.PHONY: all clean compile link $(MAKE_SUB_MODULES) $(COMPILE_SUB_MODULES) $(LINK_SUB_MODULES) $(CLEAN_SUB_MODULES)