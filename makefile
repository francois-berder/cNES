CC = cc
CFLAGS = -Wall -std=c99

all: emu

ppu.o: ppu.c ppu.h cpu.h
	$(CC) $(CFLAGS) -c ppu.c

cpu.o: cpu.c cpu.h ppu.h
	$(CC) $(CFLAGS) -c cpu.c

cart.o: cart.c cart.h
	$(CC) $(CFLAGS) -c cart.c

mappers.o: mappers.c mappers.h
	$(CC) $(CFLAGS) -c mappers.c

helper_functions.o: helper_functions.c helper_functions.h
	$(CC) $(CFLAGS) -c helper_functions.c

opcode_functions.o: opcode_functions.c opcode_functions.h helper_functions.h
	$(CC) $(CFLAGS) -c opcode_functions.c

opcode_table.o: opcode_table.c opcode_table.h helper_functions.h opcode_functions.h
	$(CC) $(CFLAGS) -c opcode_table.c

emu.o: emu.c cpu.h opcode_table.h
	$(CC) $(CFLAGS) -c emu.c

emu: emu.o cpu.o cart.o mappers.o helper_functions.o opcode_functions.o opcode_table.o ppu.o
	$(CC) -o emu emu.o cpu.o cart.o mappers.o helper_functions.o opcode_functions.o opcode_table.o ppu.o

clean:
	rm *.o
