#################
# Configurables #
#################

TARGET_NAME = libengine.a

CROSS := i686-w64-mingw32

CC := $(CROSS)-gcc
AR := $(CROSS)-ar


SRCS_DIR = src
BUILDS_DIR_NAME = build
RELEASE_DIR_NAME = release
DEBUG_DIR_NAME = debug
OBJS_DIR_NAME = objs


# Compiler flags for dependency generation
CC_DEPFLAGS = -MMD -MP

INCLUDE_DIRS :=
INCLUDE_DIRS += include

CC_INCLUDE_DIRS_FLAGS = $(foreach dir,$(INCLUDE_DIRS),-I $(dir))

CC_FLAGS = $(CC_INCLUDE_DIRS_FLAGS) $(CC_DEPFLAGS)

CC_RELEASE_FLAGS = $(CC_FLAGS) -O3 -D RELEASE
CC_DEBUG_FLAGS = $(CC_FLAGS) -Wall -Werror -Wextra -g -D DEBUG


AR_FLAGS = -rcs


#############
# Variables #
#############

BUILDS_DIR = $(BUILDS_DIR_NAME)

RELEASE_DIR = $(BUILDS_DIR)/$(RELEASE_DIR_NAME)
RELEASE_OBJS_DIR = $(RELEASE_DIR)/$(OBJS_DIR_NAME)
RELEASE_TARGET = $(RELEASE_DIR)/$(TARGET_NAME)

DEBUG_DIR = $(BUILDS_DIR)/$(DEBUG_DIR_NAME)
DEBUG_OBJS_DIR = $(DEBUG_DIR)/$(OBJS_DIR_NAME)
DEBUG_TARGET = $(DEBUG_DIR)/$(TARGET_NAME)


SRCS :=
SRCS += src/engine.c
SRCS += src/glad/glad.c

OBJS = $(patsubst $(SRCS_DIR)/%.c,%.o,$(SRCS))
RELEASE_OBJS = $(patsubst %,$(RELEASE_OBJS_DIR)/%,$(OBJS))
DEBUG_OBJS = $(patsubst %,$(DEBUG_OBJS_DIR)/%,$(OBJS))

OBJS_DIRS = $(dir $(OBJS))


##############
# Make Rules #
##############

.PHONY: clean all release debug release_dirs debug_dirs
default: debug


# Include generated dependency files
-include $(wildcard *.d)


all: release debug


# dirs
$(BUILDS_DIR):
	mkdir -p $@

release_dirs: $(BUILDS_DIR)
	mkdir -p $(RELEASE_DIR)
	mkdir -p $(RELEASE_OBJS_DIR)
	$(foreach dir,$(OBJS_DIRS), $(shell mkdir -p $(RELEASE_OBJS_DIR)/$(dir)))


debug_dirs: $(BUILDS_DIR)
	mkdir -p $(DEBUG_DIR)
	mkdir -p $(DEBUG_OBJS_DIR)
	$(foreach dir,$(OBJS_DIRS), $(shell mkdir -p $(DEBUG_OBJS_DIR)/$(dir)))


# release
release: release_dirs $(RELEASE_TARGET)

$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(AR) $(AR_FLAGS) $@ $^

$(RELEASE_OBJS_DIR)/%.o: $(SRCS_DIR)/%.c release_dirs
	$(CC) $(CC_RELEASE_FLAGS) $< -c -o $@


# debug
debug: debug_dirs $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(AR) $(AR_FLAGS) $@ $^

$(DEBUG_OBJS_DIR)/%.o: $(SRCS_DIR)/%.c debug_dirs
	$(CC) $(CC_DEBUG_FLAGS) $< -c -o $@


clean:
	rm -fr $(BUILDS_DIR)

