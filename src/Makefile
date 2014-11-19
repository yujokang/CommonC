.PHONY:
include ../common.mk
INCLUDE=-I../include
CPPFLAGS=$(_CPPFLAGS) $(INCLUDE)
SUBDIRS=
OBJS=data_structs.o
TARGETS=commonc.a
all: $(SUBDIRS) $(OBJS) $(TARGETS)
commonc.a: data_structs.o
	$(AR) $(AR_FLAGS) $@ $^
clean:
	$(RM) $(RM_FLAGS) $(OBJS) $(TARGETS)
