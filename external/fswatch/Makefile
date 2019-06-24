TARGET  = libfswatch$(strip $(DYLIB_EXT))
CURRDIR = $(shell pwd)

ifeq ($(strip $(ISCYGWIN)), y)
  FILENAME = cygfswatch-11.dll
else
  FILENAME = libfswatch.so*
endif

#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all:
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILE)
endif
ifeq ($(strip $(ISCYGWIN)), y)
	$(call CRYWIN_INSTALL)
else
	$(call LINUX_INSTALL)
endif

define BUILE
	cd fswatch-master;   \
	chmod +x autogen.sh; \
	./autogen.sh;        \
	./configure CFLAGS=-fPIC CXXFLAGS=-fPIC; \
	make -j8 V=1;       
endef

define CRYWIN_INSTALL
	fswatch-master;   \
	cp -ar  ./libfswatch/src/libfswatch/.libs/$(FILENAME) $(CURRDIR)/$(TARGET)
endef 

define LINUX_INSTALL
	cd fswatch-master;   \
	cp -ar ./libfswatch/src/libfswatch/.libs/${FILENAME} $(CURRDIR)/
endef

clean:
	cd fswatch-master/; \
	make celan;         \
	make distclean;     \
	rm -rf $(CURRDIR)/$(TARGET)*

.PHONY: clean
