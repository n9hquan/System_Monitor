#include <stdio.h>
#include "cpu.h"

static unsigned long long prev_total = 0;
static unsigned long long prev_idle  = 0;

static int read_cpu_times(unsigned long long *total, unsigned long long *idle)
{
    FILE *f = fopen("/proc/stat", "r");
    if (!f) return -1;

    unsigned long long user, nice, system, idle_t, iowait,
                       irq, softirq, steal, guest, guest_nice;
    int n = fscanf(f, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle_t, &iowait,
                   &irq, &softirq, &steal, &guest, &guest_nice);
    fclose(f);
    if (n < 4) return -1;

    *idle  = idle_t + iowait;
    *total = user + nice + system + idle_t + iowait
           + irq + softirq + steal + guest + guest_nice;
    return 0;
}

void cpu_init(void)
{
    read_cpu_times(&prev_total, &prev_idle);
}

double cpu_read_percent(void)
{
    unsigned long long total, idle;
    if (read_cpu_times(&total, &idle) != 0) return 0.0;

    unsigned long long total_delta = total - prev_total;
    unsigned long long idle_delta  = idle  - prev_idle;

    prev_total = total;
    prev_idle  = idle;

    if (total_delta == 0) return 0.0;
    return 100.0 * (double)(total_delta - idle_delta) / (double)total_delta;
}
