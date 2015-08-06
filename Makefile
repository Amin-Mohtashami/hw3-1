CC=gcc
CC_FLAGS=-ggdb3

all: main map wc

main:
	$(CC) $(CC_FLAGS) main.c -o main
map:
	$(CC) $(CC_FLAGS) map.c -o map
wc:
	$(CC) $(CC_FLAGS) wc.c -o wc
clean:
	rm -f wc main map
