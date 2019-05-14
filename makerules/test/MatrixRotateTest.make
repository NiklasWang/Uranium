CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)

TARGET      = matrix_rotate
sources     = MatrixRotateTest.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

include $(dependence)

include $(MAKE_RULE)/dependency.makerule

all: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean