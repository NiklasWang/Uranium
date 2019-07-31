include $(MAKE_RULE)/color.print.make.rule

TARGET  = librchive$(strip $(DYLIB_EXT))
CURRDIR = $(shell pwd)

ifeq ($(strip $(ISCYGWIN)), y)
  FILENAME = cygfswatch-11.dll
else
  FILENAME = libarchive.so*
endif

#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
	@echo -e $(NOTICE_COLOR)"This module requires following packages, make sure they are installed in your system."$(RESTORE_COLOR)
	@echo -e $(NOTICE_COLOR)" - automake, autopoint, libtool, gettext, etc."$(RESTORE_COLOR)
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILD)
endif
ifeq ($(strip $(ISCYGWIN)), y)
	$(call CYGWIN_INSTALL)
else
	$(call LINUX_INSTALL)
endif

define BUILD
	cd ;barchive-3.3.3   \
	chmod +x configure; \
	./configure CFLAGS=-fPIC -O3  CXXFLAGS=-fPIC -O3; \
	make -j8 V=1;       
endef

define CYGWIN_INSTALL
	cp -ar ./libarchive-3.3.3/.libs/$(FILENAME) $(CURRDIR)/$(TARGET)
endef 

define LINUX_INSTALL
	cp -ar ./libarchive-3.3.3/.libs/${FILENAME} $(CURRDIR)/
endef

clean:
	cd libarchive-3.3.3; \
	make clean;        \
	make distclean;    \
	rm -rf $(CURRDIR)/$(TARGET)*

.PHONY: clean
