 #define mu_assert(message, test) do { if (!(test)) return message; } while (0)
 #define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)

extern int test_run;

#include <stdio.h>
#include "sat_api.h"

 int tests_run = 0;
 /*
 int foo = 7;
 int bar = 5;
 
 static char * test_foo() {
     SatState* satState = sat_state_new("test/test.cnf");
     printf("satState created %p\n", satState);
     return 0;
 }
 
 static char * test_bar() {
     mu_assert("error, bar != 5", bar == 5);
     return 0;
 }
 
 static char * all_tests() {
     mu_run_test(test_foo);
     mu_run_test(test_foo);
     mu_run_test(test_bar);
     return 0;
 }
 */
 int main(int argc, char **argv) {
 	SatState* satState = sat_state_new("test/test.cnf");
     printf("satState created %p\n", satState);
 	//char *result = test_foo();
 	return 0;
 	/*
     char *result = all_tests();
     if (result != 0) {
         printf("%s\n", result);
     }
     else {
         printf("ALL TESTS PASSED\n");
     }
     printf("Tests run: %d\n", tests_run);
 
     return result != 0;
     */
 }
