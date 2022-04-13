/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.
 *
 * Any additional functions and unit tests written for the lab go
 * here.
 *
 */

#include <assert.h>
#include <bitpack.h>
#include <seq.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t Um_instruction;
typedef enum Um_opcode {
    CMOV = 0,
    SLOAD,
    SSTORE,
    ADD,
    MUL,
    DIV,
    NAND,
    HALT,
    ACTIVATE,
    INACTIVATE,
    OUT,
    IN,
    LOADP,
    LV
} Um_opcode;

/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);

/* Wrapper functions for each of the instructions */

static inline Um_instruction sload(Um_register a, Um_register b, Um_register c)
{
    return three_register(SLOAD, a, b, c);
}

static inline Um_instruction sstore(Um_register a, Um_register b, Um_register c)
{
    return three_register(SSTORE, a, b, c);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c)
{
    return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, Um_register c)
{
    return three_register(MUL, a, b, c);
}

static inline Um_instruction halt(void)
{
    return three_register(HALT, r0, r0, r0);
}

static inline Um_instruction map(Um_register b, Um_register c)
{
    return three_register(ACTIVATE, r0, b, c);
}

static inline Um_instruction unmap(Um_register c)
{
    return three_register(INACTIVATE, r0, r0, c);
}

static inline Um_instruction output(Um_register c)
{
    return three_register(OUT, r0, r0, c);
}

statis inline Um_instruction input(Um_register c)
{
    return three_register(IN, r0, r0, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
    assert(sizeof(inst) <= sizeof(uintptr_t));
    Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
    assert(output != NULL && stream != NULL);
    int stream_length = Seq_length(stream);
    for (int i = 0; i < stream_length; i++) {
        Um_instruction inst = (uintptr_t)Seq_remlo(stream);
        for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
            fputc(Bitpack_getu(inst, 8, lsb), output);
        }
    }
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
    uint32_t instr = Bitpack_newu(0, 4, 28, op);
    instr = Bitpack_newu(instr, 3, 0, rc);
    instr = Bitpack_newu(instr, 3, 3, rb);
    instr = Bitpack_newu(instr, 3, 6, ra);
    return instr;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
    uint32_t instr = Bitpack_newu(0, 4, 28, LV);
    instr = Bitpack_newu(instr, 3, 25, ra);
    instr = Bitpack_newu(instr, 25, 0, val);
    return instr;
}

/* Unit tests for the UM */

// ensures the machine stops at the end of a program
void build_halt_test(Seq_T stream) { append(stream, halt()); }

// ensures the machine doesn't execute anything after a halt
// ensures the machine can load values
// ensures the machine can output values
void build_output_test(Seq_T stream) {
    append(stream, loadval(r1, '1'));
    append(stream, output(r1));
    append(stream, halt());
    append(stream, loadval(r1, '2'));
    append(stream, output(r1));
}

// ensures the machine can't output values larger than 255
// this test fails
void build_output_fail(Seq_T steam) {
    append(stream, loadval(r1, 256));
    append(stream, output(41));
    append(stream, halt());
}

// ensures the machine adds two numbers properly
void build_add_test(Seq_T stream)
{
    append(stream, loadval(r1, 48));
    append(stream, loadval(r2, 1));
    append(stream, add(r3, r1, r2));
    append(stream, output(r3));
    append(stream, halt());
}

// ensures the machine mods added numbers by 2^32
void build_round_test(Seq_T stream)
{
    append(stream, loadval(r1, 48));
    append(stream, loadval(r2, 2));
    append(stream, add(r3, r1, r2));
    append(stream, loadval(r4, 0xFFFFFFFE));
    append(stream, add(r5, r3, r4));
    append(stream, output(r5));
    append(stream, halt());
}

// ensures segments can be mapped
// ensures segments can be unmapped
void build_mapping_test(Seq_T stream)
{
    append(stream, loadval(r0, 1));
    append(stream, map(r1, r0));
    append(stream, unmap(r1));
    append(stream, halt());
}

// ensures all words in a segment can be stored in
// ensures segments can be read from
void build_loading_test(Seq_T stream)
{
    append(stream, loadval(r0, 2));
    append(stream, map(r1, r0));
    append(stream, loadval(r2, 0));
    append(stream, loadval(r3, '1'));
    append(stream, sstore(r1, r2, r3));
    append(stream, sload(r4, r1, r2));
    append(stream, output(r4));
    append(stream, loadval(r5, 1));
    append(stream, sstore(r1, r5, r3));
    append(stream, halt());
}

// ensures the machine handles input correctly
void build_input_test(Seq_T stream)
{
    append(stream, input(r0));
    append(stream, output(r0));
    append(stream, halt());
}
