# Compiler and Linker
CC          := mipsel-openipc-linux-musl-gcc

# The Target Binary Program
TARGET      := ingenic-autonight

# Directories
BUILDDIR    := build
SOURCEDIR   := src
INCLUDEDIR  := include

# Source Files
SOURCES     := $(wildcard $(SOURCEDIR)/*.c)
OBJECTS     := $(SOURCES:$(SOURCEDIR)/%.c=$(BUILDDIR)/%.o)

# Flags, Libraries, and Includes
CFLAGS      := -Wall
INCLUDES    := -I$(INCLUDEDIR)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: clean
