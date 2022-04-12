#include "memory.h"

struct Memory {
    UArray_T segments;
    Seq_T unmapped_ids;
    long highest_id;
};

Memory new_memory_module(uint32_t *program, int size)
{
    Memory mem = malloc(sizeof(struct Memory));
    mem->segments = UArray_new(16, sizeof(Segment));
    mem->unmapped_ids = Seq_new(16);
    *((Segment *)UArray_at(mem->segments, 0)) = (Segment){size, program};
    mem->highest_id = 1;

    return mem;
}

void free_memory_module(Memory *mem)
{
    // Free all segments
    for (int i = 0; i < UArray_length((*mem)->segments); i++) {
        Segment *segment = UArray_at((*mem)->segments, i);
        free(segment->data);
    }
    UArray_free(&(*mem)->segments);

    // Free the unmapped ids sequence
    while (Seq_length((*mem)->unmapped_ids) > 0)
        free(Seq_remhi((*mem)->unmapped_ids));
    Seq_free(&(*mem)->unmapped_ids);

    // Free the memory module
    free(*mem);

    // Set client's pointer to null
    *mem = NULL;
}

Segment *get_segment(Memory mem, int index)
{
    return UArray_at(mem->segments, index);
}

int new_segment(Memory mem, int size)
{
    int id = -1;

    // If there are no unmapped ids, increment highest id
    if (Seq_length(mem->unmapped_ids) == 0)
        id = mem->highest_id++;

    // else get an id from the unmapped ids
    else {
        int *idptr = Seq_remhi(mem->unmapped_ids);
        id = *idptr;
        free(idptr);
    }

    assert(id >= 0);

    // Resize segment array if needed
    int curr_len = UArray_length(mem->segments);
    if (id >= curr_len)
        UArray_resize(mem->segments, curr_len * 2);

    // Allocate segment
    uint32_t *data = malloc(size * sizeof(uint32_t));
    for (int i = 0; i < size; i++)
        data[i] = 0;
    Segment *segment = UArray_at(mem->segments, id);
    segment->data = data;
    segment->size = size;

    return id;
}

void remove_segment(Memory mem, int index)
{
    // Free segment data
    Segment *segment = UArray_at(mem->segments, index);
    free(segment->data);
    segment->data = NULL;
    segment->size = -1;

    // Push id onto unmapped ids stack
    int *id = malloc(sizeof(int));
    *id = index;
    Seq_addhi(mem->unmapped_ids, id);
}
