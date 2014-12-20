.PHONY:
include ../common.mk
INCLUDE=-I../include
CPPFLAGS=$(_CPPFLAGS) $(INCLUDE)
SUBDIRS=
OBJS=data_structs.o logger.o get_random.o xmath.o permutation.o
TARGETS=commonc.a
all: $(SUBDIRS) $(OBJS) $(TARGETS)
commonc.a: $(OBJS)
	$(AR) $(AR_FLAGS) $@ $^
clean:
	$(RM) $(RM_FLAGS) $(OBJS) $(TARGETS)
