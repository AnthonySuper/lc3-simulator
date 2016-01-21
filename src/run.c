#include "run.h"


static inline m_word _extract_bits(m_word val, 
        unsigned start, 
        unsigned end){
    // Partially taken from SO
    m_word r = 0;
    for(unsigned i = start; i <= end; i++){
        r |= (1 << i);
    }
    m_word s_val = (val & r);
    m_word shift_length = start;
    return (s_val) >> (shift_length);
}

// Adding subroutine
// Used in case we have an instant mode addition
static void _add_sub(machine* m, m_word inst){
    // Immediate mode
    if(_extract_bits(inst, 5, 5)){
        mop_add_immediate(m,
                _extract_bits(inst, 9, 11),
                _extract_bits(inst, 6, 8),
                _extract_bits(inst, 0, 4));
    }
    // Reigster mode
    else{
        mop_add_register(m,
                _extract_bits(inst, 9, 11),
                _extract_bits(inst, 6, 8),
                _extract_bits(inst, 0, 2));
    }
}


// AND subroutine
// used in case we have an instant mode AND
static void _and_sub(machine *m, m_word inst){
    // Immediate mode
    if(_extract_bits(inst, 5, 5)){
        mop_and_immediate(m,
                _extract_bits(inst, 9, 11),
                _extract_bits(inst, 6, 8),
                _extract_bits(inst, 0, 4));
    }
    // register mode
    else{
        mop_and_register(m,
                _extract_bits(inst, 9, 11),
                _extract_bits(inst, 6, 8),
                _extract_bits(inst, 0, 2));
    }
}
// convert the value to a signed offset of LENGTH bits
static signed char _signed_offset(m_word val, unsigned char length){
    // Negative number, use 1-padding
    if(val & (1 << length)){
        for(int i = 0; i < (16 - length); i++){
            val |= (1 << length);
        }
    }
    return val;
}

static void _delegate_instruction(machine *m, m_word inst){
    if(m->debug){
        fprintf(m->debug,
                "EXEC\t%hx\n",
                inst);
    }
    switch(_extract_bits(inst, 12, 15)){
        // NOT
        case 0b1001:
            mop_not(m,
                    _extract_bits(inst,9,11),
                    _extract_bits(inst,6,7));
            break;
        case 0b0001:
            _add_sub(m, inst);
            break;
        case 0b0101:
            _and_sub(m, inst);
            break;
        case 0b0010:
            mop_ld(m,
                   _extract_bits(inst,9,11),
                   _signed_offset(_extract_bits(inst, 0, 8),9));
            break;
        case 0b011:
            mop_st(m,
                   _extract_bits(inst,9,11),
                   _signed_offset(_extract_bits(inst, 0, 8),9));
            break;
        case 0b1010:
            mop_ldi(m,
                    _extract_bits(inst, 9,11),
                    _signed_offset(_extract_bits(inst, 0, 8),9));
            break;
        case 0b1011:
            mop_sti(m,
                    _extract_bits(inst, 9,11),
                    _signed_offset(_extract_bits(inst, 0,8),9));
            break;
        case 0b0110:
            mop_ldr(m,
                    _extract_bits(inst,9,11),
                    _extract_bits(inst,6,8),
                    _signed_offset(_extract_bits(inst,0,5),6));
            break;
        case 0b0111:
            mop_str(m,
                    _extract_bits(inst, 9, 11),
                    _extract_bits(inst, 6, 8),
                    _signed_offset(_extract_bits(inst,0,5),6));
            break;
        case 0b1110:
            mop_lea(m,
                    _extract_bits(inst, 9, 11),
                    _signed_offset(_extract_bits(inst, 0, 8),9));
            break;
        case 0b0000:
            mop_br(m,
                   _extract_bits(inst, 11, 11),
                   _extract_bits(inst, 10, 10),
                   _extract_bits(inst, 9, 9),
                   _signed_offset(_extract_bits(inst,0,8),9));
            break;
        case 0b1100:
            mop_jmp(m,
                    _extract_bits(inst,6,7));
            break;
        case 0b1111:
            mop_trap(m,
                    _extract_bits(inst, 0, 8));
            break;
        default:
            exit(-1);
            break;
    }
}
static void _run_routine(machine *m){
    while(1){
        if(m->debug){
            fprintf(m->debug,
                    "LOAD\t%hx\n",
                    m->pc);
        }
        m_word instruction = m->ram[m->pc];
        if(m->debug){
            fprintf(m->debug,
                    "STEP\n");
        }
        ++(m->pc);
        _delegate_instruction(m, instruction);
    }
}
void execute_program(machine *m, m_word start_instruction){
    m->pc = start_instruction;
    if(!setjmp(m->_end_jump)){
        _run_routine(m);
    }
    else{
        printf("\n\nProgram completed successfully.\n");
    }   
}
