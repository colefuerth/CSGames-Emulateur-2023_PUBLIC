/**
 * MODIFIED THE atlantis.bin TO RUN FIB(10), PUTS RESULT TO FFF1, AND PRINTS AND EXITS
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MEMSIZE 65536
#define OUTADDR 0xFFF1
#define OUTPUTFILE

uint8_t mem[65536];

int restart_flag = 0;

uint8_t read6502(uint16_t address);
void write6502(uint16_t address, uint8_t value);

// we need to catch when we get an output with this
uint16_t counterfunc(uint16_t address)
{
    if (address == OUTADDR && mem[OUTADDR] != 0)
    {
        printf("fib(10): %d\n", read6502(0xfff1));
        restart_flag = 1;
    }
    return 0;
}

// externally supplied functions
uint8_t read6502(uint16_t address)
{
    return mem[address];
}
void write6502(uint16_t address, uint8_t value)
{
    mem[address] = value;
    counterfunc(address);
}

uint8_t mem[65536];
int main(int argc, char *argv[])
{
    // takes the base atlantis input and also the fib binary to modify
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <path_to_input <path_to_fib>\n", *argv);
        exit(1);
    }

    printf("reading memory...");
    fflush(stdout);
    FILE *fin = fopen(argv[1], "r");
    int size;
    if ((size = fread(mem, sizeof(uint8_t), MEMSIZE, fin)) != MEMSIZE)
    {
        perror("read error!");
        exit(2);
    }
    fclose(fin);
    puts("done.");

    // modify mem to do fib
    write6502(0xFFFC, 0);
    write6502(0xFFFD, 0);
    for (int i = 0; i < size; i++)
    {
        write6502(i, mem[size]);
    }

    FILE *ffib = fopen(argv[2], "r");
    size = fread(mem, 1, MEMSIZE, ffib);
    fclose(ffib);

    // go until exit requirement is reached 
    
    restart_flag = 0;
    reset6502();
    printf("starting emulation\n");
    while (!restart_flag)
    {
        step6502();
    }

    exit(0);
}