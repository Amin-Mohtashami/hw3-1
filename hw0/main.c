#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

int
main() {
    struct rlimit lim;
    int stack_size = 0, proc_lim = 0, max_file_desc = 0;

    if (getrlimit(RLIMIT_STACK, &lim) == 0)
        stack_size = lim.rlim_cur;
    printf("stack size: %d\n",  stack_size);

    if (getrlimit(RLIMIT_NPROC, &lim) == 0)
        proc_lim = lim.rlim_cur;
    printf("process limit: %d\n", proc_lim);

    if (getrlimit(RLIMIT_NOFILE, &lim) == 0)
        max_file_desc = lim.rlim_cur;
    printf("max file descriptors: %d\n", max_file_desc);
}
