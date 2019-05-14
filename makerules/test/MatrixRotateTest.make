CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)

TARGET      = matrix_rotate
sources     = MatrixRotateTest.c
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