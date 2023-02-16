# Default target
all: writer

# Cross-compilation support
ifeq ($(CROSS_COMPILE),)
CC := gcc
else
CC := $(CROSS_COMPILE)gcc
endif

# Source files
SRCS := writer.c

# Object files
OBJS := $(SRCS:.c=.o)

# Compiler flags
CFLAGS := -Wall

# Linker flags
LDFLAGS :=

# Targets
writer: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f writer $(OBJS)

