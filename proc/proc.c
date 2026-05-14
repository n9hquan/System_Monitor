#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include "proc.h"

#define MAX_PROCS 1024

typedef struct {
    int                pid;
    unsigned long long prev_cpu;
} PrevSample;

static PrevSample         prev_samples[MAX_PROCS];
static int                prev_count = 0;
static unsigned long long prev_total_jiffies = 0;
static long               num_cores = 1;

static int read_proc_stat(int pid, char *name, size_t name_sz,
                          unsigned long long *cpu_jiffies)
{
    char path[64];
    snprintf(path, sizeof path, "/proc/%d/stat", pid);

    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char buf[1024];
    if (!fgets(buf, sizeof buf, f)) { fclose(f); return -1; }
    fclose(f);

    char *open  = strchr(buf, '(');
    char *close = strrchr(buf, ')');
    if (!open || !close || close <= open) return -1;

    size_t len = (size_t)(close - open - 1);
    if (len >= name_sz) len = name_sz - 1;
    memcpy(name, open + 1, len);
    name[len] = '\0';

    unsigned long long utime = 0, stime = 0;
    char state;
    int dummy_i;
    unsigned long dummy_lu;
    long dummy_ld;

    int n = sscanf(close + 2,
        "%c %d %d %d %d %d %lu %lu %lu %lu %llu %llu",
        &state,
        &dummy_i, &dummy_i, &dummy_i, &dummy_i, &dummy_i,
        &dummy_lu, &dummy_lu, &dummy_lu, &dummy_lu,
        &utime, &stime);
    if (n < 12) return -1;

    *cpu_jiffies = utime + stime;
    return 0;
}

static long read_proc_rss_kb(int pid)
{
    char path[64];
    snprintf(path, sizeof path, "/proc/%d/status", pid);

    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char line[256];
    long rss = 0;
    while (fgets(line, sizeof line, f)) {
        if (sscanf(line, "VmRSS: %ld kB", &rss) == 1) break;
    }
    fclose(f);
    return rss;
}

static unsigned long long read_total_jiffies(void)
{
    FILE *f = fopen("/proc/stat", "r");
    if (!f) return 0;
    unsigned long long v[10] = {0};
    fscanf(f, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
           &v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&v[8],&v[9]);
    fclose(f);
    unsigned long long total = 0;
    for (int i = 0; i < 10; i++) total += v[i];
    return total;
}

static unsigned long long lookup_prev_cpu(int pid)
{
    for (int i = 0; i < prev_count; i++)
        if (prev_samples[i].pid == pid) return prev_samples[i].prev_cpu;
    return 0;   
}

void proc_init(void)
{
    num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores < 1) num_cores = 1;
    prev_total_jiffies = read_total_jiffies();
    prev_count = 0;
}

int proc_read_all(ProcInfo *out, int max)
{
    unsigned long long total_now   = read_total_jiffies();
    unsigned long long total_delta = total_now - prev_total_jiffies;
    if (total_delta == 0) total_delta = 1;   

    DIR *d = opendir("/proc");
    if (!d) return 0;

    PrevSample new_samples[MAX_PROCS];
    int new_count = 0;
    int out_count = 0;

    struct dirent *e;
    while ((e = readdir(d)) != NULL && out_count < max && new_count < MAX_PROCS) {
        if (!isdigit((unsigned char)e->d_name[0])) continue;
        int pid = atoi(e->d_name);

        char name[PROC_NAME_LEN] = {0};
        unsigned long long cpu_now = 0;
        if (read_proc_stat(pid, name, sizeof name, &cpu_now) != 0) continue;

        unsigned long long cpu_prev  = lookup_prev_cpu(pid);
        unsigned long long cpu_delta = (cpu_now > cpu_prev) ? (cpu_now - cpu_prev) : 0;

        double cpu_pct = 100.0 * (double)cpu_delta / (double)total_delta * (double)num_cores;
        long   rss_kb  = read_proc_rss_kb(pid);

        out[out_count].pid         = pid;
        strncpy(out[out_count].name, name, PROC_NAME_LEN - 1);
        out[out_count].name[PROC_NAME_LEN - 1] = '\0';
        out[out_count].cpu_percent = cpu_pct;
        out[out_count].mem_mb      = rss_kb / 1024.0;
        out_count++;

        new_samples[new_count].pid      = pid;
        new_samples[new_count].prev_cpu = cpu_now;
        new_count++;
    }
    closedir(d);

    memcpy(prev_samples, new_samples, sizeof(PrevSample) * new_count);
    prev_count          = new_count;
    prev_total_jiffies  = total_now;

    return out_count;
}