#ifndef SATAPI_H_
#define SATAPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Lists.h"

/******************************************************************************
* sat_api.h shows the function prototypes you should implement to create libsat.a
*
* To use the sat library, a sat state should be constructed from an input cnf file
*
* The sat state supports functions that are usually used to implement a sat solver,
* such as deciding variables and learning clauses. It also supports functions for
* obtaining stats about the input cnf, such as the number of variables and clauses.
*
* The functions supported by a sat state are sufficient to implement a modern sat
* solver (also known as CDC solver).
*
* A sat state has a "decision level", which is incremented when a literal is
* decided, and decremented when a literal is undecided. Hence, each decided
* literal is associated with a decision level. Similarly, literals which are implied
* by unit resolution are also associated with a decision level.
*
* A learned clause is associated with an "assertion level". The clause can be
* asserted (added) to a state state only when the decision level of the sat
* state equals to the clause's assertion level.
******************************************************************************/

/******************************************************************************
* typedefs
******************************************************************************/

typedef char BOOLEAN;			//signed

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count

/******************************************************************************
* Forward Declarations
******************************************************************************/
typedef struct literal Lit;

/******************************************************************************
* Basic structures
******************************************************************************/

/******************************************************************************
* Variables:
* --You must represent variables using the following struct
* --Variable index must start at 1, and is no greater than the number of cnf variables
* --Index of a variable must be of type "c2dSize"
* --The field "mark" below and its related functions should not be changed
******************************************************************************/

typedef struct var {
	int instantiated;				//1 if the variable has been instantiated, 0 otherwise

	Lit* pos_lit;					//The positive literal corresponding to this variable
	Lit* neg_lit;					//The negative literal corresponding to this variable
	unsigned long num_mentioned;	//Number of clauses mentioning this literal

	c2dSize index;					//Variable index (you can change the variable name as you wish)
	int decision_level;				//Decision level at which the variable is instantiated

	BOOLEAN mark; //THIS FIELD MUST STAY AS IS

} Var;

/******************************************************************************
* Literals:
* --You must represent literals using the following struct
* --Positive literals' indices range from 1 to n (n is the number of cnf variables)
* --Negative literals' indices range from -n to -1 (n is the number of cnf variables)
* --Index of a literal must be of type "c2dLiteral"
******************************************************************************/

typedef struct literal {
	c2dLiteral index;					//Literal index (you can change the variable name as you wish)
	BOOLEAN truth_value;				//1 if the variable is true, 0 if it is false, -1 if it is not set
	Var* var;							//The variable corresponding to this literal	
	clauseList* clauses;				//List of clauses containing this literal						***MAKE ARRAY***
	clauseList* learnedClauses;			//List of clauses containing this literal that were learned
	Clause* unit_on;					//Clause on which this literal becomes unit
	litList* unit_children;				//List of literals that became unit once this literal was set
	BOOLEAN in_contradiction_clause;	//True if this literal is in the contradcition clause
	BOOLEAN DFS_ignore;					//True if the DFS for a uip should ignore this literal (i.e. inspecting whether or not this literal is the uip)
} Lit;

/******************************************************************************
* Clauses:
* --You must represent clauses using the following struct
* --Clause index must start at 1, and is no greater than the number of cnf clauses
* --Index of a clause must be of type "c2dSize"
* --A clause must have an array consisting of its literals
* --The index of literal array must start at 0, and is less than the clause size
* --The field "mark" below and its related functions should not be changed
******************************************************************************/

typedef struct clause {
	//c2dSize index;  clause index   (you can change the variable name as you wish)
	//Lit** literals; literal array  (you can change the variable name as you wish
	Lit** literals;			//Array of pointers to literals
	BOOLEAN subsumed;		//1 if the clause is subsumed at the current decision level, 0 otherwise
	int free_lits;			//Number of literals free at the current decision level (not updated after clause is subsumed)
	Lit* subsumed_on;		//Literal which caused the clause to become subsumed
	c2dSize index;			//Index of the clause in the CNF
	c2dSize num_lits;		//The number of literals in the clause

	int dec_level;			//Decision level at which the clause was learned. -1 if never learned (i.e. in the original CNF)

	BOOLEAN mark; //THIS FIELD MUST STAY AS IS

} Clause;

/******************************************************************************
* SatState:
* --The following structure will keep track of the data needed to
* condition/uncondition variables, perform unit resolution, and so on ...
******************************************************************************/

typedef struct sat_state_t {
	Var** vars;									//Array of pointers to variables (indices 1 to n)
	Lit** lits;									//Array of pointers to literals (indices -n to -1 and 1 to n)
	Clause *CNF;								//Array of clauses forming the CNF
	clauseList *learnedClauses;					//List of learned clauses
	c2dLiteral num_lits;						//Number of literals
	c2dSize num_vars;							//Number of variables
	c2dSize num_clauses;						//Number of clauses in the CNF
	int decision_level;							//Current decision level
	decList* decisions;							//List of the decisions made (head is the most recent decision)
	c2dSize num_learned;						//Number of learned clauses

} SatState;

typedef struct decision {
	Lit* dec_lit;					//Literal on which the decision was made
	dlitList* units;				//Unit literals found based on the decision made at this level
	dlitList* implication_graph;	//Literals to inspect in the implication graph
	int contradiction_lits;			//Number of literals in the contradiction clause found at this decision level
} Decision;

/******************************************************************************
* API:
* --Using the above structures you must implement the following functions
* --Incomplete implementations of the functions can be found in sat_api.c
* --These functions are all you need for the knowledge compiler
* --You must implement each function below
* --Note that most of the functions can be implemented in 1 line or so
******************************************************************************/

/******************************************************************************
* function prototypes
******************************************************************************/

/******************************************************************************
* Variables
******************************************************************************/

//returns a variable structure for the corresponding index
Var* sat_index2var(c2dSize index, const SatState* sat_state);

//returns the index of a variable
c2dSize sat_var_index(const Var* var);

//returns the variable of a literal
Var* sat_literal_var(const Lit* lit);

//returns 1 if the variable is instantiated, 0 otherwise
//a variable is instantiated either by decision or implication (by unit resolution)
BOOLEAN sat_instantiated_var(const Var* var);

//returns 1 if all the clauses mentioning the variable are subsumed, 0 otherwise
BOOLEAN sat_irrelevant_var(const Var* var);

//returns the number of variables in the cnf of sat state
c2dSize sat_var_count(const SatState* sat_state);

//returns the number of clauses mentioning a variable
//a variable is mentioned by a clause if one of its literals appears in the clause
c2dSize sat_var_occurences(const Var* var);

//returns the index^th clause that mentions a variable
//index starts from 0, and is less than the number of clauses mentioning the variable
//this cannot be called on a variable that is not mentioned by any clause
Clause* sat_clause_of_var(c2dSize index, const Var* var);

/******************************************************************************
* Literals
******************************************************************************/

//returns a literal structure for the corresponding index
Lit* sat_index2literal(c2dLiteral index, const SatState* sat_state);

//returns the index of a literal
c2dLiteral sat_literal_index(const Lit* lit);

//returns the positive literal of a variable
Lit* sat_pos_literal(const Var* var);

//returns the negative literal of a variable
Lit* sat_neg_literal(const Var* var);

//returns 1 if the literal is implied, 0 otherwise
//a literal is implied by deciding its variable, or by inference using unit resolution
BOOLEAN sat_implied_literal(const Lit* lit);

//sets the literal to true, and then runs unit resolution
//returns a learned clause if unit resolution detected a contradiction, NULL otherwise
Clause* sat_decide_literal(Lit* lit, SatState* sat_state);

//Updates the CNF based on a decision of a literal or a unit resolution of a literal
Clause* set_literal(Lit* lit, SatState* sat_state);

//undoes the last literal decision and the corresponding implications obtained by unit resolution
void sat_undo_decide_literal(SatState* sat_state);

//Returns the opposite literal (i.e. if A, return -A. If -A, return A)
Lit* opp_lit(const Lit* lit);

//Remove literal (performed when the opposite literal is decided or asserted by unit resolution)
Clause* add_opposite(clauseList* clauses, SatState* sat_state);

//Subsume all clauses containing a literal
void subsume_clauses(Lit* lit, clauseList* clauses);

//Undoes a decision of a literal or a unit resolution of a literal
void undo_set_literal(Lit* lit, SatState* sat_state);

//Undo any subsumptions that occurred due to a decision or unit resolution
void undo_subsume_clauses(Lit* lit, clauseList* clauses);

//Undo remove literal (performed when the opposite literal is decided or asserted by unit resolution)
void undo_add_opposite(clauseList* clauses);

/******************************************************************************
* Clauses
******************************************************************************/

//returns a clause structure for the corresponding index
Clause* sat_index2clause(c2dSize index, const SatState* sat_state);

//returns the index of a clause
c2dSize sat_clause_index(const Clause* clause);

//returns the literals of a clause
Lit** sat_clause_literals(const Clause* clause);

//returns the number of literals in a clause
c2dSize sat_clause_size(const Clause* clause);

//returns 1 if the clause is subsumed, 0 otherwise
BOOLEAN sat_subsumed_clause(const Clause* clause);

//returns the number of clauses in the cnf of sat state
c2dSize sat_clause_count(const SatState* sat_state);

//returns the number of learned clauses in a sat state (0 when the sat state is constructed)
c2dSize sat_learned_clause_count(const SatState* sat_state);

//adds clause to the set of learned clauses, and runs unit resolution
//returns a learned clause if unit resolution finds a contradiction, NULL otherwise
//
//this function is called on a clause returned by sat_decide_literal() or sat_assert_clause()
//moreover, it should be called only if sat_at_assertion_level() succeeds
Clause* sat_assert_clause(Clause* clause, SatState* sat_state);

//Check if a specific list of clauses are subsumed
BOOLEAN check_list_subsumed(clauseList* clauses);

//Gets the only literal not instantiated
Lit* get_unit_lit(Clause* clause);

/******************************************************************************
* SatState
******************************************************************************/

//constructs a SatState from an input cnf file
SatState* sat_state_new(const char* file_name);

//frees the SatState
void sat_state_free(SatState* sat_state);

//applies unit resolution to the cnf of sat state
//returns 1 if unit resolution succeeds, 0 if it finds a contradiction
BOOLEAN sat_unit_resolution(SatState* sat_state);

//Check all of the initial clauses in the CNF are unit, and perform unit resolution if they are
BOOLEAN initial_unit_resolution(SatState* sat_state);

//undoes sat_unit_resolution(), leading to un-instantiating variables that have been instantiated
//after sat_unit_resolution()
void sat_undo_unit_resolution(SatState* sat_state);

//Undoes resolution at decision level 0 (i.e. before a decision is made)
void undo_all_resolution(SatState* sat_state);

//Free the decision and all its underlying pointers
void free_decision(Decision* undo_dec);

//returns 1 if the decision level of the sat state equals to the assertion level of clause,
//0 otherwise
//
//this function is called after sat_decide_literal() or sat_assert_clause() returns clause.
//it is used to decide whether the sat state is at the right decision level for adding clause.
BOOLEAN sat_at_assertion_level(const Clause* clause, const SatState* sat_state);

// Gets the last decision made in the SatState
Decision* get_latest_decision(SatState* sat_state);

// Print out current clauses
void debug_print_clauses(SatState*);

//Gets the uip
Lit* sat_get_uip(Clause* contradiction, SatState* sat_state);

//Gets a queue of literals leading to the contradiciton clause in reverse order
void find_uip_lits(Clause* contradiction, SatState* sat_state);

//Perform DFS from the decision literal to the contradiciton clause
BOOLEAN uip_DFS(SatState* sat_state);

//Get the assertion clause
Clause* get_assertion_clause(Clause* contradiction, SatState* sat_state);

/******************************************************************************
* The functions below are already implemented for you and MUST STAY AS IS
******************************************************************************/

//returns the weight of a literal (which is simply 1 for our purposes)
c2dWmc sat_literal_weight(const Lit* lit);

//returns 1 if a variable is marked, 0 otherwise
BOOLEAN sat_marked_var(const Var* var);

//marks a variable (which is not marked already)
void sat_mark_var(Var* var);

//unmarks a variable (which is marked already)
void sat_unmark_var(Var* var);

//returns 1 if a clause is marked, 0 otherwise
BOOLEAN sat_marked_clause(const Clause* clause);

//marks a clause (which is not marked already)
void sat_mark_clause(Clause* clause);

//unmarks a clause (which is marked already)
void sat_unmark_clause(Clause* clause);

#endif //SATAPI_H_

/******************************************************************************
* end
******************************************************************************/
