# Sanity
CLAGS += -Wall -Wextra -Wpedantic
# On types
CFLAGS += -Wwrite-strings -Wfloat-equal -Wconversion -Wvla
# On assumptions
CFLAGS += -Waggregate-return -Winline

CFLAGS += -std=c17 -pthread

fdr: fdr.o utils/fdr_utils.o

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
