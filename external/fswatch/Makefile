include $(MAKE_RULE)/color.print.make.rule

TARGET  = libfswatch$(strip $(DYLIB_EXT))
CURRDIR = $(shell pwd)

ifeq ($(strip $(ISCYGWIN)), y)
  FILENAME = cygfswatch-11.dll
else
  FILENAME = libfswatch.so*
endif

#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
	@echo -e $(NOTICE_COLOR)"This module requires following packages, make sure they are installed in your system."$(RESTORE_COLOR)
	@echo -e $(NOTICE_COLOR)" - automake, autopoint, libtool, gettext, etc."$(RESTORE_COLOR)
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILD)
ifeq ($(strip $(ISCYGWIN)), y)
	$(call CYGWIN_INSTALL)
else
	$(call LINUX_INSTALL)
endif
endif

define BUILD
	cd fswatch-master;   \
	chmod +x autogen.sh; \
	./autogen.sh;        \
	./configure CFLAGS=-fPIC CXXFLAGS=-fPIC; \
	make -j8 V=1;       
endef

define CYGWIN_INSTALL
	cd fswatch-master; \
	cp -ar ./libfswatch/src/libfswatch/.libs/$(FILENAME) $(CURRDIR)/$(TARGET)
endef 

define LINUX_INSTALL
	cd fswatch-master; \
	cp -ar ./libfswatch/src/libfswatch/.libs/${FILENAME} $(CURRDIR)/
endef

clean:
	cd fswatch-master; \
	make clean;        \
	make distclean;    \
	rm -rf $(CURRDIR)/$(TARGET)*; \
	rm -rf $(CURRDIR)/*.dll

.PHONY: clean
