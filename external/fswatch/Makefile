TARGET  = libfswatch$(strip $(DYLIB_EXT))
CURRDIR = $(shell pwd)

ifeq ($(strip $(ISCYGWIN)), y)
  FILENAME = cygfswatch-11.dll
else
  FILENAME = libfswatch-11.dll
endif

#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILE)
endif

define BUILE
	cd fswatch-master;   \
	chmod +x autogen.sh; \
	./autogen.sh;        \
	./configure CFLAGS=-fPIC CXXFLAGS=-fPIC; \
	make -j8 V=1;        \
	cp ./libfswatch/src/libfswatch/.libs/$(FILENAME) $(CURRDIR)/$(TARGET)
endef

clean:
	cd fswatch-master/; \
	make celan;         \
	make distclean;     \
	rm -rf $(CURRDIR)/$(TARGET)

.PHONY: clean
