# Sanity
CLAGS += -Wall -Wextra -Wpedantic
# On types
CFLAGS += -Wwrite-strings -Wfloat-equal -Wconversion -Wvla
# On assumptions
CFLAGS += -Waggregate-return -Winline

CFLAGS += -std=c17
LDFLAGS += -pthread

fdr: fdr.o utils/fdr_utils.o utils/fibonacci.o utils/math_ops.o utils/dec_to_hex.o utils/roman_to_hex.o

%.s: CFLAGS += -O1 -masm=intel -fno-asynchronous-unwind-tables

.PHONY: debug
debug: CFLAGS += -g
debug: fdr

.PHONY: check
check: test/test-all
	CK_EXCLUDE_TAGS=profile ./test/test-all

test/test-all: LDLIBS += -lcheck -lm -lrt -lsubunit -lpthread
test/test-all: test/test-all.o

.PHONY: profile
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: fdr

.PHONY: clean
clean:
	$(RM) *.o */*.o fdr test/test-all
