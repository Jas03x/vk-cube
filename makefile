cc = gcc

bin = bin
src = source

output = vk-cube

objects = $(patsubst $(src)/%.c, $(bin)/%.o, $(wildcard $(src)/*.c))

$(bin)/%.o: $(src)/%.c
	$(cc) -c $^ -o $@

$(bin)/$(output): $(objects)
	$(cc) $(objects) -lSDL2 -o $@

clean:
	rm -f $(bin)/*.o
	rm -f $(bin)/$(output)
