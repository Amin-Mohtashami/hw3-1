#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

int cmd_quit(tok_t arg[]) {
    printf("Bye\n");
    exit(0);

    return 1;
}

int cmd_cd(tok_t arg[]) {
    return chdir(arg[0]);
}

int cmd_help(tok_t arg[]);

/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token array and return int */
typedef struct fun_desc {
    cmd_fun_t *fun;
    char *cmd;
    char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
    {cmd_help, "?", "show this help menu"},
    {cmd_quit, "quit", "quit the command shell"},
    {cmd_cd, "cd", "change directory"},
};

tok_t *PATH;
int PATH_SIZE = 0;

int cmd_help(tok_t arg[]) {
    int i;

    for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
        printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
    }

    return 1;
}

int lookup(char cmd[]) {
    int i;

    for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
    }

    return -1;
}

void init_shell() {
    /* Check if we are running interactively */
    shell_terminal = STDIN_FILENO;

    /** Note that we cannot take control of the terminal if the shell
    is not interactive */
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        /* force into foreground */
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
            kill( - shell_pgid, SIGTTIN);

        shell_pgid = getpid();
        /* Put shell in its own process group */
        if(setpgid(shell_pgid, shell_pgid) < 0){
          perror("Couldn't put the shell in its own process group... Eish.");
          exit(1);
        }

        /* Take control of the terminal */
        tcsetpgrp(shell_terminal, shell_pgid);
        tcgetattr(shell_terminal, &shell_tmodes);
  }

  /** YOUR CODE HERE */
}

/**
 * Add a process to our process list
 */
void add_process(process* p) {
  /** YOUR CODE HERE */
}

/**
 * Creates a process given the inputString from stdin
 */
process* create_process(tok_t *input_str_tokens) {
    /* Make sure there's an actual program to be run */
    if (input_str_tokens[0] != NULL) {
        pid_t pid = fork();
        int status;

        if (pid >= 0) {
            if (pid == 0) {
                char *prog = path_resolve(input_str_tokens[0]);
                input_str_tokens[3] = '\0';
                int ret = execve(prog, input_str_tokens, NULL);
                free(prog);

                if (ret == -1)
                    switch (errno) {
                        case 2 : printf("No such file or directory bro.\n");
                    }

                exit(0);
            }
            else
                wait(&status);
        }
    }

    return NULL;
}

tok_t path_resolve(tok_t program) {
    if (program[0] != '/') {
        int i;

        for (i = 0; i < MAXTOKS && PATH[i]; i++) {
            tok_t _temp = concat(3, PATH[i], "/", program);

            if (file_exists(_temp))
                return _temp;
        }
    }

    return program;
}

int file_exists(char *filename) {
    return (access(filename, F_OK) != -1);
}

int shell(int argc, char *argv[]) {
    char *s = malloc(INPUT_STRING_SIZE+1);			/* user input string */

    tok_t *t;		     	/* tokens parsed from input */
    PATH = getToks(getenv("PATH"));

    int lineNum = 0;
    int fundex = -1;

    pid_t pid = getpid();   /* get current processes PID */
    pid_t ppid = getppid();	/* get parents PID */
    pid_t cpid, tcpid, cpgid;

    init_shell();

    fprintf(stdout, "%d %s $ ", lineNum, get_current_dir_name());

    while ((s = freadln(stdin))) {
        t = getToks(s); /* break the line into tokens */
        fundex = lookup(t[0]); /* Is first token a shell literal */

        if (fundex >= 0)
            cmd_table[fundex].fun(&t[1]);
        else {
            create_process(t);
            freeToks(t);
        }

        lineNum++;
        fprintf(stdout, "%d %s $ ", lineNum, get_current_dir_name());
    }

    return 0;
}
