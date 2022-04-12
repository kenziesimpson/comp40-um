#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

#include "seq.h"
#include "uarray.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Memory *Memory;

typedef struct Segment {
    int size;
    uint32_t *data;
} Segment;

/*
 * get_segment
 *
 * Gets the address of the segment of memory stored at the given index.
 *
 * @param  memory *mem      A pointer to the memory module to access from
 * @param  int index        The index of the segment to get
 * @return uint32_t*        A pointer to the segment that was requested
 * @expect The index has been previously mapped to a segment and has not been
 *         unmapped since its mapping
 * @expect The memory pointer is not NULL and points to a valid memory module
 */
Segment *get_segment(Memory mem, int index);

/*
 * new_segment
 *
 * Requests a new segment of memory from the memory module.
 *
 * @param  memory *mem      A pointer to the memory module to request from
 * @param  int size         The size of the segment to request in words
 * @return int              The index of the newly created segment
 * @expect The memory pointer is not NULL and points to a valid memory module
 * @expect The size is greater than 0
 */
int new_segment(Memory mem, int size);

/*
 * remove_segment
 *
 * Removes a segment of memory from the memory module.
 *
 * @param  memory *mem      A pointer to the memory module to remove from
 * @param  int index        The index of the segment to remove
 * @return void
 * @expect The memory pointer is not NULL and points to a valid memory module
 * @expect The index has been previously mapped to a segment and has not been
 *         unmapped since its mapping
 */
void remove_segment(Memory mem, int index);

/*
 * new_memory_module
 *
 * Allocates and returns a new memory module. The memory module is initialized
 * with the given program as its first segment.
 *
 * @param  uint32_t *program    A pointer to the program to initialize the
 *                              memory module with
 * @param  int size             The size of the program in words
 * @return memory *             A pointer to the new memory module
 */
Memory new_memory_module(uint32_t *program, int size);

/*
 * free_memory_module
 *
 * Frees a memory module and all of its segments.
 *
 * @param  memory *mem      A pointer to the memory module to free
 * @return void
 * @expect The memory pointer is not NULL and points to a valid memory module
 */
void free_memory_module(Memory *mem);

#endif
