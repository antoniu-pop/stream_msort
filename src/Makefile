INST_DIR=../../../install
LIB_DIR = $(INST_DIR)/lib
LIB64_DIR = $(INST_DIR)/lib64
GCC = /usr/bin/gcc
OPENSTREAM_GCC = $(INST_DIR)/bin/gcc
LIBWSTREAM_DF_LIB_DIR = $(LIB64_DIR)

CFLAGS = -Wall -std=c99 -O3 -ffast-math
LDFLAGS = -L$(LIB_DIR) -lm -rdynamic -Wl,-rpath,$(LIB_DIR)

TESTS = msort stream_msort openmp_msort
all: 	$(TESTS)
msort: msort.c
	$(GCC) $(CFLAGS) -o $@ $^

stream_%: LDFLAGS += -Wl,-rpath,$(LIBWSTREAM_DF_LIB_DIR) -L$(LIBWSTREAM_DF_LIB_DIR) -lwstream_df
stream_%: CFLAGS += -fopenmp
stream_%: stream_%.c
	$(OPENSTREAM_GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@

openmp_%:
openmp_%: CFLAGS += -fopenmp
openmp_%: openmp_%.c
	$(GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(TESTS) *.s *.out *.o
