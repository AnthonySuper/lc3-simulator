#ifndef _machine_h
#define _machine_h
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#define MACHINE_RAM_SIZE 65536

/**
 * The LC-3 specifies 16-bit words.
 */
typedef uint16_t m_word;

typedef enum{
    UNKNOWN_ERROR,
    NONEXISTANT_REGISTER,
} machine_error_t;

struct machine_error_info{
    char *message;
    machine_error_t type;
};

/**
 * A struct representing some machine that uses the LC-3 instruction set.
 * You should use machine_init to craete one of these structures, 
 * as machine_init properly allocates the RAM buffer.
 */
typedef struct _machine{
    m_word registers[8]; /**< Simulate the LC-3's eight registers */
    m_word *ram; /**< A simulated RAM buffer */
    m_word pc; /**< the program counter register, which shows what instruction
                 we will execute next */
    /**
     * LC-3 has 3 condition registers, which are set depending on the last
     * written register. We set those bits here.
     */
    unsigned char condition_n;
    unsigned char condition_z;
    unsigned char condition_p;
    /**
     * Used to longjmp to give control back after we intercept an EXIT
     * TRAP instruction
     */
    jmp_buf _end_jump;
    /**
     * A pointe rto an error we might have. Note that the only
     * current possible error is trying to write to/read from a nonexistant
     * register, which is an error that's actually only possible if the 
     * emulator itself is buggy. Client code can never trigger this, since
     * the LC-3 only uses 3 bits at a time to index a register.
     */
    struct machine_error_info *_err;
    /**
     * If this is non-null, we will print debugging information to the given
     * FILE pointer.
     */
    FILE *debug;
} machine;


/**
 * Create a new machine.
 * This heap-allocates the machine, as well as the RAM buffer needed to 
 * simulate real programs.
 * You should not free() the returned pointer, but instead use machine_destroy.
 */
machine* machine_init();

/**
 * Destroy a given machine, freeing all resources.
 *
 * Takes a double pointer as it nullifies its argument.
 */
void machine_destroy(machine**);

m_word m_read_regiter(machine*, unsigned char);
void m_write_register(machine*, unsigned char, m_word);
// INSTRUCTIONS
void mop_not(machine*, unsigned char, unsigned char);
void mop_add_immediate(machine*, unsigned char, unsigned char, unsigned char);
void mop_and_immediate(machine*, unsigned char, unsigned char, unsigned char);
void mop_and_register(machine*, unsigned char, unsigned char, unsigned char);
void mop_add_register(machine*, unsigned char, unsigned char, unsigned char);
void mop_ld(machine*, unsigned char, signed char);
void mop_st(machine*, unsigned char, signed char);
void mop_ldi(machine*, unsigned char, signed char);
void mop_sti(machine*, unsigned char, signed char);
void mop_ldr(machine*, unsigned char, unsigned char, signed char);
void mop_str(machine*, unsigned char, unsigned char, signed char);
void mop_lea(machine*, unsigned char, signed char);
void mop_br(machine*, unsigned char, unsigned char, unsigned char, signed char);
void mop_jmp(machine*, unsigned char);
void mop_trap(machine*, unsigned char);

#endif

