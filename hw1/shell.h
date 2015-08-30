#ifndef _SHELL_H_
#define _SHELL_H_

#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

#include "parse.h"

int shell_terminal; //File descripter for the terminal.
int shell_is_interactive; //1 if shell_terminal is a valid terminal. 0 otherwise
struct termios shell_tmodes; //terminal options for the shell
pid_t shell_pgid; //The shell's process id

int shell(int argc, char *argv[]);
tok_t path_resolve(tok_t program);
int file_exists(char *filename);

#endif
