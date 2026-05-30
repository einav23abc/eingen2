#################
# Configurables #
#################

TARGET_NAME = libengine.a

CROSS := i686-w64-mingw32

CC := $(CROSS)-gcc
AR := $(CROSS)-ar


SRCS_DIR = src
SHADERS_DIR = shaders
BUILDS_DIR_NAME = build
RELEASE_DIR_NAME = release
DEBUG_DIR_NAME = debug
OBJS_DIR_NAME = objs


# Compiler flags for dependency generation
CC_DEPFLAGS = -MMD -MP

INCLUDE_DIRS :=
INCLUDE_DIRS += include src

CC_INCLUDE_DIRS_FLAGS = $(foreach dir,$(INCLUDE_DIRS),-I $(dir))

CC_FLAGS = $(CC_INCLUDE_DIRS_FLAGS) $(CC_DEPFLAGS)

CC_RELEASE_FLAGS = $(CC_FLAGS) -O3 -D RELEASE
CC_DEBUG_FLAGS = $(CC_FLAGS) -Wall -Werror -Wextra -g -D DEBUG


AR_FLAGS = -rcs


SHADER_TO_C := devops_tools/shader_to_c


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
SRCS += src/utils/utils.c
SRCS += src/vec_mat_quat/vec_mat_quat.c
SRCS += src/shaders/shaders.c
SRCS += src/cameras/cameras.c
SRCS += src/frame_buffer_objects/frame_buffer_objects.c
SRCS += src/meshes_and_animations/meshes_and_animations.c
SRCS += $(foreach shader_name,$(SHADERS),$(SHADERS_DIR)/$(shader_name)/$(shader_name).c)

SHADERS :=
SHADERS += default_shader
OBJS := $(patsubst %.c,%.o,$(SRCS))

RELEASE_OBJS = $(patsubst %,$(RELEASE_OBJS_DIR)/%,$(OBJS))
DEBUG_OBJS = $(patsubst %,$(DEBUG_OBJS_DIR)/%,$(OBJS))

OBJS_DIRS := $(dir $(OBJS))
RELEASE_DIRS := $(BUILDS_DIR) $(RELEASE_DIR) $(RELEASE_OBJS_DIR) $(foreach dir, $(OBJS_DIRS), $(RELEASE_OBJS_DIR)/$(dir))
DEBUG_DIRS := $(BUILDS_DIR) $(DEBUG_DIR) $(DEBUG_OBJS_DIR) $(foreach dir, $(OBJS_DIRS), $(DEBUG_OBJS_DIR)/$(dir))


##############
# Make Rules #
##############

# Don't delete files after no longer needed
.SECONDARY:

.PHONY: default clean all release debug
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
release: $(RELEASE_TARGET)

$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(AR) $(AR_FLAGS) $@ $^

$(RELEASE_OBJS_DIR)/$(SRCS_DIR)/%.o: $(SRCS_DIR)/%.c | $(RELEASE_DIRS)
	$(CC) $(CC_RELEASE_FLAGS) $< -c -o $@

# release shader

$(RELEASE_OBJS_DIR)/$(SHADERS_DIR)/%.h $(RELEASE_OBJS_DIR)/$(SHADERS_DIR)/%.c: $(SHADERS_DIR)/%.frag $(SHADERS_DIR)/%.vert | $(RELEASE_DIRS)
	$(SHADER_TO_C) $(dir $<) $(RELEASE_OBJS_DIR)/$(SHADERS_DIR)

$(RELEASE_OBJS_DIR)/$(SHADERS_DIR)/%.o: $(RELEASE_OBJS_DIR)/$(SHADERS_DIR)/%.c $(RELEASE_OBJS_DIR)/$(SHADERS_DIR)/%.h | $(RELEASE_DIRS)
	$(CC) $(CC_RELEASE_FLAGS) $< -c -o $@


# debug
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(AR) $(AR_FLAGS) $@ $^

$(DEBUG_OBJS_DIR)/$(SRCS_DIR)/%.o: $(SRCS_DIR)/%.c | $(DEBUG_DIRS)
	$(CC) $(CC_DEBUG_FLAGS) $< -c -o $@

# debug shaders

$(DEBUG_OBJS_DIR)/$(SHADERS_DIR)/%.h $(DEBUG_OBJS_DIR)/$(SHADERS_DIR)/%.c: $(SHADERS_DIR)/%.frag $(SHADERS_DIR)/%.vert | $(DEBUG_DIRS)
	$(SHADER_TO_C) $(dir $<) $(DEBUG_OBJS_DIR)/$(SHADERS_DIR)

$(DEBUG_OBJS_DIR)/$(SHADERS_DIR)/%.o: $(DEBUG_OBJS_DIR)/$(SHADERS_DIR)/%.c $(DEBUG_OBJS_DIR)/$(SHADERS_DIR)/%.h | $(DEBUG_DIRS)
	$(CC) $(CC_DEBUG_FLAGS) $< -c -o $@


clean:
	rm -fr $(BUILDS_DIR)

