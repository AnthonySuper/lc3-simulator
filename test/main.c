#define mu_assert(message, test) do { asserts_run++; if(!(test)) return message;} while(0)
#define mu_run_test(test) do { char *message = test(); tests_run++;\
    if(message) return message;} while (0)
int tests_run;
int asserts_run;
#include <stdio.h>
// Yes, we're including .c files on purpose

#include "test_machine.c"

static char * run_tests(){
    mu_run_test(test_machine);
    return 0;
}
int main(){
    tests_run = 0;
    asserts_run = 0;
    char *res = run_tests();
    if(res != 0){
        printf("Test failed: %s\n", res);
    }
    else{
        printf("All tests passed, %i cases with %i asserts\n",
                tests_run,
                asserts_run);
    }
    return res != 0;
}
