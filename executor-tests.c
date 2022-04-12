#include "executor.h"
#include "utest.h"
#include <except.h>

struct Fixture {
    Executor executor;
    Memory mem;
    uint32_t *reg;
    uint32_t *pc;
};

UTEST_F_SETUP(Fixture)
{
    utest_fixture->mem = new_memory_module(NULL, 0);
    utest_fixture->reg = malloc(sizeof(uint32_t) * 8);
    for (int i = 0; i < 8; i++)
        utest_fixture->reg[i] = 0;
    utest_fixture->pc = malloc(sizeof(uint32_t));
    *utest_fixture->pc = 0;
    utest_fixture->executor =
        new_executor(utest_fixture->mem, utest_fixture->reg, utest_fixture->pc);
}

UTEST_F_TEARDOWN(Fixture)
{
    free_memory_module(&utest_fixture->mem);
    free_executor(&utest_fixture->executor);
    free(utest_fixture->reg);
    free(utest_fixture->pc);
}

UTEST_F(Fixture, ConstructorAndTeardown) { EXPECT_TRUE(1); }

UTEST_F(Fixture, ConditionalMoveInstructionFalse)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // Conditional move, A=0, B=1, C=2
    uint32_t instruction = 0x0000000A;

    // Setup registers r[A]=0, r[B]=FLAG, r[C]=0
    uint32_t FLAG = 0x8F8F8F8F;
    reg[1] = FLAG;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], 0);
    EXPECT_EQ(reg[1], FLAG);
    EXPECT_EQ(reg[2], 0);
}

UTEST_F(Fixture, ConditionalMoveInstructionTrue)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // Conditional move, A=0, B=1, C=2
    uint32_t instruction = 0x0000000A;

    // Setup registers r[A]=0, r[B]=FLAG, r[C]=1
    uint32_t FLAG = 0x8F8F8F8F;
    reg[1] = FLAG;
    reg[2] = 1;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], FLAG);
    EXPECT_EQ(reg[1], FLAG);
    EXPECT_EQ(reg[2], 1);
}

UTEST_F(Fixture, SegmentedLoadInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;
    Memory mem = utest_fixture->mem;

    // Segmented load, A=0, B=1, C=2
    uint32_t instruction = 0x1000000A;

    // Map a segment
    int seg_id = new_segment(mem, 32);

    // Setup values m[r[B]][r[C]]=FLAG
    uint32_t FLAG = 0x8F8F8F8F;
    reg[1] = seg_id;
    reg[2] = 16;
    get_segment(mem, reg[1])->data[reg[2]] = FLAG;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], FLAG);
    EXPECT_EQ(reg[1], seg_id);
    EXPECT_EQ(reg[2], 16);
    EXPECT_EQ(get_segment(mem, reg[1])->data[reg[2]], FLAG);
}

UTEST_F(Fixture, SegmentedStoreInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;
    Memory mem = utest_fixture->mem;

    // Segmented store, A=0, B=1, C=2
    uint32_t instruction = 0x2000000A;

    // Map a segment
    int seg_id = new_segment(mem, 32);

    // Setup values r[A]=seg_id, r[B]=16, r[C]=FLAG
    uint32_t FLAG = 0x8F8F8F8F;
    reg[0] = seg_id;
    reg[1] = 16;
    reg[2] = FLAG;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], seg_id);
    EXPECT_EQ(reg[1], 16);
    EXPECT_EQ(reg[2], FLAG);
    EXPECT_EQ(get_segment(mem, reg[0])->data[reg[1]], reg[2]);
}

UTEST_F(Fixture, AddInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // Add, A=0, B=1, C=2
    uint32_t instruction = 0x3000000A;

    // Setup registers r[A]=0, r[B]=num1, r[C]=num2
    uint32_t num1 = 0x8F8F8F8F;
    uint32_t num2 = 0xA4A4A4A4;
    reg[1] = num1;
    reg[2] = num2;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], num1 + num2);
    EXPECT_EQ(reg[1], num1);
    EXPECT_EQ(reg[2], num2);
}

UTEST_F(Fixture, MultiplyInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // Multiply, A=0, B=1, C=2
    uint32_t instruction = 0x4000000A;

    // Setup registers r[A]=0, r[B]=num1, r[C]=num2
    uint32_t num1 = 0x8F8F8F8F;
    uint32_t num2 = 0xA4A4A4A4;
    reg[1] = num1;
    reg[2] = num2;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], num1 * num2);
    EXPECT_EQ(reg[1], num1);
    EXPECT_EQ(reg[2], num2);
}

UTEST_F(Fixture, DivideInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // Divide, A=0, B=1, C=2
    uint32_t instruction = 0x5000000A;

    // Setup registers r[A]=0, r[B]=num1, r[C]=num2
    uint32_t num1 = 0x8F8F8F8F;
    uint32_t num2 = 0xA4A4A4A4;
    reg[1] = num1;
    reg[2] = num2;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], num1 / num2);
    EXPECT_EQ(reg[1], num1);
    EXPECT_EQ(reg[2], num2);
}

UTEST_F(Fixture, BitwiseNANDInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // Bitwise NAND, A=0, B=1, C=2
    uint32_t instruction = 0x6000000A;

    // Setup registers r[A]=0, r[B]=num1, r[C]=num2
    uint32_t num1 = 0x8F8F8F8F;
    uint32_t num2 = 0xA4A4A4A4;
    reg[1] = num1;
    reg[2] = num2;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], ~(num1 & num2));
    EXPECT_EQ(reg[1], num1);
    EXPECT_EQ(reg[2], num2);
}

UTEST_F(Fixture, HaltInstruction)
{
    Executor executor = utest_fixture->executor;

    // Halt
    uint32_t instruction = 0x70000000;

    EXPECT_EQ(Executor_process(executor, instruction), HALT);
}

UTEST_F(Fixture, MapSegmentInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;
    Memory mem = utest_fixture->mem;

    // MapSegment, A=0, B=1, C=2
    uint32_t instruction = 0x8000000A;

    // Setup registers r[B]=0, r[C]=size
    uint32_t size = 0x80;
    reg[2] = size;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    int id = reg[1];
    EXPECT_EQ(get_segment(mem, id)->size, size);
    EXPECT_EQ(reg[2], size);

    // Attempt access
    for (int i = 0; i < size; i++)
        EXPECT_EQ(get_segment(mem, id)->data[i], 0);
}

UTEST_F(Fixture, UnmapSegmentInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;
    Memory mem = utest_fixture->mem;

    // UnmapSegment, A=0, B=1, C=2
    uint32_t instruction = 0x9000000A;

    int id = new_segment(mem, 0x80);

    // Setup registers r[B]=0, r[C]=size
    reg[2] = id;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ((uint64_t)get_segment(mem, id)->data, 0);
    EXPECT_EQ(reg[2], id);
}

UTEST_F(Fixture, LoadProgramInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;
    Memory mem = utest_fixture->mem;
    uint32_t *pc = utest_fixture->pc;

    // LoadProgram, A=0, B=1, C=2
    uint32_t instruction = 0xC000000A;

    uint32_t FLAG = 0x8F8F8F8F;

    // Setup new program segment
    int prog_id = new_segment(mem, 16);
    // Set arbitrary data to check
    get_segment(mem, prog_id)->data[15] = FLAG;

    // Setup registers r[B]=prog_id, r[C]=1
    reg[1] = prog_id;
    reg[2] = 4;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[1], prog_id);
    EXPECT_EQ(reg[2], 4);

    // Check that pc is set correctly
    EXPECT_EQ(*pc, 4u);

    // Check that both segments are intact
    for (int i = 0; i < 15; i++) {
        EXPECT_EQ(get_segment(mem, 0)->data[i], 0);
        EXPECT_EQ(get_segment(mem, prog_id)->data[i], 0);
    }
    EXPECT_EQ(get_segment(mem, 0)->data[15], FLAG);
    EXPECT_EQ(get_segment(mem, prog_id)->data[15], FLAG);

    // Check that the segment was actually duplicated
    EXPECT_NE(get_segment(mem, prog_id), get_segment(mem, 0));
}

UTEST_F(Fixture, LoadValueInstruction)
{
    uint32_t *reg = utest_fixture->reg;
    Executor executor = utest_fixture->executor;

    // LoadValue, A=0, value=0x8F8F
    uint32_t val = 0x8F8F;
    uint32_t instruction = 0xD0000000 | val;

    EXPECT_EQ(Executor_process(executor, instruction), CONT);
    EXPECT_EQ(reg[0], val);
}
