# helper function
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#################
# Configurables #
#################

TARGET_NAME = game.exe

CC = gcc.exe


INCLUDE_ROOT_DIR = include
ENGINE_INCLUDE_ROOT_DIR = ../engine/include
SRCS_DIR = src
BUILDS_DIR_NAME = build
RELEASE_DIR_NAME = release
DEBUG_DIR_NAME = debug
OBJS_DIR_NAME = objs


ENGINE_RELEASE_LIBRARY_LOCATION = ../engine/build/release
ENGINE_DEBUG_LIBRARY_LOCATION = ../engine/build/debug
# ensures the libengine.a is linked fully
CC_RELEASE_ENGINE_LINK_FLAGS = -Wl,--whole-archive -L"$(ENGINE_RELEASE_LIBRARY_LOCATION)" -l engine -Wl,--no-whole-archive
CC_DEBUG_ENGINE_LINK_FLAGS = -Wl,--whole-archive -L"$(ENGINE_DEBUG_LIBRARY_LOCATION)" -l engine -Wl,--no-whole-archive

LIBRARIES = SDL2main SDL2 opengl32 SDL2_image SDL2_mixer libpng16-16 Ws2_32
CC_LINK_FLAGS = $(foreach lib,$(LIBRARIES),-l $(lib))

CC_RELEASE_LINK_FLAGS = $(CC_RELEASE_ENGINE_LINK_FLAGS) $(CC_LINK_FLAGS)
CC_DEBUG_LINK_FLAGS = $(CC_DEBUG_ENGINE_LINK_FLAGS) $(CC_LINK_FLAGS)


# Compiler flags for dependency generation
CC_DEPFLAGS = -MMD -MP

# Find all include subdirectories
INCLUDE_DIRS = $(sort $(foreach file,$(call rwildcard,$(INCLUDE_ROOT_DIR),*.h),$(dir $(file))))
ENGINE_INCLUDE_DIRS = $(sort $(foreach file,$(call rwildcard,$(ENGINE_INCLUDE_ROOT_DIR),*.h),$(dir $(file))))
CC_INCLUDE_DIRS_FLAGS = $(foreach dir,$(INCLUDE_DIRS) $(ENGINE_INCLUDE_DIRS),-I $(dir))

CC_FLAGS = $(CC_INCLUDE_DIRS_FLAGS) $(CC_DEPFLAGS)

CC_RELEASE_FLAGS = $(CC_FLAGS) -O3 -D RELEASE
CC_DEBUG_FLAGS = $(CC_FLAGS) -Wall -Werror -Wextra -g -D DEBUG


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


SRCS = $(call rwildcard,$(SRCS_DIR),*.c)
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
	if not exist $(subst /,\,$@) mkdir $(subst /,\,$@)

release_dirs: $(BUILDS_DIR)
	if not exist $(subst /,\,$(RELEASE_DIR)) mkdir $(subst /,\,$(RELEASE_DIR))
	if not exist $(subst /,\,$(RELEASE_OBJS_DIR)) mkdir $(subst /,\,$(RELEASE_OBJS_DIR))
	$(foreach dir,$(OBJS_DIRS), $(shell if not exist $(subst /,\,$(RELEASE_OBJS_DIR)/$(dir)) mkdir $(subst /,\,$(RELEASE_OBJS_DIR)/$(dir))))


debug_dirs: $(BUILDS_DIR)
	if not exist $(subst /,\,$(DEBUG_DIR)) mkdir $(subst /,\,$(DEBUG_DIR))
	if not exist $(subst /,\,$(DEBUG_OBJS_DIR)) mkdir $(subst /,\,$(DEBUG_OBJS_DIR))
	$(foreach dir,$(OBJS_DIRS), $(shell if not exist $(subst /,\,$(DEBUG_OBJS_DIR)/$(dir)) mkdir $(subst /,\,$(DEBUG_OBJS_DIR)/$(dir))))


# release
release: release_dirs $(RELEASE_TARGET)

$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(CC) $(CC_RELEASE_FLAGS) $(CC_RELEASE_LINK_FLAGS) $^ -o $@

$(RELEASE_OBJS_DIR)/%.o: $(SRCS_DIR)/%.c $(INCLUDE_ROOT_DIR)/%.h release_dirs
	$(CC) $(CC_RELEASE_FLAGS) $< -c -o $@


# debug
debug: debug_dirs $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CC) $(CC_DEBUG_FLAGS) $(CC_DEBUG_LINK_FLAGS) $^ -o $@

$(DEBUG_OBJS_DIR)/%.o: $(SRCS_DIR)/%.c $(INCLUDE_ROOT_DIR)/%.h debug_dirs
	$(CC) $(CC_DEBUG_FLAGS) $< -c -o $@


clean:
	del /Q /F /S $(subst /,\,$(BUILDS_DIR)\*)
	rmdir /Q /S $(subst /,\,$(BUILDS_DIR))

