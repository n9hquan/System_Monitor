#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "cpu/cpu.h"
#include "mem/mem.h"
#include "proc/proc.h"

#define MAX_SHOWN 20
#define WIDTH 78
#define INNER_WIDTH (WIDTH - 4)
#define BAR_WIDTH 24

static void restore_terminal(void)
{
    printf("\033[?25h\033[?1049l");
    fflush(stdout);
}

static void handle_exit_signal(int signo)
{
    (void)signo;
    restore_terminal();
    exit(0);
}

static void setup_terminal(void)
{
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);
    atexit(restore_terminal);

    printf("\033[?1049h\033[?25l\033[2J");
    fflush(stdout);
}

static void draw_line(void)
{
    for (int i = 0; i < WIDTH; i++)
        putchar('=');
    putchar('\n');
}

static void draw_row(const char *text)
{
    printf("| %-*s |\n", INNER_WIDTH, text);
}

static void draw_header(const char *title)
{
    draw_line();
    draw_row(title);
    draw_line();
}

static void build_bar(double percent, char *bar, size_t size)
{
    int filled = (int)(percent * BAR_WIDTH / 100.0 + 0.5);
    if (filled < 0)
        filled = 0;
    if (filled > BAR_WIDTH)
        filled = BAR_WIDTH;

    for (int i = 0; i < BAR_WIDTH && (size_t)i + 1 < size; i++)
        bar[i] = (i < filled) ? '#' : '.';

    bar[BAR_WIDTH] = '\0';
}

int main(void)
{
    cpu_init();
    proc_init();
    setup_terminal();

    sleep(1);

    ProcInfo procs[1024];

    while (1) {
        double cpu = cpu_read_percent();
        MemInfo mem;
        mem_read(&mem);
        int n = proc_read_all(procs, 1024);
        char cpu_bar[BAR_WIDTH + 1];
        char mem_bar[BAR_WIDTH + 1];
        char row[128];

        build_bar(cpu, cpu_bar, sizeof cpu_bar);
        build_bar(mem.percent, mem_bar, sizeof mem_bar);

        printf("\033[H");

        draw_header("SYSTEM MONITOR");

        snprintf(row, sizeof row, "CPU Cores : %-3ld", sysconf(_SC_NPROCESSORS_ONLN));
        draw_row(row);

        snprintf(row, sizeof row, "CPU Usage : %5.1f%%  [%s]", cpu, cpu_bar);
        draw_row(row);

        snprintf(row, sizeof row, "Memory    : %5ld / %-5ld MB  %5.1f%%  [%s]",
                 mem.used_mb, mem.total_mb, mem.percent, mem_bar);
        draw_row(row);

        draw_line();

        printf("| %-6s | %-8s | %-10s | %-41s |\n",
               "PID", "CPU%", "MEM(MB)", "NAME");

        draw_line();

        int shown = (n < MAX_SHOWN) ? n : MAX_SHOWN;

        for (int i = 0; i < shown; i++) {
            printf("| %-6d | %-8.1f | %-10.1f | %-41.41s |\n",
                   procs[i].pid,
                   procs[i].cpu_percent,
                   procs[i].mem_mb,
                   procs[i].name);
        }

        draw_line();

        fflush(stdout);

        sleep(1);
    }

    return 0;
}
