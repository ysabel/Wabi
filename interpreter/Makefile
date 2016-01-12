TOP_SRC ?= .
-include $(TOP_SRC)/user.mk

CXXFLAGS = -std=c++14 -Wall -g -Og

SRC_DIRS = $(TOP_SRC)
srcs := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cc))
incs := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))
objs := $(patsubst %.cc,%.o,$(srcs))
deps := $(patsubst %.cc,%.d,$(srcs))

.PHONY:	clean

all:: wabi

clean::
	$(RM) -f wabi $(objs) $(deps)

wabi: $(objs)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.d:	%.cc
	@echo "Building dependencies for $<..."
	@set -e; rm -f $@; \
	 $(CXX) -M $(CXXFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

-include $(deps)
