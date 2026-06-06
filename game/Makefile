#################
# Configurables #
#################

TARGET_NAME = game.exe


SRCS_DIR = src
SHADERS_DIR = shaders
BUILDS_DIR_NAME = build
EXPORT_DIR := /mnt/c/Users/einav/Desktop/eingen2/
RELEASE_DIR_NAME = release
DEBUG_DIR_NAME = debug
OBJS_DIR_NAME = objs
LIBS_DIR := libs
MODELS_DIR := models
TEXTURES_DIR := textures


ENGINE_RELEASE_LIBRARY_LOCATION = ../engine/build/release
ENGINE_DEBUG_LIBRARY_LOCATION = ../engine/build/debug

LIBRARIES :=
# LIBRARIES += SDL2main
LIBRARIES += SDL2
LIBRARIES += opengl32
# LIBRARIES += SDL2_mixer
# LIBRARIES += Ws2_32


STATIC_LIBRARIES :=
STATIC_LIBRARIES += ./libs/libSDL2_image.a

DEBUG_STATIC_LIBRARIES := $(STATIC_LIBRARIES)
DEBUG_STATIC_LIBRARIES += $(ENGINE_DEBUG_LIBRARY_LOCATION)/libengine.a

RELEASE_STATIC_LIBRARIES := $(STATIC_LIBRARIES)
RELEASE_STATIC_LIBRARIES += $(ENGINE_RELEASE_LIBRARY_LOCATION)/libengine.a


INCLUDE_DIRS := include src shaders

ENGINE_INCLUDE_DIRS :=
ENGINE_INCLUDE_DIRS += ../engine/include
ENGINE_INCLUDE_DIRS += ../engine/include/SDL2
ENGINE_INCLUDE_DIRS += ../engine/include/SDL2_image
ENGINE_INCLUDE_DIRS += ../engine/src

BUILDS_DIR = $(BUILDS_DIR_NAME)

RELEASE_BUILD_DIR := $(BUILDS_DIR)/$(RELEASE_DIR_NAME)
RELEASE_OBJS_DIR := $(RELEASE_BUILD_DIR)/$(OBJS_DIR_NAME)
RELEASE_EXPORT_DIR := $(EXPORT_DIR)/$(RELEASE_DIR_NAME)
RELEASE_TARGET := $(RELEASE_EXPORT_DIR)/$(TARGET_NAME)

DEBUG_BUILD_DIR := $(BUILDS_DIR)/$(DEBUG_DIR_NAME)
DEBUG_OBJS_DIR := $(DEBUG_BUILD_DIR)/$(OBJS_DIR_NAME)
DEBUG_EXPORT_DIR := $(EXPORT_DIR)/$(DEBUG_DIR_NAME)
DEBUG_TARGET := $(DEBUG_EXPORT_DIR)/$(TARGET_NAME)


SRCS := 
SRCS += src/init.c
SRCS += src/update.c
SRCS += src/render.c

SHADERS :=
SHADERS += global_shader
SHADERS += sun_shadow_map_shader
SHADERS += grass_shader
SHADER_SRCS := $(foreach shader_path,$(SHADERS),$(SHADERS_DIR)/$(shader_path)/$(notdir $(shader_path)).c)
SRCS += $(SHADER_SRCS)

OBJS := $(patsubst %.c,%.o,$(SRCS))

RELEASE_OBJS = $(patsubst %,$(RELEASE_OBJS_DIR)/%,$(OBJS))
DEBUG_OBJS = $(patsubst %,$(DEBUG_OBJS_DIR)/%,$(OBJS))

OBJS_DIRS := $(dir $(OBJS))
RELEASE_DIRS := $(BUILDS_DIR) $(RELEASE_EXPORT_DIR) $(RELEASE_BUILD_DIR) $(RELEASE_OBJS_DIR) $(foreach dir, $(OBJS_DIRS), $(RELEASE_OBJS_DIR)/$(dir))
DEBUG_DIRS := $(BUILDS_DIR) $(DEBUG_EXPORT_DIR) $(DEBUG_BUILD_DIR) $(DEBUG_OBJS_DIR) $(foreach dir, $(OBJS_DIRS), $(DEBUG_OBJS_DIR)/$(dir))


###################
# Tools and flags #
###################

CROSS := i686-w64-mingw32

CC := $(CROSS)-gcc
LD := $(CROSS)-ld


CC_RELEASE_ENGINE_LINK_FLAGS = -Wl,--whole-archive $(foreach lib, $(RELEASE_STATIC_LIBRARIES), -L $(dir $(lib)) -l $(patsubst lib%.a,%,$(notdir $(lib)))) -Wl,--no-whole-archive
CC_DEBUG_ENGINE_LINK_FLAGS = -Wl,--whole-archive $(foreach lib, $(DEBUG_STATIC_LIBRARIES), -L $(dir $(lib)) -l $(patsubst lib%.a,%,$(notdir $(lib)))) -Wl,--no-whole-archive

CC_LINK_FLAGS = $(foreach lib,$(LIBRARIES),-l $(lib)) -L $(LIBS_DIR)

CC_RELEASE_LINK_FLAGS = $(CC_RELEASE_ENGINE_LINK_FLAGS) $(CC_LINK_FLAGS)
CC_DEBUG_LINK_FLAGS = $(CC_DEBUG_ENGINE_LINK_FLAGS) $(CC_LINK_FLAGS)


# Compiler flags for dependency generation
CC_DEPFLAGS = -MMD -MP


CC_INCLUDE_DIRS_FLAGS = $(foreach dir,$(INCLUDE_DIRS) $(ENGINE_INCLUDE_DIRS),-I $(dir))

CC_FLAGS = $(CC_INCLUDE_DIRS_FLAGS) $(CC_DEPFLAGS)

CC_RELEASE_FLAGS = $(CC_FLAGS) -O3 -D RELEASE
CC_DEBUG_FLAGS = $(CC_FLAGS) -Wall -Werror -Wextra -g -D DEBUG


SHADER_TO_C := ../engine/devops_tools/shader_to_c


##############
# Make Rules #
##############

# Don't delete files after no longer needed
.SECONDARY:

.PHONY: clean all release debug release_dirs debug_dirs shaders
default: debug


# Include generated dependency files
-include $(patsubst %.c,%.d,$(SRCS))


all: release debug


# dirs
$(RELEASE_DIRS):
	mkdir -p $@

$(DEBUG_DIRS):
	mkdir -p $@


# release
release: $(RELEASE_TARGET) | $(RELEASE_DIRS)
	cp $(LIBS_DIR)/* $(RELEASE_EXPORT_DIR)
	mkdir -p $(RELEASE_EXPORT_DIR)/game
	cp -r $(MODELS_DIR) $(RELEASE_EXPORT_DIR)/game
	cp -r $(TEXTURES_DIR) $(RELEASE_EXPORT_DIR)/game

$(RELEASE_TARGET): $(RELEASE_OBJS) $(ENGINE_RELEASE_LIBRARY_LOCATION)/libengine.a
	$(CC) $(CC_RELEASE_FLAGS) $(CC_RELEASE_LINK_FLAGS) $(filter %.o, $^) -o $@

$(RELEASE_OBJS_DIR)/%.o: %.c | $(RELEASE_DIRS)
	$(CC) $(CC_RELEASE_FLAGS) $< -c -o $@


# debug
debug: $(DEBUG_TARGET) | $(DEBUG_DIRS)
	cp $(LIBS_DIR)/* $(DEBUG_EXPORT_DIR)
	mkdir -p $(DEBUG_EXPORT_DIR)/game
	cp -r $(MODELS_DIR) $(DEBUG_EXPORT_DIR)/game
	cp -r $(TEXTURES_DIR) $(DEBUG_EXPORT_DIR)/game

$(DEBUG_TARGET): $(DEBUG_OBJS) $(ENGINE_DEBUG_LIBRARY_LOCATION)/libengine.a
	$(CC) $(CC_DEBUG_FLAGS) $(CC_DEBUG_LINK_FLAGS) $(filter %.o, $^) -o $@

$(DEBUG_OBJS_DIR)/%.o: %.c | $(DEBUG_DIRS)
	$(CC) $(CC_DEBUG_FLAGS) $< -c -o $@


# shaders

shaders: $(SHADER_SRCS)

$(SHADERS_DIR)/%.h $(SHADERS_DIR)/%.c: $(SHADERS_DIR)/%.frag $(SHADERS_DIR)/%.vert
	$(SHADER_TO_C) $(dir $<)


clean:
	rm -fr $(BUILDS_DIR)
	rm -fr $(RELEASE_EXPORT_DIR)
	rm -fr $(DEBUG_EXPORT_DIR)
	rm -fr $(EXPORT_DIR_NAME)

