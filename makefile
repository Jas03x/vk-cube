
bin = bin
src = source

output = vk-cube

vertex_shaders = $(patsubst $(src)/%.vert.glsl, $(bin)/%.vert.spv, $(wildcard $(src)/*.vert.glsl))
fragment_shaders = $(patsubst $(src)/%.frag.glsl, $(bin)/%.frag.spv, $(wildcard $(src)/*.frag.glsl))
objects = $(patsubst $(src)/%.c, $(bin)/%.o, $(wildcard $(src)/*.c))

ifeq ($(OS),Windows_NT)
include makefile.win
else ifeq ($(shell uname -s), Linux)
include makefile.lnx
else
$(error unknown os)
endif
