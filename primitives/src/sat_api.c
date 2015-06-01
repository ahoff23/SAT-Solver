/*******************NOTE: CURRENTLY DOES NOT UNDO LERANING NEW CLAUSES*******************/



#include "C:\Users\ahoff_000\Desktop\UCLA\CS 264\SATSolver\SATSolver\primitives\include\sat_api.h"

/******************************************************************************
* We explain here the functions you need to implement
*
* Rules:
* --You cannot change any parts of the function signatures
* --You can/should define auxiliary functions to help implementation
* --You can implement the functions in different files if you wish
* --That is, you do not need to put everything in a single file
* --You should carefully read the descriptions and must follow each requirement
******************************************************************************/

/******************************************************************************
* Variables
******************************************************************************/

//returns a variable structure for the corresponding index
Var* sat_index2var(c2dSize index, const SatState* sat_state) {
	if (index > 0 && index < sat_state->num_vars)
		return sat_state->vars[index];

	return NULL; //Return NULL if the index exceeds teh bounds of the number of variables
}

//returns the index of a variable
c2dSize sat_var_index(const Var* var) {
	if (var == NULL)
		return 0; //Return 0 if the variable does not exist
	return var->index;
}

//returns the variable of a literal
Var* sat_literal_var(const Lit* lit) {
	//Return NULL if the literal does not exist
	if (lit == NULL)
		return NULL;
	return lit->var;
}

//returns 1 if the variable is instantiated, 0 otherwise
//a variable is instantiated either by decision or implication (by unit resolution)
//returns -1 if var is NULL
BOOLEAN sat_instantiated_var(const Var* var) {
	if (var == NULL)
		return -1; //Return -1 if the variable does not exist
	return var->instantiated;
}

//returns 1 if all the clauses mentioning the variable are subsumed, 0 otherwise
BOOLEAN sat_irrelevant_var(const Var* var) {
	//Find all the clauses containing the positive literal for non-learned clauses
	if (check_list_subsumed(var->pos_lit->clauses) == 0)
		return 0;

	//Find all the clauses containing the negative literal for non-learned clauses
	if (check_list_subsumed(var->neg_lit->clauses) == 0)
		return 0;

	return 1;
}

//Check if a specific list of clauses are subsumed
//@param clause_indices: A list of indices of clauses to check
//@param sat_state: The sat state to check against
//@return 1 if all are subsumed, 0 otherwise
BOOLEAN check_list_subsumed(clauseNode *clauses)
{
	//Create a node to traverse the list
	struct clauseNode *curr = clauses;

	//If the list is empty, then all clauses are subsumed
	if (curr == NULL)
		return 1;

	//Loop through every clause containing the literal
	do
	{
		if (sat_subsumed_clause(curr->node_clause) == 0)
			return 0;
	} while ((curr = curr->next) != NULL);

	return 1; //All clauses containing the variable are subsumed
}

//returns the number of variables in the cnf of sat state
c2dSize sat_var_count(const SatState* sat_state) {
	if (sat_state == NULL)
		return 0;		//0 if the sat state does not exist
	return sat_state->num_vars;
}

//returns the number of clauses mentioning a variable
//a variable is mentioned by a clause if one of its literals appears in the clause
c2dSize sat_var_occurences(const Var* var) {
	if (var == NULL)
		return 0;
	return var->num_mentioned;
}

//returns the index^th clause that mentions a variable
//index starts from 0, and is less than the number of clauses mentioning the variable
//this cannot be called on a variable that is not mentioned by any clause
Clause* sat_clause_of_var(c2dSize index, const Var* var) {
	//Make sure the index is legal
	if (index < 0 || index >= var->num_mentioned)
		return NULL;

	//Create a node to traverse the list of clauses containing the positive literal
	struct clauseNode *curr = sat_pos_literal(var)->clauses;

	if (curr == NULL)
		return NULL;

	//Counter to track the index of the current clause
	unsigned long index_counter = 0;

	//Loop through every clause containing the positive literal
	do
	{
		if (index_counter == index)
			return curr->node_clause;
		index_counter++;
	} while ((curr = curr->next) != NULL);

	//Create a node to traverse the list of clauses containing the positive literal
	curr = sat_neg_literal(var)->clauses;

	if (curr == NULL)
		return NULL;

	//Loop through every clause containing the negative literal
	do
	{
		if (index_counter == index)
			return curr->node_clause;
		index_counter++;
	} while ((curr = curr->next) != NULL);

	return NULL; //All clauses containing the variable are subsumed
}

/******************************************************************************
* Literals
******************************************************************************/

//returns a literal structure for the corresponding index
Lit* sat_index2literal(c2dLiteral index, const SatState* sat_state) {
	if (sat_state == NULL)
		return NULL; //sat_state does not exist
	return sat_state->lits[index];
}

//returns the index of a literal
c2dLiteral sat_literal_index(const Lit* lit) {
	if (lit == NULL)
		return 0; //the literal does not exist
	return lit->index;
}

//returns the positive literal of a variable
Lit* sat_pos_literal(const Var* var) {
	if (var == NULL)
		return NULL; //the variable does not exist
	return var->pos_lit;
}

//returns the negative literal of a variable
Lit* sat_neg_literal(const Var* var) {
	if (var == NULL)
		return NULL; //the variable does not exist
	return var->neg_lit;
}

//Returns the opposite literal (i.e. if A, return -A. If -A, return A)
Lit* opp_lit(const Lit* lit)
{
	if (lit == NULL)
		return NULL;
	else if (lit == lit->var->pos_lit)
		return lit->var->neg_lit;
	else
		return lit->var->pos_lit;
}

//returns 1 if the literal is implied, 0 otherwise
//a literal is implied by deciding its variable, or by inference using unit resolution
//returns -1 if lit is NULL
BOOLEAN sat_implied_literal(const Lit* lit) {
	//Return the truth value of the literal
	if (lit == NULL)
		return -1; //the literal does not exist

	//The literal's truth value is -1 if it is not implied
	if (lit->truth_value == -1)
		return 0;
	return 1;
}

//sets the literal to true, and then runs unit resolution
//returns a learned clause if unit resolution detected a contradiction, NULL otherwise
//
//if the current decision level is L in the beginning of the call, it should be updated 
//to L+1 so that the decision level of lit and all other literals implied by unit resolution is L+1
Clause* sat_decide_literal(Lit* lit, SatState* sat_state) {
	//Add literal to list of decisions
	Decision* new_dec = (Decision*)malloc(sizeof(Decision));
	new_dec->dec_lit = lit;
	decList_push(&(sat_state->decisions),new_dec);

	//Set the literal
	set_literal(lit, sat_state);
		
	//Run unit resolution
	if (sat_unit_resolution(sat_state) == 1)
	{
		sat_state->decision_level++;	//Increment the decision level
		return NULL;
	}
	else
	{
		sat_state->decision_level++; 	//Increment the decision level
		return sat_state->learnedClauses->node_clause;
	}
}

//Updates the CNF based on a decision of a literal or a unit resolution of a literal
//@param lit: the literal being updated or decided on
//@param sat_state: the SatState of the CNF
//@return a contradicted clause if one is found via add_opposite, NULL if no contradictionis found
Clause* set_literal(Lit* lit, SatState* sat_state)
{
	//Set variable to instantiated
	Var* var = sat_literal_var(lit);
	var->instantiated = 1;

	//Set the variable's decision level
	var->decision_level = sat_state->decision_level;

	//Set the literal's and its opposite literal's truth value
	lit->truth_value = 1;
	opp_lit(lit)->truth_value = 0;

	//Update all clauses containing the literal
	subsume_clauses(lit, lit->clauses);

	//Update all clauses containing the opposite of the literal				
	return add_opposite(opp_lit(lit)->clauses, sat_state);
}

//Subsume all clauses containing a literal
//@param lit: the literal causing the subsumption
//@param clauses: the list of clauses being subsumed
void subsume_clauses(Lit* lit, clauseNode* clauses)
{
	//Create a node to traverse the list
	struct clauseNode* curr = clauses;

	//Loop through every clause containing the opposite of the literal
	if (curr == NULL)
		return;
	do
	{
		//Skip clauses that have already been subsumed
		if (curr->node_clause->subsumed == 1)
			continue;

		//Set subsumed to 1
		curr->node_clause->subsumed = 1;
		
		//Set the literal the clause was subsumed on
		curr->node_clause->subsumed_on = lit;
	} while ((curr = curr->next) != NULL);
}

//Remove literal (performed when the opposite literal is decided or asserted by unit resolution)
//@param clauses: the clauses which are being updated
//@param sat_state: the SatState of the problem space
//@return contradiction clause if found, otherwise return NULL
Clause* add_opposite(struct clauseNode* clauses, SatState* sat_state)
{
	//Create a node to traverse the list
	struct clauseNode* curr = clauses;

	//Literal that will be unit resolved on
	Lit* unit_lit;

	//Loop through every clause containing the opposite of the literal
	if (curr == NULL)
		return NULL;

	do {
		//Do not make changes to subsumed clauses
		if (curr->node_clause->subsumed == 1)
			continue;

		//Decrement the number of free literals
		curr->node_clause->free_lits--;

		//Check if the number of literals is 1 (i.e. perform unit resolution)
		if (curr->node_clause->free_lits == 1)
		{
			unit_lit = get_unit_lit(curr->node_clause);
			dlitList_push_back(&(sat_state->decisions.node_dec->units_head), &(sat_state->decisions.node_dec->units_tail), unit_lit);
		}

		//Check if the number of literals is 0 (i.e. a contradiction was found)
		if (curr->node_clause->free_lits == 0)
			return curr->node_clause;
	} while ((curr = curr->next) != NULL);
	
	return NULL;
}

//undoes the last literal decision and the corresponding implications obtained by unit resolution
//
//if the current decision level is L in the beginning of the call, it should be updated 
//to L-1 before the call ends
void sat_undo_decide_literal(SatState* sat_state) {
	//Undo unit resolution								
	undo_unit_resolution(sat_state);
	
	//Undo the decision of the literal and remove the decision from the list of decisions
	undo_set_literal(decList_pop(&(sat_state->decisions))->dec_lit, sat_state);

	//Decrement the decision level
	sat_state->decision_level--;
}

//Undoes a decision of a literal or a unit resolution of a literal
//@param lit: the literal being un-instantiated
//@param sat_state: the SatState of the CNF
void undo_set_literal(Lit* lit, SatState* sat_state)
{
	//Set variable to uninstantiated
	Var* var = sat_literal_var(lit);
	var->instantiated = 0;

	//Reset the variable's decision level
	var->decision_level = -1;

	//Set the literal's and its opposite literal's truth value
	lit->truth_value = -1;
	opp_lit(lit)->truth_value = -1;

	//Reverse all clauses containing the literal
	undo_subsume_clauses(lit, lit->clauses);

	//Reverse all clauses containing the opposite of the literal
	undo_add_opposite(lit->clauses);
}

//Undo any subsumptions that occurred due to a decision or unit resolution
//@param lit: the literal on which the clause was subsumed
//@param clauses: the list of clauses containing lit
void undo_subsume_clauses(Lit* lit, struct clauseNode* clauses)
{
	//Create a node to traverse the list
	struct clauseNode* curr = clauses;

	//Loop through every clause containing the opposite of the literal
	if (curr == NULL)
		return;

	do {
		//Only reverse the subsumption if the clause was subsumed on this literal
		if (curr->node_clause->subsumed_on != lit)
			continue;

		//Set subsumed to 0
		curr->node_clause->subsumed = 0;
		
		//Reset the literal the clause was subsumed on
		curr->node_clause->subsumed_on = NULL;
	} while ((curr = curr->next) != NULL);
}

//Undo remove literal (performed when the opposite literal is decided or asserted by unit resolution)
//@param clauses: the clauses which are being updated
void undo_add_opposite(struct clauseNode* clauses)
{
	//Create a node to traverse the list
	struct clauseNode* curr = clauses;

	//Loop through every clause containing the opposite of the literal
	if (curr == NULL)
		return;

	do {
		//If the clause is subsumed, do not make any changes to it
		if (curr->node_clause->subsumed == 1)
			continue;

		//Increment the number of free literals
		curr->node_clause->free_lits++;
	} while ((curr = curr->next) != NULL);
}

/******************************************************************************
* Clauses
******************************************************************************/

//returns a clause structure for the corresponding index
Clause* sat_index2clause(c2dSize index, const SatState* sat_state) {
	if (sat_state != NULL && index > 0 && index < sat_state->num_clauses)
		return &(sat_state->CNF[index]);

	return NULL;	//Parameter error
}

//returns the index of a clause
c2dSize sat_clause_index(const Clause* clause) {
	if (clause == NULL)
		return 0;
	return clause->index;
}

//returns the literals of a clause
Lit** sat_clause_literals(const Clause* clause) {
	if (clause == NULL)
		return NULL;
	return clause->literals;
}

//returns the number of literals in a clause
c2dSize sat_clause_size(const Clause* clause) {
	if (clause == NULL)
		return NULL;
	return clause->num_lits;
}

//returns 1 if the clause is subsumed, 0 otherwise (-1 if the clause does not exist)
BOOLEAN sat_subsumed_clause(const Clause* clause) {
	if (clause == NULL)
		return -1;
	return clause->subsumed;
}

//returns the number of clauses in the cnf of sat state
c2dSize sat_clause_count(const SatState* sat_state) {
	if (sat_state == NULL)
		return 0;
	return sat_state->num_clauses;
}

//returns the number of learned clauses in a sat state (0 when the sat state is constructed)
c2dSize sat_learned_clause_count(const SatState* sat_state) {
	if (sat_state == NULL)
		return 0;
	return sat_state->num_learned;
}

//adds clause to the set of learned clauses, and runs unit resolution
//returns a learned clause if unit resolution finds a contradiction, NULL otherwise
//
//this function is called on a clause returned by sat_decide_literal() or sat_assert_clause()
//moreover, it should be called only if sat_at_assertion_level() succeeds
Clause* sat_assert_clause(Clause* clause, SatState* sat_state) {
	//Add clause to list of clauses
	clauseList_push(sat_state->learnedClauses, clause);

	//Increment the number of clauses learned
	sat_state->num_learned++;

	//Stores each literal in the clause
	Lit* clause_lit;

	//Add clause to hashtable of literal-clauses for each literal
	for (int i = 0; i < clause->num_lits; i++)
	{
		//Get the current literal
		clause_lit = clause->literals[i];
		
		//Push the new clause onto the list of clauses associated with the current variable
		clauseList_push(clause_lit->clauses, clause);
	}

	//Run unit resolution
	if (sat_unit_resolution(sat_state) == 1)
		return NULL;
	else
		return sat_state->learnedClauses->node_clause;
}

//Gets the only literal not instantiated
//@param clause: the clause to be search for a unit literal
//@return the unit literal
Lit* get_unit_lit(Clause* clause)
{
	//Traverse each literal in the clause
	for (int i = 0; i < clause->num_lits; i++)
	{
		if (clause->literals[i]->truth_value == -1)
			return clause->literals[i];
	}
	return NULL;	//All literals are instantiated (ERROR)
}

/******************************************************************************
* A SatState should keep track of pretty much everything you will need to
* condition/uncondition variables, perform unit resolution, and do clause learning
*
* Given an input cnf file you should construct a SatState
*
* This construction will depend on how you define a SatState
* Still, you should at least do the following:
* --read a cnf (in DIMACS format, possible with weights) from the file
* --initialize variables (n of them)
* --initialize literals  (2n of them)
* --initialize clauses   (m of them)
*
* Once a SatState is constructed, all of the functions that work on a SatState
* should be ready to use
*
* You should also write a function that frees the memory allocated by a
* SatState (sat_state_free)
******************************************************************************/

//constructs a SatState from an input cnf file
SatState* sat_state_new(const char* file_name) {

	// ... TO DO ...

	return NULL; //dummy valued
}

//frees the SatState
void sat_state_free(SatState* sat_state) {

	// ... TO DO ...

	return; //dummy valued
}

/******************************************************************************
* Given a SatState, which should contain data related to the current setting
* (i.e., decided literals, subsumed clauses, decision level, etc.), this function
* should perform unit resolution at the current decision level
*
* It returns 1 if succeeds, 0 otherwise (after constructing an asserting
* clause)
*
* There are three possible places where you should perform unit resolution:
* (1) after deciding on a new literal (i.e., in sat_decide_literal())
* (2) after adding an asserting clause (i.e., in sat_assert_clause(...))
* (3) neither the above, which would imply literals appearing in unit clauses
*
* (3) would typically happen only once and before the other two cases
* It may be useful to distinguish between the above three cases
*
* Note if the current decision level is L, then the literals implied by unit
* resolution must have decision level L
*
* This implies that there must be a start level S, which will be the level
* where the decision sequence would be empty
*
* We require you to choose S as 1, then literals implied by (3) would have 1 as
* their decision level (this level will also be the assertion level of unit
* clauses)
*
* Yet, the first decided literal must have 2 as its decision level
******************************************************************************/

//applies unit resolution to the cnf of sat state
//returns 1 if unit resolution succeeds, 0 if it finds a contradiction
BOOLEAN sat_unit_resolution(SatState* sat_state) {
	//Create a litNode to traverse the decision's list of unit literals
	struct dlitNode* trav = &(sat_state->decisions.node_dec->units_head);

	//While not at the end of the literal list
	while (trav != NULL)
	{
		//Set the literal and return 0 if unit resolution returns a contradiction clause
		if (set_literal(trav->node_lit, sat_state) != NULL)
			return 0;

		trav = trav->next;
	}
	return 1;		//Resolution completed without returning a contradiction clause i.e. unit resolution succeeded
}

//undoes sat_unit_resolution(), leading to un-instantiating variables that have been instantiated
//after sat_unit_resolution()
void sat_undo_unit_resolution(SatState* sat_state) {
	//Create a litNode to traverse the decision's list of unit literals
	struct dlitNode* trav = sat_state->decisions.node_dec->units_tail;

	//While not at the end of the literal list
	while (trav != NULL)
	{
		//Undo the unit resolution on the current literal
		undo_set_literal(trav->node_lit, sat_state);
		
		//Go to the next unit to undo resolution on
		trav = trav->prev;
	}
}

//returns 1 if the decision level of the sat state equals to the assertion level of clause,
//0 otherwise
//
//this function is called after sat_decide_literal() or sat_assert_clause() returns clause.
//it is used to decide whether the sat state is at the right decision level for adding clause.
BOOLEAN sat_at_assertion_level(const Clause* clause, const SatState* sat_state) {

	// ... TO DO ...

	return 0; //dummy valued
}

/******************************************************************************
* The functions below are already implemented for you and MUST STAY AS IS
******************************************************************************/

//returns the weight of a literal (which is 1 for our purposes)
c2dWmc sat_literal_weight(const Lit* lit) {
	return 1;
}

//returns 1 if a variable is marked, 0 otherwise
BOOLEAN sat_marked_var(const Var* var) {
	return var->mark;
}

//marks a variable (which is not marked already)
void sat_mark_var(Var* var) {
	var->mark = 1;
}

//unmarks a variable (which is marked already)
void sat_unmark_var(Var* var) {
	var->mark = 0;
}

//returns 1 if a clause is marked, 0 otherwise
BOOLEAN sat_marked_clause(const Clause* clause) {
	return clause->mark;
}

//marks a clause (which is not marked already)
void sat_mark_clause(Clause* clause) {
	clause->mark = 1;
}
//unmarks a clause (which is marked already)
void sat_unmark_clause(Clause* clause) {
	clause->mark = 0;
}

/******************************************************************************
* end
******************************************************************************/
