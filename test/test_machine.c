#include "machine.h"

static char * t_machine_destruction(){
    machine *m = machine_init();
    mu_assert("It constructs properly", m != NULL);
    machine_destroy(&m);
    mu_assert("Destruct sets to null", m == NULL);
    return 0;
}

static char * t_machine_mop_not(){
    machine *m = machine_init();
    m->registers[0] = 0b1010101010101010;
    mop_not(m, 1, 0);
    m_word reverse = 0b0101010101010101;
    mu_assert("Binary NOT works with other destination", \
            m->registers[1] == reverse);
    mop_not(m, 0, 0);
    mu_assert("Binary NOT works with self destination", \
            m->registers[0] == reverse);
    machine_destroy(&m);
    return 0;
}

static char * t_machine_mop_and_register(){
    machine *m = machine_init();
    m->registers[0] = 0b101;
    m->registers[1] = 0b100;
    mop_and_register(m, 2, 0, 1);
    mu_assert("Register AND instruction", m->registers[2] == 0b100);
    machine_destroy(&m);
    return 0;
}

static char * t_machine_mop_add_register(){
    machine *m = machine_init();
    m->registers[0] = 1;
    m->registers[1] = 1;
    mop_add_register(m, 2, 0, 1);
    mu_assert("Register ADD works with positive numbers",
            m->registers[2] == 2);
    m->registers[0] = (~4) + 1; // -4
    m->registers[1] = 5;
    mop_add_register(m, 2, 0, 1);
    mu_assert("Register ADD works with negative rollaround",
            ((int) m->registers[2]) == 1);
    m->registers[0] = (~4) + 1; // -4
    m->registers[1] = (~4) + 1; // -4
    mop_add_register(m, 2, 0, 1);
    mu_assert("Register ADD works with negative numbers",
            m->registers[2] == (m_word) (~8) + 1);
    machine_destroy(&m);
    return 0;
}
static char * t_machine_mop_ld(){
    machine *m = machine_init();
    m->ram[3] = 10;
    mop_ld(m, 0, 3);
    mu_assert("LD works with positive offset",
            m->registers[0] == 10);
    m->pc = 1;
    m->ram[5] = 50;
    mop_ld(m, 0, 4);
    mu_assert("LD works with positive offset and nonzero PC",
            m->registers[0] == 50);
    m->pc = 20;
    m->ram[10] = 99;
    mop_ld(m, 0, -10);
    mu_assert("LD works with negative offset",
            m->registers[0] == 99);
    machine_destroy(&m);
    return 0;
}
static char * t_machine_mop_ldi(){
    machine *m = machine_init();
    m->ram[1] = 10;
    m->ram[10] = 99;
    mop_ldi(m, 0, 1);
    mu_assert("LDI works",
            m->registers[0] == 99);
    machine_destroy(&m);
    return 0;
}
static char * t_machine_mop_sti(){
    machine *m = machine_init();
    m->registers[0] = 100;
    m->ram[1] = 10;
    mop_sti(m, 0, 1);
    mu_assert("STI works",
            m->ram[10] == 100);
    machine_destroy(&m);
    return 0;
}
static char * t_machine_mop_ldr(){
    machine *m = machine_init();
    m->registers[0] = 100;
    m->ram[100] = 99;
    mop_ldr(m, 1, 0, 0);
    mu_assert("LDR works",
            m->registers[1] == 99);
    machine_destroy(&m);
    return 0;
}

static char * t_machine_mop_str(){
    machine *m = machine_init();
    m->registers[0] = 100;
    m->registers[1] = 200;
    mop_str(m, 1, 0, 0);
    mu_assert("STR works",
            m->ram[100] == 200);
    machine_destroy(&m);
    return 0;
}

static char * t_machine_mop_lea(){
    machine *m = machine_init();
    mop_lea(m, 0, 10);
    mu_assert("LEA works with zero PC",
            m->registers[0] == 10);
    m->pc = 100;
    mop_lea(m, 0, 10);
    mu_assert("LEA works with nonzero PC",
            m->registers[0] == 110);
    machine_destroy(&m);
    return 0;
}
static char * t_machine_m_write_register(){
    machine *m = machine_init();
    m_write_register(m, 0, 0);    
    mu_assert("Setting register sets zero flag",
            m->condition_z && ! m->condition_p && ! m->condition_n);
    m_write_register(m, 0, 10);
    mu_assert("Setting registers changes state",
            m->registers[0] == 10);
    mu_assert("Setting register sets positive flag",
        m->condition_p && ! m->condition_n && ! m->condition_z);
    m_write_register(m, 0, (~10) + 1);
    mu_assert("Setting register sets negative flag",
            m->condition_n && ! m->condition_p && ! m->condition_z);
    machine_destroy(&m);
    return 0;
}
static char * test_machine(){
    mu_run_test(t_machine_destruction);
    mu_run_test(t_machine_mop_not);
    mu_run_test(t_machine_mop_and_register);
    mu_run_test(t_machine_mop_add_register);
    mu_run_test(t_machine_mop_ld);
    mu_run_test(t_machine_mop_ldi);
    mu_run_test(t_machine_mop_sti);
    mu_run_test(t_machine_mop_ldr);
    mu_run_test(t_machine_mop_lea);
    mu_run_test(t_machine_mop_str);
    mu_run_test(t_machine_m_write_register);
    return 0;
}
