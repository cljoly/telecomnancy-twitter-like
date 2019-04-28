#Compiler and Linker
CC          := gcc

#The Target Binary Program
TARGET      := client

#The Directories, Source, Objects, Binary
SRCDIR      := .
BUILDDIR    := obj
TARGETDIR   := .
SRCEXT      := c
OBJEXT      := o

#Flags and Libraries
CFLAGS      := -Wall -Werror -Wextra -Wformat -g $(shell pkg-config --cflags json-c)
LIB         := $(shell pkg-config --libs json-c)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

#Default Make
all: directories $(TARGET)

#test
test: all test.sh
	bash test.sh $(TARGETDIR)/$(TARGET)

#Remake
remake: cleaner all

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) $(TARGETDIR)/$(TARGET)

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

#Non-File Targets
.PHONY: all remake clean cleaner directories