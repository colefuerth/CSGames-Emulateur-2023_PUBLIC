#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * void reset6502()                                  *
 *   - Call this once before you begin execution.    *
 *                                                   *
 * void exec6502(uint32_t tickcount)                 *
 *   - Execute 6502 code up to the next specified    *
 *     count of clock ticks.                         *
 *                                                   *
 * void step6502()                                   *
 *   - Execute a single instrution.                  *
 *                                                   *
 * void irq6502()                                    *
 *   - Trigger a hardware IRQ in the 6502 core.      *
 *                                                   *
 * void nmi6502()                                    *
 *   - Trigger an NMI in the 6502 core.              *
 *                                                   *
 * void hookexternal(void *funcptr)                  *
 *   - Pass a pointer to a void function taking no   *
 *     parameters. This will cause Fake6502 to call  *
 *     that function once after each emulated        *
 *     instruction.
 */

// 808

#define MEMSIZE 65536
#define KEYADDR 0x200
#define OUTADDR 0xFFF1

uint8_t mem[65536];
uint8_t output_writes = 0;
int restart_flag = 0;

char out[49] = {0};
int writes = 0;

uint8_t read6502(uint16_t address);
void write6502(uint16_t address, uint8_t value);

uint16_t counterfunc(uint16_t address)
{
    if (address == OUTADDR)
    {
        char c = read6502(OUTADDR);
        out[writes++] = c;
        if (writes == 48)
        {
            writes = 0;
            printf("%s\n", out);
            restart_flag = 1;
        }
        return 1;
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

int main(int argc, char *argv[])
{
    // step until we write 48 times
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <path_to_binary>\n", *argv);
        exit(1);
    }

    printf("reading memory...");
    fflush(stdout);
    FILE *bin = fopen(argv[1], "r");
    if (fread(mem, sizeof(uint8_t), MEMSIZE, bin) != MEMSIZE)
    {
        perror("read error!");
        exit(2);
    }
    puts("done.");

    // we brute forced this
    char key[] = "808";
    printf("KEY: %s\n", key);
    for (int i = 0; i < 3; i++)
        write6502(KEYADDR + i, key[i]);

    restart_flag = 0;
    reset6502();
    printf("starting emulation\n");
    while (!restart_flag)
    {
        step6502();
    }
    if (read6502(OUTADDR) != '#')
    {
        return 0;
    }
}