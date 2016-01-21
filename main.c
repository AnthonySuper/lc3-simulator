#include <stdio.h>
#include <machine.h>
#include <run.h>

int main(){
    machine *m = machine_init();
    m->debug = stdout;
    m->registers[0] = 'C';
    m->ram[3000] = 0b1111000000100000;
    m->ram[3001] = 0b1111000000100001;
    m->ram[3002] = 0b1111000000100101;
    execute_program(m, 3000);
    return 0;
}
