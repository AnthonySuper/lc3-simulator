#include "machine.h"

machine* machine_init(){
    machine *m = malloc(sizeof(machine));
    m->ram = malloc(MACHINE_RAM_SIZE * sizeof(m_word));
    m->pc = 0;
    m->debug = 0;
    return m;
}

void machine_destroy(machine** m){
    free((*m)->ram);
    free(*m);
    *m = NULL;
}



void m_register_range_check(machine *m, unsigned char r){
    if(r >= 8){
        m->_err = malloc(sizeof(struct machine_error_info));
        // Eh, who needs Windows compatibility
        asprintf(&(m->_err->message),
                "Tried to access nonexistant register %i",
                r);
        m->_err->type = NONEXISTANT_REGISTER;
        longjmp(m->_end_jump, -1);
    }

}

m_word m_read_register(machine *m, unsigned char r){
    m_register_range_check(m,r);
    return m->registers[r];
}

void m_write_register(machine *m, unsigned char r, m_word val){
    m_register_range_check(m,r);
    m->registers[r] = val;
    if(val == 0){
        m->condition_z = 1;
        m->condition_p = 0;
        m->condition_n = 0;
    }
    else if(val >> 15 == 1){
        m->condition_z = 0;
        m->condition_p = 0;
        m->condition_n = 1;
    }
    else{
        m->condition_z = 0;
        m->condition_p = 1;
        m->condition_n = 0;
    }
}

/**
 * Execute a NOT instruction
 */
void mop_not(machine* m, unsigned char dst, unsigned char src){
    if(m->debug){
        fprintf(m->debug,
                "NOT\t%hhx\t%hhx\n",

                dst, src);
    }
    m_write_register(m, dst, ~(m_read_register(m, src)));
}

/**
 * Execute an AND instruction in immediate mode
 */
void mop_add_immediate(machine *m,
        unsigned char dst,
        unsigned char src,
        unsigned char val){
    if(m->debug){
        fprintf(m->debug,
                "ADD IMMEDIATE\t%hhx\t%hhx\t%hhx\n",
                dst, src, val);
    }
    m_write_register(m, dst, (m_read_register(m, src)) + val);
}

/**
 * Execute an AND instruction in immediate mode
 */
void mop_and_immediate(machine *m,
        unsigned char dst,
        unsigned char src,
        unsigned char val){
    if(m->debug){
        fprintf(m->debug,
                "AND IMMEDIATE\t%hhx\t%hhx\t%hhx\n",
                dst, src, val);
    }
    m_write_register(m, dst, (m_read_register(m, src) & val));
}
/**
 * Excute an AND instruction that loads from a register
 */
void mop_and_register(machine* m,
        unsigned char dst,
        unsigned char src1,
        unsigned char src2){
    m_word value = m_read_register(m, src1) & m_read_register(m, src2);
    if(m->debug){
        fprintf(m->debug,
                "ADD REGISTER\t%hhx\t%hhx\t%hhx\t(%hx)\n",
                dst, src1, src2, value);
    }
    m_write_register(m, dst, value);
}

/**
 * Execute a ADD instruction that loads from a register
 */
void mop_add_register(machine *m,
        unsigned char dst,
        unsigned char src1,
        unsigned char src2){
    m_word value = m_read_register(m, src1) + m_read_register(m, src2);
    if(m->debug){
        
        fprintf(m->debug,
                "ADD REGISTER\t%hhx\t%hhx\t%hhx\t(%hx)\n",
                dst, src1, src2, value);
    }
    m_write_register(m, dst, value);
}

void mop_ld(machine *m, unsigned char dst, signed char offset){
    m_word total_offset = m->pc + offset;
    if(m->debug){
        fprintf(m->debug,
                "LD\t%hhx\t%hhx\t%hhx\t(%hx)\n",
                dst, offset, offset, total_offset);
    }
    m_write_register(m, dst, m->ram[total_offset]);
}

void mop_st(machine *m, unsigned char src, signed char offset){
    m_word total_offset = m->pc + offset;
    m->ram[total_offset] = m->registers[src];
}

// Read from address OFFSET words from PC
// Take that value as a pointer to elsewhere in RAM
// Read from this location and store in DST
void mop_ldi(machine *m, unsigned char dst, signed char offset){
    m_word initial_address = m->pc + offset;
    m_word final_address = m->ram[initial_address];
    if(m->debug){
        fprintf(m->debug,
                "LDI\t%hhx\t%hhx\t(%hx)\t(%hx)\n",
                dst, offset, initial_address, final_address);
    }
    m_write_register(m, dst, m->ram[final_address]);
}

// Rread from address OFFSET words from PC
// Take that value as a pointer to elsewhere in RAM
// Seet that location with the value of register SRC
void mop_sti(machine *m, unsigned char src, signed char offset){
    m_word initial_address = m->pc + offset;
    m_word final_address = m->ram[initial_address];
    if(m->debug){
        fprintf(m->debug,
                "STI\t%hhx\t%hhx\t(%hx)\t(%hx)\n",
                src, offset, initial_address, final_address);
    }
    m->ram[final_address] = m->registers[src];
}

// Read an address from register BASE,
// add OFFSET,
// store in register DST
void mop_ldr(machine *m, 
        unsigned char dst,
        unsigned char base,
        signed char offset){
    m_word total_offset = m->registers[base] + offset;
    if(m->debug){
        fprintf(m->debug,
                "LDR\t%hhx\t%hhx\t%hhx\t(%hx)\n",
                dst, base, offset, total_offset);
    }
    m_write_register(m, dst, m->ram[total_offset]);
}

// Read an address from register BASE,
// add OFFSET,
// write value from SRC into address
void mop_str(machine *m,
        unsigned char src,
        unsigned char base,
        signed char offset){
    m_word total_offset = m->registers[base] + offset;
    if(m->debug){
        fprintf(m->debug,
                "STR\t%hhx\t%hhx\t%hhx\t(%hx)",
                src, base, offset, total_offset);
    }
    m->ram[total_offset] = m->registers[src];
}

void mop_lea(machine *m,
        unsigned char dst,
        signed char offset){
    if(m->debug){
        fprintf(m->debug,
                "LEA\t%hhx\t%hhx\t(%x)",
                dst, offset, ((m_word) m->pc + offset));
    }
    m_write_register(m, dst, m->pc + offset);
}


void mop_br(machine *m,
        unsigned char n,
        unsigned char z,
        unsigned char p,
        signed char offset){
    if(m->debug){
        fprintf(m->debug,
                "BR\t%hhx\t%hhx\t%hhx\t%hhx",
                n, z, p, offset);
    }
    if((n && m->condition_n) ||
            (z && m->condition_z) ||
            (p && m->condition_p)){
        m->pc = m->pc + offset;
    }
}

void mop_jmp(machine *m, unsigned char base){
    if(m->debug){
        fprintf(m->debug,
                "JMP\t%hhx\n",
                base);
    }
    m->pc = m->registers[base];
}

void mtrap_getc(machine *m){
    char c = getchar();
    if(m->debug){
        fprintf(m->debug,
                "GETCH\t(%c)\n",
                c);
    }
    m_write_register(m, 0, c);
}

void mtrap_output(machine *m){
    putchar(m->registers[0]);
}

void mtrap_puts(machine *m){
    printf("%s",(const char *) &(m->ram[m->registers[0]]));
}

// Exit the program
void mtrap_halt(machine *m){
    longjmp(m->_end_jump, 1);
}

void mop_trap(machine *m, unsigned char routine){
    if(m->debug){
        fprintf(m->debug,
                "TRAP\t%hhx\n",
                routine);
    }
    switch(routine){
        case 0x20:
            mtrap_getc(m);
            break;
        case 0x21:
            mtrap_output(m);
            break;
        case 0x22:
            mtrap_puts(m);
            break;
        case 0x25:
            mtrap_halt(m);
            break;
        default:
            break;
    }
}

