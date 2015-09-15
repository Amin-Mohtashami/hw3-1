#################
#   variables   #
#################

# files
EXECUTABLE  = words
<<<<<<< HEAD
SOURCES  = words.c 
=======
SOURCES  = words.c
>>>>>>> 055eb9ae37b9bea59c9bfc2706f835be03c5041b

OBJECTS  = $(SOURCES:.c=.o)

# compilation and linking
CC      = gcc
CFLAGS  = -c -std=c99 -D_GNU_SOURCE
LDFLAGS = -lpthread
WARN    = -Wall -Wextra -pedantic
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(WARN)
LINK.c    = $(CC)

# define paths
vpath %.c src
vpath %.h include

#################
#     targets   #
#################

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK.c) $^ -o $@ $(LDFLAGS)
<<<<<<< HEAD

$(OBJECTS): %.o: %.c
	$(COMPILE.c) $< -o $@

# phony targets
.PHONY: clean

=======

$(OBJECTS): %.o: %.c
	$(COMPILE.c) $< -o $@

# phony targets
.PHONY: clean

>>>>>>> 055eb9ae37b9bea59c9bfc2706f835be03c5041b
# remove object files, emacs temporaries
clean:
	rm -f *.o *~

# print-VAR prints the value of the variable VAR
print-%  : ; @echo $* = $($*)
<<<<<<< HEAD



=======
>>>>>>> 055eb9ae37b9bea59c9bfc2706f835be03c5041b
