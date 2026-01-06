CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
OBJECTS = src/main.o src/parser.o src/utils.o

NAME = ws_interpreter

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(NAME)

.PHONY: all clean
