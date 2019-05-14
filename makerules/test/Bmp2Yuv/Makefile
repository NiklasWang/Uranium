CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS) -lm

TARGET      = bmp2yuv
sources     = Bmp2Yuv.c Bmp2YuvTester.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

include $(dependence)

include $(MAKE_RULE)/dependency.makerule

all: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)
	rm -f *.nv21

.PHONY: clean