#include "executor.h"
#include "memory.h"
#include <assert.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#define OPCODE_WIDTH 4
#define OPCODE_LSB 28
#define NUM_INSTRUCTIONS 14

struct Executor {
    Memory memory;
    uint32_t *registers;
    uint32_t *pc;
    Status (*handlers[NUM_INSTRUCTIONS])(Executor executor,
                                         uint32_t instruction);
};

Status handle_cmov(Executor executor, uint32_t instruction);
Status handle_slod(Executor executor, uint32_t instruction);
Status handle_sstr(Executor executor, uint32_t instruction);
Status handle_adtn(Executor executor, uint32_t instruction);
Status handle_mult(Executor executor, uint32_t instruction);
Status handle_dvsn(Executor executor, uint32_t instruction);
Status handle_nand(Executor executor, uint32_t instruction);
Status handle_halt(Executor executor, uint32_t instruction);
Status handle_mseg(Executor executor, uint32_t instruction);
Status handle_useg(Executor executor, uint32_t instruction);
Status handle_outp(Executor executor, uint32_t instruction);
Status handle_inpt(Executor executor, uint32_t instruction);
Status handle_lodp(Executor executor, uint32_t instruction);
Status handle_lodv(Executor executor, uint32_t instruction);

Executor new_executor(Memory memory, uint32_t *registers, uint32_t *pc)
{
    assert(memory != NULL);
    assert(registers != NULL);

    Executor executor;
    NEW(executor);

    executor->memory = memory;
    executor->registers = registers;
    executor->pc = pc;

    executor->handlers[0] = handle_cmov;
    executor->handlers[1] = handle_slod;
    executor->handlers[2] = handle_sstr;
    executor->handlers[3] = handle_adtn;
    executor->handlers[4] = handle_mult;
    executor->handlers[5] = handle_dvsn;
    executor->handlers[6] = handle_nand;
    executor->handlers[7] = handle_halt;
    executor->handlers[8] = handle_mseg;
    executor->handlers[9] = handle_useg;
    executor->handlers[10] = handle_outp;
    executor->handlers[11] = handle_inpt;
    executor->handlers[12] = handle_lodp;
    executor->handlers[13] = handle_lodv;

    return executor;
}

void free_executor(Executor *executor)
{
    assert(executor != NULL && *executor != NULL);

    Executor dexecutor = *executor;
    FREE(dexecutor);
}

Status Executor_process(Executor executor, uint32_t instruction)
{
    assert(executor != NULL);

    uint32_t opcode = Bitpack_getu(instruction, OPCODE_WIDTH, OPCODE_LSB);

    return executor->handlers[opcode](executor, instruction);
}

Status handle_cmov(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    if (reg[rc] != 0)
        reg[ra] = reg[rb];

    return CONT;
}

Status handle_slod(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;
    Memory mem = executor->memory;

    reg[ra] = (get_segment(mem, reg[rb])->data)[reg[rc]];

    return CONT;
}

Status handle_sstr(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    get_segment(executor->memory, reg[ra])->data[reg[rb]] = reg[rc];

    return CONT;
}

Status handle_adtn(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    reg[ra] = reg[rb] + reg[rc];

    return CONT;
}

Status handle_mult(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    reg[ra] = reg[rb] * reg[rc];

    return CONT;
}

Status handle_dvsn(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    reg[ra] = reg[rb] / reg[rc];

    return CONT;
}

Status handle_nand(Executor executor, uint32_t instruction)
{
    uint32_t ra = Bitpack_getu(instruction, 3, 6);
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    reg[ra] = ~(reg[rb] & reg[rc]);

    return CONT;
}

Status handle_halt(Executor executor, uint32_t instruction)
{
    (void)executor;
    (void)instruction;

    return HALT;
}

Status handle_mseg(Executor executor, uint32_t instruction)
{
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    reg[rb] = new_segment(executor->memory, reg[rc]);

    return CONT;
}

Status handle_useg(Executor executor, uint32_t instruction)
{
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    remove_segment(executor->memory, executor->registers[rc]);

    return CONT;
}

Status handle_outp(Executor executor, uint32_t instruction)
{
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;

    assert(0 <= (int)reg[rc] && (int)reg[rc] < 256);
    putc((char)(reg[rc]), stdout);

    return CONT;
}

Status handle_inpt(Executor executor, uint32_t instruction)
{
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    int c = getchar();

    assert(c >= -1 && c < 256);

    executor->registers[rc] = (c == -1 ? ~(0u) : (uint32_t)c);

    return CONT;
}

Status handle_lodp(Executor executor, uint32_t instruction)
{
    uint32_t rb = Bitpack_getu(instruction, 3, 3);
    uint32_t rc = Bitpack_getu(instruction, 3, 0);

    uint32_t *reg = executor->registers;
    Memory mem = executor->memory;

    uint32_t rbv = reg[rb];
    uint32_t rcv = reg[rc];

    if (rbv != 0) {
        // Get segments to operate on
        Segment *seg = get_segment(mem, rbv);
        Segment *prog_seg = get_segment(mem, 0);

        // Duplicate the segment
        uint32_t *new_data = malloc(seg->size * sizeof(uint32_t));
        for (int i = 0; i < seg->size; i++)
            new_data[i] = seg->data[i];

        // Free old program and set new program
        free(prog_seg->data);
        prog_seg->data = new_data;
        prog_seg->size = seg->size;
    }
    // Set program counter
    *executor->pc = rcv;

    return CONT;
}

Status handle_lodv(Executor executor, uint32_t instruction)
{
    // printf("loading!\n");
    uint32_t ra = Bitpack_getu(instruction, 3, 25);
    uint32_t value = Bitpack_getu(instruction, 25, 0);

    executor->registers[ra] = value;

    return CONT;
}
