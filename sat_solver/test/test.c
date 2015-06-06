#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test, index) do { char *message = test(); tests_run++; if (message) return message; else printf("Passed test %d\n", index);} while (0)

extern int test_run;

#include <stdio.h>
#include "sat_api.h"

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count
typedef struct var Var;
typedef struct literal Lit;
typedef struct decision Decision;
typedef struct clause Clause;
typedef struct sat_state_t SatState;

int tests_run = 0;

static char * test_sat_state_var_count() {
	SatState* s = sat_state_new("test/test.cnf");
	mu_assert("num_vars != 11", sat_var_count(s) == 11);
	sat_state_free(s);
	return 0;
}

static char* test_var_index() {
	SatState* s = sat_state_new("test/test.cnf");
	for(c2dSize i = 1; i < sat_var_count(s); i++) {
		mu_assert("var->index != index of var",sat_var_index(sat_index2var(i, s)) == i);
	}
	sat_state_free(s);
	return 0;
}

static char* test_lit_index() {
	SatState* s = sat_state_new("test/test.cnf");
	for(c2dLiteral i = 1; i < sat_var_count(s); i++) {
		mu_assert("lit->index != index of lit",sat_literal_index(sat_index2literal(i, s)) == i);
		mu_assert("lit->index != index of lit",sat_literal_index(sat_index2literal(i*-1, s)) == i*-1);
	}
	sat_state_free(s);
	return 0;
}

static char* test_sat_literal_var() {
	SatState* s = sat_state_new("test/test.cnf");
	for(int i = 1; i < sat_var_count(s); i++) {
		Lit* litp = sat_index2literal(i, s);
		Lit* litn = sat_index2literal(i*-1, s);
		Var* var = sat_index2var(i, s);
		mu_assert("", sat_pos_literal(var) == litp);
		mu_assert("", sat_neg_literal(var) == litn);
		mu_assert("", sat_literal_var(litp) == var);
		mu_assert("", sat_literal_var(litn) == var);
	}
	sat_state_free(s);
	return 0;
}

static char* test_decide_literal() {
	SatState* s = sat_state_new("test/test.cnf");
	// Decide literal 3
	Lit* lit = sat_index2literal(3, s);
	sat_decide_literal(lit, s);
	mu_assert("Variable 3 not instantiated", sat_instantiated_var(sat_literal_var(lit)) == 1);
	
	mu_assert("Literal 3 was not implied.", sat_implied_literal(lit) == 1);
	mu_assert("Literal -3 opposite was implied.", sat_implied_literal(opp_lit(lit)) == 0);
	
	// Check if right clauses are subsumed
	Clause* clause;
	for(c2dSize i = 1; i <= sat_clause_count(s) ; i++) {
		clause = sat_index2clause(i, s);
		if(i!=2 && i!= 4 && i!=13 && i!=14) {
			mu_assert("Clause i subsumed", sat_subsumed_clause(clause) == 0);
		} else {
			mu_assert("Clause i not subsumed", sat_subsumed_clause(clause) == 1);
		}
	}
	
	// Next, decide literal 2 (should imply -1)
	lit = sat_index2literal(2, s);
	sat_decide_literal(lit, s);

	// 2 should now be implied
	mu_assert("Variable 2 not instantiated", sat_instantiated_var(sat_literal_var(lit)) == 1);
	mu_assert("Literal 2 was not implied.", sat_implied_literal(lit) == 1);
	mu_assert("Literal -2 was implied.", sat_implied_literal(opp_lit(lit)) == 0);
	
	// -1 should have been resolved when 2 was decided
	lit = sat_index2literal(-1, s);
	mu_assert("Variable 1 not instantiated", sat_instantiated_var(sat_literal_var(lit)) == 1);
	mu_assert("Literal -1 was not implied.", sat_implied_literal(lit) == 1);
	mu_assert("Literal 1 was implied.", sat_implied_literal(opp_lit(lit)) == 0);
	
	// Check if right clauses are subsumed
	for(c2dSize i = 1; i <= sat_clause_count(s) ; i++) {
		clause = sat_index2clause(i, s);
		if(i!=2 && i!= 4 && i!=13 && i!=14 && i!=1 && i!=3 && i!=7 && i!=8) {
			mu_assert("Clause i subsumed", sat_subsumed_clause(clause) == 0);
		} else {
			mu_assert("Clause i not subsumed", sat_subsumed_clause(clause) == 1);
		}
	} 
	
	sat_state_free(s);
	return 0;
}

static char* test_sat_var_occurences() {
	SatState* s = sat_state_new("test/test.cnf");
	mu_assert("Incorrect num_occurences of var 1", sat_var_occurences(sat_index2var(1, s)) == 3);
	mu_assert("Incorrect num_occurences of var 2", sat_var_occurences(sat_index2var(2, s)) == 5);
	mu_assert("Incorrect num_occurences of var 3", sat_var_occurences(sat_index2var(3, s)) == 7);
	mu_assert("Incorrect num_occurences of var 4", sat_var_occurences(sat_index2var(4, s)) == 3);
	mu_assert("Incorrect num_occurences of var 5", sat_var_occurences(sat_index2var(5, s)) == 5);
	mu_assert("Incorrect num_occurences of var 6", sat_var_occurences(sat_index2var(6, s)) == 2);
	mu_assert("Incorrect num_occurences of var 7", sat_var_occurences(sat_index2var(7, s)) == 4);
	mu_assert("Incorrect num_occurences of var 8", sat_var_occurences(sat_index2var(8, s)) == 7);
	mu_assert("Incorrect num_occurences of var 9", sat_var_occurences(sat_index2var(9, s)) == 2);
	mu_assert("Incorrect num_occurences of var 10", sat_var_occurences(sat_index2var(10, s)) == 2);
	mu_assert("Incorrect num_occurences of var 11", sat_var_occurences(sat_index2var(11, s)) == 2);
	sat_state_free(s);
	return 0;
}

static char* test_undo_decide_literal() {
	SatState* s = sat_state_new("test/test.cnf");
	// Decide literal 3
	Lit* lit = sat_index2literal(3, s);
	sat_decide_literal(lit, s);
	
	// Next, decide literal 2 (should imply -1)
	lit = sat_index2literal(2, s);
	sat_decide_literal(lit, s);
	
	// Undo the decision of literal 2 (thus the implied -1 too)
	sat_undo_decide_literal(s);
	
	// Check that the right clauses are subsumed after undoing decision.
	Clause* clause;
	for(c2dSize i = 1; i <= sat_clause_count(s) ; i++) {
		clause = sat_index2clause(i, s);
		if(i!=2 && i!= 4 && i!=13 && i!=14) {
			mu_assert("Clause i subsumed", sat_subsumed_clause(clause) == 0);
		} else {
			mu_assert("Clause i not subsumed", sat_subsumed_clause(clause) == 1);
		}
	}
	
	sat_state_free(s);
	return 0;
}

static char * all_tests() {
	mu_run_test(test_sat_state_var_count, 0);
	mu_run_test(test_var_index, 1);
	mu_run_test(test_lit_index, 2);
	mu_run_test(test_sat_literal_var, 3);
	mu_run_test(test_decide_literal, 4);
	mu_run_test(test_undo_decide_literal, 5);
	mu_run_test(test_sat_var_occurences, 6);
	return 0;
}

int main(int argc, char **argv) {
	char *result = all_tests();
	if (result != 0) {
		printf("%s\n", result);
	} else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
	
}
