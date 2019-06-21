TARGET=libfswatch$(strip $(STLIB_EXT))

#exist = $(shell if [ -f ./fswatch-master/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all:
ifneq ($(TARGET), $(notdir $(wildcard ./fswatch-master/$(TARGET))))
	$(call BUILE)
endif

#ifeq ($(exist), notexist)
#	echo "test"
#	$(call BUILE)
#endif

define BUILE
	cd fswatch-master/; \
	chmod +x autogen.sh; \
	./autogen.sh; \
	./configure CFLAGS=-fPIC CXXFLAGS=-fPIC; \
	make -j8 V=1; \
	cp -ar ./libfswatch/src/libfswatch/.libs/libfswatch.a ./
endef

clean:
	cd fswatch-master/; \
	make celan; \
	make distclean; \
	rm -rf $(TARGET) 

.PHONY: clean
