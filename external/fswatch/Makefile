
all:
	cd fswatch-master/; \
	./autogen.sh; \
	./configure; \
	make -j 8; \
	cp -ar ./libfswatch/src/libfswatch/.libs/libfswatch.a ../../../release/


clean:
	cd fswatch-master/; \
	make celan; \
	make distclean;

.PHONY: clean
