# Target programs
programs := \
	queue_tester.x \
	uthread_hello.x \
	test_preempt.x \
	uthread_yield.x 

# User-level thread library
UTHREADLIB := libuthread
UTHREADPATH := ../$(UTHREADLIB)
libuthread := $(UTHREADPATH)/$(UTHREADLIB).a

# Default rule
all: $(programs)

# Avoid builtin rules and variables
MAKEFLAGS += -rR

# Don't print the commands unless explicitly requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

# Current directory
CUR_PWD := $(shell pwd)

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS	:= -Wall -Wextra -Werror
CFLAGS	+= -pipe
## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif
## Include path
CFLAGS 	+= -I$(UTHREADPATH)
## Dependency generation
CFLAGS	+= -MMD

# Linker options
LDFLAGS := -L$(UTHREADPATH) -luthread

# Application objects to compile
objs := $(patsubst %.x,%.o,$(programs))

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Rule for libuthread.a
$(libuthread): FORCE
	@echo "MAKE	$@"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH)

# Generic rule for linking final applications
%.x: %.o $(libuthread)
	@echo "LD	$@"
	$(Q)$(CC) -o $@ $< $(LDFLAGS)

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning rule
clean: FORCE
	@echo "CLEAN	$(CUR_PWD)"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH) clean
	$(Q)rm -rf $(objs) $(deps) $(programs)

# Keep object files around
.PRECIOUS: %.o
.PHONY: FORCE
FORCE:

