#include "memory.h"
#include "utest.h"
#include <except.h>

struct Fixture {
    Memory mem;
};

UTEST_F_SETUP(Fixture) { utest_fixture->mem = new_memory_module(NULL, 0); }

UTEST_F_TEARDOWN(Fixture) { free_memory_module(&utest_fixture->mem); }

UTEST_F(Fixture, ConstructorAndTeardown) { EXPECT_TRUE(1); }

UTEST_F(Fixture, NewSegment)
{
    Memory mem = utest_fixture->mem;
    int id = new_segment(mem, 10);
}

UTEST_F(Fixture, MultipleNewSegments)
{
    Memory mem = utest_fixture->mem;
    for (int i = 0; i < 32; i++)
        new_segment(mem, 10);
}

UTEST_F(Fixture, RemoveSegment)
{
    Memory mem = utest_fixture->mem;
    int id = new_segment(mem, 10);
    remove_segment(mem, id);
}

UTEST_F(Fixture, RemoveMultipleSegments)
{
    Memory mem = utest_fixture->mem;
    int ids[64];
    for (int i = 0; i < 64; i++)
        ids[i] = new_segment(mem, 32);

    for (int i = 0; i < 64; i++)
        remove_segment(mem, ids[i]);
}

UTEST_F(Fixture, ReassignIds)
{
    Memory mem = utest_fixture->mem;
    int ids[64];
    // Create 64 segments
    for (int i = 0; i < 64; i++)
        ids[i] = new_segment(mem, 32);
    // Remove all segments
    for (int i = 0; i < 64; i++)
        remove_segment(mem, ids[i]);
    // Create another 64 segments
    for (int i = 0; i < 64; i++)
        ids[i] = new_segment(mem, 32);
    // Ensure all IDs were reassigned
    for (int i = 0; i < 64; i++)
        EXPECT_LE(ids[i], 64);
}

UTEST_F(Fixture, UniqueIds)
{
    Memory mem = utest_fixture->mem;
    int ids[64];
    // Create 64 segments
    for (int i = 0; i < 64; i++)
        ids[i] = new_segment(mem, 32);
    // Ensure all IDs are unique
    for (int i = 0; i < 64; i++)
        for (int j = i + 1; j < 64; j++)
            EXPECT_NE(ids[i], ids[j]);
}

UTEST_F(Fixture, GetSegment)
{
    Memory mem = utest_fixture->mem;
    int id = new_segment(mem, 10);
    Segment *seg = get_segment(mem, id);
    EXPECT_EQ(seg->size, 10);
    for (int i = 0; i < 10; i++)
        EXPECT_EQ(seg->data[i], 0u);
}

UTEST_F(Fixture, PersistentSegments)
{
    Memory mem = utest_fixture->mem;
    int id = new_segment(mem, 10);
    Segment *seg = get_segment(mem, id);
    seg->data[0] = 1;
    seg->data[1] = 2;
    seg = get_segment(mem, id);
    EXPECT_EQ(seg->data[0], 1u);
    EXPECT_EQ(seg->data[1], 2u);
}
