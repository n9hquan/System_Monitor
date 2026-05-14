#include <stdio.h>
#include <unistd.h>
#include "proc.h"

int main(void) {
    ProcInfo list[1024];
    proc_init();
    sleep(1);
    int n = proc_read_all(list, 1024);
    printf("Got %d processes\n", n);
    for (int i = 0; i < n && i < 10; i++) {
        printf("%5d %6.1f%% %8.1f MB  %s\n",
               list[i].pid, list[i].cpu_percent,
               list[i].mem_mb, list[i].name);
    }
    return 0;
}