#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

/* Globals */
char *s;
char *cwd;
tok_t *PATH;
int PATH_SIZE = 0, lineNum = 0;

int cmd_quit(tok_t arg[]);
int cmd_help(tok_t arg[]);
int cmd_cd(tok_t arg[]);

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
    {cmd_cd, "cd", "change working directory"},
};

int cmd_help(tok_t arg[]) {
    for (int i = 0; i < (int)(sizeof(cmd_table)/sizeof(fun_desc_t)); i++)
        printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
    return 1;
}

int cmd_quit(tok_t arg[]) {
    printf("Bye\n");

    free(s);
    freeToks(PATH);
    exit(0);

    return 1;
}

int cmd_cd(tok_t arg[]) {
    return chdir(arg[0]);
}

int lookup(char cmd[]) {
    for (int i = 0; i < (int)(sizeof(cmd_table)/sizeof(fun_desc_t)); i++)
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
    return -1;
}

void init_shell() {
    /* Check if we are running interactively */
    shell_terminal = STDIN_FILENO;
    s = malloc(INPUT_STRING_SIZE+1); /* user input string */

    /** Note that we cannot take control of the terminal if the shell
    is not interactive */
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        /* force into foreground */
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
            kill (- shell_pgid, SIGTTIN);

            /* Ignore interactive and job-control signals.  */
            signal (SIGINT, SIG_IGN);
            signal (SIGQUIT, SIG_IGN);
            signal (SIGTSTP, SIG_IGN);
            signal (SIGTTIN, SIG_IGN);
            signal (SIGTTOU, SIG_IGN);
            signal (SIGCHLD, SIG_IGN);

        shell_pgid = getpid();
        /* Put shell in its own process group */
        if(setpgid(shell_pgid, shell_pgid) < 0) {
          perror("Couldn't put the shell in its own process group... Eish.");
          exit(1);
        }

        /* Take control of the terminal */
        tcsetpgrp(shell_terminal, shell_pgid);
        tcgetattr(shell_terminal, &shell_tmodes);
  }
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
process* create_process(tok_t *input_str_tokens, int token_count,
    tok_t token_markers[], int bg) {
    /* Make sure there's an actual program to be run */
    if (input_str_tokens[0] != NULL) {
        pid_t pid = fork();
        int status;
        int pgid = getpgrp();

        if (pid >= 0) {
            if (pid == 0) {
                process p;

                // Initial fill in of process struct
                p.argv = input_str_tokens;
                p.argc = token_count;
                p.pid =  getpid();

		// if bg process, don't bring to fg
		if (bg == 0) {
                    if (pgid == 0) pgid = p.pid;
                    setpgid(p.pid, pgid);
	            tcsetpgrp(STDIN_FILENO, pgid);
		}
		else {
		    fprintf(stdout, "[1] %d\n", p.pid);
	            tcsetpgrp(STDIN_FILENO, pgid);
		}

                /* Signal control back to normal.  */
                signal (SIGINT, SIG_DFL);
                signal (SIGQUIT, SIG_DFL);
                signal (SIGTSTP, SIG_DFL);
                signal (SIGTTIN, SIG_DFL);
                signal (SIGTTOU, SIG_DFL);
                signal (SIGCHLD, SIG_DFL);

                p.background = 'n';
                p.completed  = 'n';
                p.stopped    = 'n';

                p.next = NULL;
                p.prev = NULL;

                char *prog = path_resolve(input_str_tokens[0]);

                // Make sure args end with null byte;
                input_str_tokens[token_count] = '\0';

                // Check for any redirection, if found set appropriately
                for (int i = 0; token_markers[i] && (i < MAXTOKS-1); i+=2) {
                    int in_out = token_markers[i][0] == '<' ? 0 : 1;
                    int fd;

                    if (in_out == 0) {
                        fd = open(token_markers[i+1], O_RDONLY);
                        p.stdin = fd;
                    }
                    else {
                        fd = open(token_markers[i+1], O_WRONLY | O_TRUNC |
                            O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                        p.stdout = fd;
                    }

                    dup2(fd, in_out);
                    close(fd);
                }

                // execute program after resolution, redirection
                int ret;
                ret = execve(prog, input_str_tokens, NULL);
                free(prog);

                // update process struct
                p.completed = 'y';
                p.stopped = 'y';

		// If there any errors present
		if (ret == -1)
			perror("execve");

                exit(ret);
            }
            else
                wait(&status);
        }
    }

    return NULL;
}

tok_t path_resolve(tok_t program) {
    if (program[0] != '/')
        for (int i = 0; i < MAXTOKS && PATH[i]; i++) {
            tok_t _temp = concat(3, PATH[i], "/", program);

            if (file_exists(_temp)) return _temp;
        }

    return program;
}

int file_exists(char *filename) {
    return (access(filename, F_OK) != -1);
}

void prompter() {
    cwd = getcwd(NULL, 0);
    fprintf(stdout, "%d - \033[40m\033[4m\033[1m\033[33m%s\033[0m $ ",
        ++lineNum, cwd);
    free(cwd);
}

int shell(int argc, char *argv[]) {
    tok_t tok_special_indexes[MAXTOKS];

    fprintf(stdout, "Welcome to Shelley v1.0\n...\n");

    tok_t *t; /* tokens parsed from input */

    int fundex = -1, tok_num = 0, bg = 0;

    pid_t pid = getpid(),   /* get current processes PID */
          ppid = getppid(), /* get parents PID */
          cpid, tcpid, cpgid;

    init_shell();

    PATH = getToks(getenv("PATH"), &tok_num, tok_special_indexes, &bg);

    do {
        t = getToks(s, &tok_num, tok_special_indexes, &bg); /* break the line into tokens */

        if (t != NULL) {
            fundex = lookup(t[0]); /* Is first token a shell literal */

            if (fundex >= 0)
                cmd_table[fundex].fun(&t[1]);
            else {
                create_process(t, tok_num, tok_special_indexes, bg);
                freeToks(t);
            }
        }

        prompter();
    } while ((s = freadln(stdin)));

    return cmd_table[lookup("quit")].fun(&t[1]);
}
