# Sanity
CLAGS += -W -Wall -Wextra -Wpedantic
# On types
CFLAGS += -Wwrite-strings -Wfloat-equal -Wconversion -Wvla
# On assumptions
CFLAGS += -Waggregate-return -Winline

CFLAGS += -std=c18
LDFLAGS += -pthread

fdr: fdr.o utils/fdr_utils.o utils/fibonacci.o utils/math_ops.o

%.s: CFLAGS += -O1 -masm=intel -fno-asynchronous-unwind-tables

.PHONY: debug
debug: CFLAGS += -g
debug: fdr

.PHONY: check
check: fdr
check:
	./test/test.bash

.PHONY: profile
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: fdr

.PHONY: clean
clean:
	$(RM) *.o */*.o fdr test/test-all
