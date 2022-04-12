#ifndef EXECUTOR_INCLUDED
#define EXECUTOR_INCLUDED

#include "bitpack.h"
#include "memory.h"
#include <stdlib.h>

typedef struct Executor *Executor;
typedef enum Status { CONT, HALT } Status;

/*
 * Executor_new
 *
 * Create a new executor.
 *
 * @param Memory *mem           The memory module to use
 * @param uint32_t *registers   The array of registers to use
 * @param uint32_t *pc          The program counter to use
 * @return the new executor
 * @expect The memory module and registers are not NULL and have been
 *         initialized
 * @expect There are exactly 8 registers
 */
Executor new_executor(Memory memory, uint32_t *registers, uint32_t *pc);

/*
 * Executor_free
 *
 * Free an executor. None of the executor's fields are freed, so the client is
 * responsible for freeing the memory module, registers and program counter.
 *
 * @param Executor *exec    A pointer to the executor to free
 * @expect The executor is not NULL
 */
void free_executor(Executor *exec);

/*
 * Executor_process
 *
 * Executes a program instruction on the current state of memory and registers.
 *
 * @param  uint32_t instruction    The instruction to execute
 * @return Status                  The result of the the instruction execution
 *                                 CONT: the instruction was executed
 *                                       successfully, continue program
 *                                 HALT: the instruction was a halt instruction
 *                                 FAIL: the instruction finished with an error
 * @expect The instruction is formatted correctly according to the UM spec
 */
Status Executor_process(Executor executor, uint32_t instruction);

#endif