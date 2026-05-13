#ifndef MEM_H
#define MEM_H

typedef struct {
    long   total_mb;
    long   used_mb;
    double percent;
} MemInfo;

void mem_read(MemInfo *out);

#endif