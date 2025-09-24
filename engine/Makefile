#################
# Configurables #
#################

TARGET_NAME = engine

CC = gcc


INCLUDE_DIR = include
SRCS_DIR = src
BUILDS_DIR_NAME = build
RELEASE_DIR_NAME = release
DEBUG_DIR_NAME = debug
OBJS_DIR_NAME = objs


# Compiler flags for dependency generation
DEPFLAGS = -MMD -MP

CFLAGS = -I $(INCLUDE_DIR) $(DEPFLAGS)

RELEASE_FLAGS = -O3 -DNDEBUG
DEBUG_FLAGS = -Wall -Werror -Wextra


#############
# Variables #
#############

BUILDS_DIR = $(BUILDS_DIR_NAME)

RELEASE_DIR = $(BUILDS_DIR)/$(RELEASE_DIR_NAME)
RELEASE_OBJS_DIR = $(RELEASE_DIR)/$(OBJS_DIR_NAME)
RELEASE_TARGET = $(RELEASE_DIR)/$(TARGET_NAME).exe

DEBUG_DIR = $(BUILDS_DIR)/$(DEBUG_DIR_NAME)
DEBUG_OBJS_DIR = $(DEBUG_DIR)/$(OBJS_DIR_NAME)
DEBUG_TARGET = $(DEBUG_DIR)/$(TARGET_NAME).exe


SRCS = $(wildcard $(SRCS_DIR)/*.c)
OBJS = $(patsubst $(SRCS_DIR)/%.c,%.o,$(SRCS))
RELEASE_OBJS = $(patsubst %,$(RELEASE_OBJS_DIR)/%,$(OBJS))
DEBUG_OBJS = $(patsubst %,$(DEBUG_OBJS_DIR)/%,$(OBJS))


##############
# Make Rules #
##############

.PHONY: clean all release debug
default: debug


# Include generated dependency files
-include $(wildcard *.d)


all: release debug


# dirs
$(BUILDS_DIR):
	mkdir $(subst /,\,$@)

$(RELEASE_DIR): $(BUILDS_DIR)
	mkdir $(subst /,\,$@)

$(RELEASE_OBJS_DIR): $(RELEASE_DIR)
	mkdir $(subst /,\,$@)

$(DEBUG_DIR): $(BUILDS_DIR)
	mkdir $(subst /,\,$@)

$(DEBUG_OBJS_DIR): $(DEBUG_DIR)
	mkdir $(subst /,\,$@)


# release
release: $(RELEASE_DIR) $(RELEASE_TARGET)

$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(RELEASE_OBJS_DIR)/%.o: $(SRCS_DIR)/%.c $(INCLUDE_DIR)/%.h $(RELEASE_OBJS_DIR)
	$(CC) $(CFLAGS) $< -c -o $@


# debug
debug: $(DEBUG_DIR) $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(DEBUG_OBJS_DIR)/%.o: $(SRCS_DIR)/%.c $(INCLUDE_DIR)/%.h $(DEBUG_OBJS_DIR)
	$(CC) $(CFLAGS) $< -c -o $@


clean:
	del $(subst /,\,$(RELEASE_OBJS_DIR)/*.o $(DEBUG_OBJS_DIR)/*.o $(RELEASE_OBJS_DIR)/*.d $(DEBUG_OBJS_DIR)/*.d $(RELEASE_TARGET) $(DEBUG_TARGET))
