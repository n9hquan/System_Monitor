#include <stdio.h>
#include "mem.h"

int main(void) {
    MemInfo m;
    mem_read(&m);
    printf("Total : %ld MB\n", m.total_mb);
    printf("Used  : %ld MB\n", m.used_mb);
    printf("Usage : %.1f %%\n", m.percent);
    return 0;
}