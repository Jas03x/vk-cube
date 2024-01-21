cc = gcc

bin = bin
src = source

output = vk-cube

objects = $(patsubst $(src)/%.c, $(bin)/%.o, $(wildcard $(src)/*.c))

ifeq ($(MAKECMDGOALS),win)
	include makefile.win
else ifeq ($(MAKECMDGOALS),lnx)
	include makefile.lnx
endif
