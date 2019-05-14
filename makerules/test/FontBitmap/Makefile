CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS) -DBUILD_LINUX_X86_64 -I/usr/include/freetype2/freetype -I/usr/include/freetype2
LDFLAGS = $(GLOBAL_LDFLAGS) -L/usr/lib/x86_64-linux-gnu/ -lfreetype

TARGET      = font_tester
sources     = FontBitmapTest.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

build: all

include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: build clean
