targets = $(shell find . -name '*.make')

all:
	$(patsubst %, make -f %;, $(targets))

clean:
	$(patsubst %, make clean -f %;, $(targets))
