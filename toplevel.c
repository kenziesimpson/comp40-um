#include "executor.h"
#include "memory.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void print_prog(uint32_t *prog, uint32_t len)
{
    for (unsigned i = 0; i < len; i++)
        printf("%d: %0X\n", i, prog[i]);
}

void print_state_of_affairs(uint32_t instruction, uint32_t *registers)
{
    printf("Executing %0X\n", instruction);
    for (uint32_t i = 0; i < 8; i++) {
        printf("register %u: %u\n", i, registers[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <program>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *program = argv[1];
    FILE *fp = fopen(program, "rb");

    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s\n", program);
        return EXIT_FAILURE;
    }

    // Get the size of the file
    struct stat st;
    stat(program, &st);
    size_t size = st.st_size / 4;

    uint32_t *prog = malloc(size * sizeof(uint32_t));

    for (size_t i = 0; i < size; i++) {
        uint8_t in = fgetc(fp);
        uint32_t out = in << 24;
        in = fgetc(fp);
        out |= in << 16;
        in = fgetc(fp);
        out |= in << 8;
        in = fgetc(fp);
        out |= in;
        prog[i] = out;
    }
    fclose(fp);

    // print_prog(prog, size);

    // Initialize the memory
    Memory memory = new_memory_module(prog, size);
    uint32_t *registers = malloc(8 * sizeof(uint32_t));
    for (int i = 0; i < 8; i++)
        registers[i] = 0;
    uint32_t pc = 0;

    Executor executor = new_executor(memory, registers, &pc);

    // Run the program
    Segment *program_segment;
    uint32_t instruction;
    Status status;
    do {
        program_segment = get_segment(memory, 0);
        instruction = program_segment->data[pc++];
        // print_state_of_affairs(instruction, registers);
        status = Executor_process(executor, instruction);
    } while (status != HALT);

    free_executor(&executor);
    free_memory_module(&memory);
    free(registers);

    return EXIT_SUCCESS;
}