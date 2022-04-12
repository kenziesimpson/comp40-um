#include <stdlib.h>
typedef struct Array_T {
} Array_T;
typedef struct Seq_T {
} Seq_T;

// Executor -> memory

struct Memory {
    Array_T segments;
    Seq_T unmapped_ids;
    long highest_id;
};

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
uint32_t *get_segment(Memory *mem, int index);

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
int new_segment(Memory *mem, int size);

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
void remove_segment(Memory *mem, int index);

// Top-level -> memory

/*
 * new_memory_module
 *
 * Allocates and returns a new memory module.
 *
 * @param  void
 * @return memory *         A pointer to the new memory module
 */
Memory *new_memory_module(void);

/*
 * free_memory_module
 *
 * Frees a memory module and all of its segments.
 *
 * @param  memory *mem      A pointer to the memory module to free
 * @return void
 * @expect The memory pointer is not NULL and points to a valid memory module
 */
void free_memory_module(Memory **mem);

// Top-level -> executor

struct Executor {
    Memory *mem;
    uint32_t *registers;
};

/*
 * new_executor
 *
 * Create a new executor.
 *
 * @param Memory *mem           The memory module to use
 * @param uint32_t *registers   The array of registers to use
 * @return the new executor
 * @expect The memory module and registers are not NULL and have been
 *         initialized
 * @expect There are exactly 8 registers
 */
Executor *new_executor(Memory *mem, uint32_t *registers);

/*
 * process
 *
 * Executes a program instruction on the current state of memory and registers.
 *
 * @param  uint32_t *instruction   A pointer to the instruction to execute
 * @return int                     The result of the the instruction execution
 *                                 0: the instruction was executed successfully
 *                                 1: the instruction finished with an error
 *                                 2: the instruction was a halt instruction
 * @expect The instruction is formatted correctly according to the UM spec
 */
int process(uint32_t *instruction);
