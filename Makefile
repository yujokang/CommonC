.PHONY:src tests
include common.mk
INCLUDE=-Iinclude
CPPFLAGS=$(_CPPFLAGS) $(INCLUDE)
SUBDIRS=src tests
OBJS=
TARGETS=commonc.a
all: $(SUBDIRS) $(OBJS) $(TARGETS)
commonc.a:
	cp src/commonc.a commonc.a
src:
	$(MAKE) -C src
tests:
	$(MAKE) -C tests
clean:
	$(RM) $(RM_FLAGS) $(OBJS) $(TARGETS)
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
