
bin = bin
src = source

output = vk-cube

objects = $(patsubst $(src)/%.c, $(bin)/%.o, $(wildcard $(src)/*.c))


ifeq ($(OS),Windows_NT)
include makefile.win
else ifeq ($(shell uname -s), Linux)
include makefile.lnx
else
$(error unknown os)
endif
