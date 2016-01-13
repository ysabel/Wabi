TOP_SRC ?= .
-include $(TOP_SRC)/user.mk

PLATFORM ?=

CC = $(PLATFORM)gcc
CXX = $(PLATFORM)g++
CFLAGS = -Wall -g -Og
CXXFLAGS = -std=c++14 $(CFLAGS)

SRC_DIRS = $(TOP_SRC)
srcs := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
incs := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))
objs := $(patsubst %.c,%.o,$(srcs))
deps := $(patsubst %.c,%.d,$(srcs))

.PHONY:	clean

all:: wabi

clean::
	$(RM) -f wabi $(objs) $(deps)

wabi: $(objs)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.d:	%.c
	@echo "Building dependencies for $<..."
	@set -e; rm -f $@; \
	 $(CC) -M $(CFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

-include $(deps)
