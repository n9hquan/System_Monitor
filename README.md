# System Monitor

A terminal-based system monitor that is inspired by `top`, written in C.

This program displays CPU usage, memory usage, and per-process statistics in real-time.

## Project Structure

```bash
System_Monitor/
├── cpu/
│   ├── cpu.h
│   ├── cpu.c
│   └── test_cpu.c
├── mem/
│   ├── mem.h
│   ├── mem.c
│   └── test_mem.c
├── proc/
│   ├── proc.h
│   ├── proc.c
│   └── test_proc.c
├── main.c
├── Makefile
└── README.md
```

## Requirements

- Linux (Ubuntu recommended) or WSL
- GCC compiler

## Build & Run

### Run the full system monitor

```bash
make ./System_Monitor
```

Stop with `Ctrl+C`. Clean build files with `make clean`.

## How It Works

Reads system information directly from the Linux `/proc` virtual filesystem.

The main loop refreshes every 1 second and displays the top 20 processes.
