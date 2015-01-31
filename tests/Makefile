.PHONY:
include ../common.mk
INCLUDE=-I../include
CPPFLAGS=$(_CPPFLAGS) $(INCLUDE)
SUBDIRS=
HEAP_TEST_OBJS=test_heap_sort.o heap_sort_tvs.o
XMATH_TEST_OBJS=test_xmath.o xmath_tvs.o
PERMUTATION_TEST_OBJS=test_permutation.o permutation_tvs.o
COLORS_TEST_OBJS=test_colors.o
FILE_BUFFER_TEST_OBJS=test_file_buffer.o file_buffer_tvs.o
OBJS=$(HEAP_TEST_OBJS) $(XMATH_TEST_OBJS) $(PERMUTATION_TEST_OBJS) \
	$(COLORS_TEST_OBJS) $(FILE_BUFFER_TEST_OBJS)
TARGETS=test_heap_sort test_xmath test_permutation test_colors test_file_buffer
all: $(SUBDIRS) $(OBJS) $(TARGETS)
test_heap_sort: $(HEAP_TEST_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ ../src/commonc.a
test_xmath: $(XMATH_TEST_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ ../src/commonc.a
test_permutation: $(PERMUTATION_TEST_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ ../src/commonc.a
test_colors: $(COLORS_TEST_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^
test_file_buffer: $(FILE_BUFFER_TEST_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ ../src/commonc.a
clean:
	$(RM) $(RM_FLAGS) $(OBJS) $(TARGETS)
