#include <stdio.h>
#include <string.h>
#include "mem.h"

void mem_read(MemInfo *out)
{
    out->total_mb = 0;
    out->used_mb  = 0;
    out->percent  = 0.0;

    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return;

    long total_kb = 0, available_kb = 0;
    char line[256];
    int  found = 0;

    while (fgets(line, sizeof line, f) && found < 2) {
        if (sscanf(line, "MemTotal: %ld kB", &total_kb) == 1) found++;
        else if (sscanf(line, "MemAvailable: %ld kB", &available_kb) == 1) found++;
    }
    fclose(f);

    if (total_kb <= 0) return;

    long used_kb  = total_kb - available_kb;
    out->total_mb = total_kb / 1024;
    out->used_mb  = used_kb  / 1024;
    out->percent  = 100.0 * (double)used_kb / (double)total_kb;
}