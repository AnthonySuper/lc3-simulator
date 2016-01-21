#ifndef _run_h
#define _run_h
#include "machine.h"
#include <setjmp.h>

void execute_program(machine* m, m_word start_instruction);
#endif
