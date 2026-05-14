CC     = gcc
CFLAGS = -Wall -Wextra -O2
OBJS   = main.o cpu/cpu.o mem/mem.o proc/proc.o

System_Monitor: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -Icpu -Imem -Iproc -c $< -o $@

cpu/cpu.o: cpu/cpu.c
	$(CC) $(CFLAGS) -c $< -o $@

mem/mem.o: mem/mem.c
	$(CC) $(CFLAGS) -c $< -o $@

proc/proc.o: proc/proc.c
	$(CC) $(CFLAGS) -Icpu -c $< -o $@

clean:
	rm -f cpu/cpu.o mem/mem.o proc/proc.o main.o System_Monitor