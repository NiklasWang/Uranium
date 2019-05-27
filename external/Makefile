build:all


include $(MAKE_RULE)/submodule.make.rule

#$(error modules=$(MAKE_SUB_MODULES))

all:$(MAKE_SUB_MODULES)

clean:$(CLEAN_SUB_MODULES)
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: all clean $(MAKE_SUB_MODULES) $(CLEAN_SUB_MODULES)
