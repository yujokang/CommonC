.PHONY:
include ../common.mk
INCLUDE=-I../include
CPPFLAGS=$(_CPPFLAGS) $(INCLUDE)
SUBDIRS=
HEAP_TEST_OBJS=test_heap_sort.o heap_sort_tvs.o
OBJS=$(HEAP_TEST_OBJS)
TARGETS=test_heap_sort
all: $(SUBDIRS) $(OBJS) $(TARGETS)
test_heap_sort: $(HEAP_TEST_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ ../src/commonc.a
clean:
	$(RM) $(RM_FLAGS) $(OBJS) $(TARGETS)
