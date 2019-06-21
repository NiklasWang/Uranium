TARGET  = libfswatch$(strip $(STLIB_EXT))
CURRDIR = $(shell pwd)

#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all:
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILE)
endif

define BUILE
	cd fswatch-master;   \
	chmod +x autogen.sh; \
	./autogen.sh;        \
	./configure CFLAGS=-fPIC CXXFLAGS=-fPIC; \
	make -j8 V=1;        \
	cp ./libfswatch/src/libfswatch/.libs/libfswatch.a $(CURRDIR)
endef

clean:
	cd fswatch-master/; \
	make celan;         \
	make distclean;     \
	rm -rf $(CURRDIR)/$(TARGET)

.PHONY: clean
