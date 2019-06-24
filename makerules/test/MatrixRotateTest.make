CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS)
LDFLAGS = $(GLOBAL_LDFLAGS)

TARGET      = matrix_rotate$(strip $(EXE_EXT))
sources     = MatrixRotateTest.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean