#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cpu/cpu.h"
#include "mem/mem.h"
#include "proc/proc.h"

#define MAX_SHOWN 20

int main(void)
{
    cpu_init();
    proc_init();
    sleep(1);

    ProcInfo procs[1024];
    proc_read_all(procs, 1024);
    sleep(1);

    while (1) {
        double  cpu = cpu_read_percent();
        MemInfo mem; mem_read(&mem);
        int n   = proc_read_all(procs, 1024);

        printf("\033[H\033[2J");
        printf("=== System Monitor ===\n");
        printf("CPU Cores : %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
        printf("CPU Usage : %.1f %%\n", cpu);
        printf("Memory    : %ld MB / %ld MB (%.1f %%)\n\n",
               mem.used_mb, mem.total_mb, mem.percent);
        printf("  PID   CPU%%   MEM(MB)  NAME\n");

        int shown = (n < MAX_SHOWN) ? n : MAX_SHOWN;
        for (int i = 0; i < shown; i++) {
            printf("%5d  %5.1f  %8.1f  %s\n",
                   procs[i].pid, procs[i].cpu_percent,
                   procs[i].mem_mb, procs[i].name);
        }

        sleep(1);
    }
    return 0;
}