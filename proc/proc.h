#ifndef PROC_H
#define PROC_H

#define PROC_NAME_LEN 64

typedef struct {
    int    pid;
    char   name[PROC_NAME_LEN];
    double cpu_percent;
    double mem_mb;
} ProcInfo;

void proc_init(void);
int  proc_read_all(ProcInfo *out, int max);
#endif /* PROC_H */