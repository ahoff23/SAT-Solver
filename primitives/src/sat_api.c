#include "sat_api.h"

#define maxLength 500

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
//@param clauses: A list of clauses to check
//@return 1 if all are subsumed, 0 otherwise
BOOLEAN check_list_subsumed(clauseList* clauses)
{
	//Create a node to traverse the list
	clauseNode* curr = clauses->head;

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
	clauseNode *curr = sat_pos_literal(var)->clauses->head;

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

	//Create a node to traverse the list of clauses containing the negative literal
	curr = sat_neg_literal(var)->clauses->head;

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
	return lit->truth_value;
}

//sets the literal to true, and then runs unit resolution
//returns a learned clause if unit resolution detected a contradiction, NULL otherwise
//
//if the current decision level is L in the beginning of the call, it should be updated 
//to L+1 so that the decision level of lit and all other literals implied by unit resolution is L+1
Clause* sat_decide_literal(Lit* lit, SatState* sat_state) {
	printf("deciding lit: %ld\n", lit->index);
	//Add literal to list of decisions
	Decision* new_dec = (Decision*)malloc(sizeof(Decision));
	new_dec->units = (dlitList*) malloc(sizeof(dlitList));
	new_dec->units->head = NULL;
	new_dec->units->tail = NULL;
	new_dec->dec_lit = lit;
	new_dec->implication_graph = (dlitList*)malloc(sizeof(dlitList));
	decList_push(sat_state->decisions,new_dec);

	sat_state->decision_level++;	//Increment the decision level


	//Set the literal and get a contradiction clause if one exists
	Clause* contradiction = set_literal(lit, sat_state); 
	
	printf("Set literal %ld. contradiction clause* = %p\n", lit->index, contradiction);
	
	//Learn a clause if a contradiction has been found
	if (contradiction != NULL)
	{
		//Get the assertion clause
		Clause* assertion = get_assertion_clause(contradiction, sat_state);

		//Add assertion clause to list of learned clauses
		sat_state->num_learned++;
		clauseList_push(sat_state->learnedClauses,assertion);

		return assertion;
	}

	//Run unit resolution
	if (sat_unit_resolution(sat_state) == 1)
		return NULL;
	else
		return sat_state->learnedClauses->head->node_clause;
}

//Updates the CNF based on a decision of a literal or a unit resolution of a literal
//@param lit: the literal being updated or decided on
//@param sat_state: the SatState of the CNF
//@return a contradicted clause if one is found via add_opposite, NULL if no contradiction is found
Clause* set_literal(Lit* lit, SatState* sat_state)
{
	printf("Setting literal %ld\n", lit->index);
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
void subsume_clauses(Lit* lit, clauseList* clauses)
{
	//Create a node to traverse the list
	clauseNode* curr = clauses->head;

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
Clause* add_opposite(clauseList* clauses, SatState* sat_state)
{
	printf("start add_opposite\n");
	//Create a node to traverse the list
	clauseNode* curr = clauses->head;

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
		printf("decrement free lits on clause %lu\n", curr->node_clause->index);
		curr->node_clause->free_lits--;

		//Check if the number of literals is 1 (i.e. perform unit resolution)
		if (curr->node_clause->free_lits == 1)
		{
			unit_lit = get_unit_lit(curr->node_clause);
			printf("found unit lit %ld\n", unit_lit->index);
			dlitList_push_back(get_latest_decision(sat_state)->units,unit_lit);
			
			//Set this literal's unit_on variable
			unit_lit->unit_on = curr->node_clause;

			//Loop through each literal in the clause
			for (int i = 0; i < curr->node_clause->num_lits; i++)
			{
				//Add this unit to each of its parents' list of children
				if (curr->node_clause->literals[i] != unit_lit)
					litList_push(curr->node_clause->literals[i]->unit_children, unit_lit);
			}
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
	printf("undoing unit resolution\n");					
	sat_undo_unit_resolution(sat_state);
	
	printf("undoing set literal decision literal\n");
	//Get decision to undecide
	Decision* undo_dec = decList_pop(sat_state->decisions);

	//Undo the decision of the literal and remove the decision from the list of decisions
	undo_set_literal(undo_dec->dec_lit, sat_state);

	//Free the decision and all its underlying pointers
	free_decision(undo_dec);

	//Decrement the decision level
	sat_state->decision_level--;
}

//Undoes a decision of a literal or a unit resolution of a literal
//@param lit: the literal being un-instantiated
//@param sat_state: the SatState of the CNF
void undo_set_literal(Lit* lit, SatState* sat_state)
{
	printf("undoing literal %ld\n", lit->index);
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
	undo_add_opposite(opp_lit(lit)->clauses);
}

//Undo any subsumptions that occurred due to a decision or unit resolution
//@param lit: the literal on which the clause was subsumed
//@param clauses: the list of clauses containing lit
void undo_subsume_clauses(Lit* lit, clauseList* clauses)
{
	//Create a node to traverse the list
	clauseNode* curr = clauses->head;

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
void undo_add_opposite(clauseList* clauses)
{
	//Create a node to traverse the list
	clauseNode* curr = clauses->head;

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
	if (sat_state != NULL && index > 0 && index <= sat_state->num_clauses)
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
		return 0;
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
		return sat_state->learnedClauses->head->node_clause;
}

//Gets the only literal not instantiated
//@param clause: the clause to be search for a unit literal
//@return the unit literal
Lit* get_unit_lit(Clause* clause)
{
	//Traverse each literal in the clause
	for (int i = 0; i < clause->num_lits; i++)
	{
		printf("get_unit_lit: clause %lu lit %ld has truth_value %d\n", clause->index, clause->literals[i]->index, clause->literals[i]->truth_value);
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
	// Open file for reading
	FILE* file = fopen(file_name, "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open file %s\n", file_name);
		exit(1);
	}

	c2dSize num_vars = 0, num_clauses = 0;
	char line[maxLength];

	// Capture problem line info (i.e. number of variables and clauses)
	while (fgets(line, maxLength, file) != NULL) {
		// Skip comment lines
		if (line[0] == 'c' || line[0] == '%' || line[0] == '0')
			continue;
		// check for problem statement line
		char* token;
		if (line[0] == 'p') {
			token = strtok(line, " \n"); // token == 'p'
			token = strtok(NULL, " \n"); // token == 'cnf'

			token = strtok(NULL, " \n"); // token == <number of variables>
			num_vars = strtoul(token, NULL, 10);

			token = strtok(NULL, " \n"); // token == <number of clauses>
			num_clauses = strtoul(token, NULL, 10);
			break;
		}
	}

	// Create SatState, add basic values
	SatState* satState = (SatState*)malloc(sizeof(SatState));
	satState->num_vars = num_vars;
	satState->num_clauses = num_clauses;
	satState->num_lits = 2 * num_vars;
	satState->decision_level = 1;
	satState->num_learned = 0;
	satState->learnedClauses = (clauseList*) malloc(sizeof(clauseList));
	satState->learnedClauses->head = NULL;
	satState->decisions = (decList*) malloc(sizeof(decList));
	satState->decisions->head = NULL;

	/*************************************************************/
	/*********************SETUP VARS AND LITS*********************/
	/*************************************************************/

	satState->vars = (Var**)malloc((num_vars + 1) * sizeof(Var*));
	satState->lits = (Lit**)malloc(((2 * num_vars) + 1) * sizeof(Lit*));


	// This step sets the start of the literals array in the middle, so we can index in + and - direction
	satState->lits = satState->lits + num_vars;

	satState->vars[0] = NULL;
	satState->lits[0] = NULL;

	for (c2dLiteral i = 1; i <= num_vars; i++) {
		// Create variable and literal structs
		Var* var = (Var*)malloc(sizeof(Var));
		Lit* litp = (Lit*)malloc(sizeof(Lit));
		Lit* litn = (Lit*)malloc(sizeof(Lit));

		// Initialize Variable
		var->index = i;
		var->instantiated = 0;
		var->decision_level = -1;
		var->pos_lit = litp;
		var->neg_lit = litn;
		satState->vars[i] = var; // add to satState

		// Initialize positive literal
		litp->index = i;
		litp->var = var;
		litp->truth_value = -1;
		litp->clauses = (clauseList*) malloc(sizeof(clauseList));
		litp->clauses->head = NULL;
		litp->learnedClauses = (clauseList*) malloc(sizeof(clauseList));
		litp->learnedClauses->head = NULL;
		satState->lits[i] = litp; // add to satState

		// Initialize negative literal
		litn->index = i * -1;
		litn->var = var;
		litn->truth_value = -1;
		litn->clauses = (clauseList*) malloc(sizeof(clauseList));
		litn->clauses->head = NULL;
		litn->learnedClauses = (clauseList*) malloc(sizeof(clauseList));
		litn->learnedClauses->head = NULL;
		satState->lits[i * -1] = litn; // add to satState
	}


	/*************************************************************/
	/************************SETUP CLAUSES************************/
	/*************************************************************/

	// Malloc space for clauses
	Clause* clauses = (Clause*)malloc((num_clauses + 1) * sizeof(Clause)); // 1 indexed array

	// Setup each clause (index starting at 1)
	for (c2dSize i = 1; i <= num_clauses; i++) {
		// Read in line if it is not a comment
		do {
			if (fgets(line, maxLength, file) == NULL) {
				fprintf(stderr, "Read error, or EOF reached before all %ld CNF's read.\n", num_clauses);
				exit(1);
			}
		} while (line[0] == 'c' || line[0] == '%' || line[0] == '0');
		// line now contains our clause string
		// Count number of literals in clause, so we can allocate the array to the appropriate size
		int num_lits = 0, j = 0;
		char* temp = line;
		while (temp[j] != '\0') {
			if (temp[j] == ' ') { // If we reach a space, increment num_lits
				num_lits++;
				while (temp[j + 1] == ' ') // go through all consecutive spaces, 
					j++;
			}
			j++;
		}

		// Create literal array for this clause, of size num_lits
		clauses[i].literals = (Lit**)malloc(num_lits * sizeof(Lit *));

		// Add first literal to clause
		char* lit_string = strtok(line, " \n"); // Get literal from line
		c2dLiteral lit_index = strtol(lit_string, NULL, 10);	// convert from string to signed long
		clauses[i].literals[0] = satState->lits[lit_index];
		
		// Add clause to the literal's clause list.
		clauseList_push(clauses[i].literals[0]->clauses, &(clauses[i]));
		
		// Add rest of literals to clause
		for (int j = 1; j < num_lits; j++) {
			lit_string = strtok(NULL, " \n"); // Get literal from line
			lit_index = strtol(lit_string, NULL, 10); // convert from string to signed long
			clauses[i].literals[j] = satState->lits[lit_index];

			// Add clause to the literal's clause list.
			clauseList_push(clauses[i].literals[j]->clauses, &(clauses[i]));
		}

		// Set the other values for this Clause struct
		clauses[i].subsumed = 0;
		clauses[i].free_lits = num_lits;
		clauses[i].subsumed_on = NULL;
		clauses[i].index = i;
		clauses[i].num_lits = num_lits;
		clauses[i].dec_level = -1;
	}

	satState->CNF = clauses;

	/******DEBUG / TEST SETUP CODE *************
	SatState* s = satState;
	printf("num_lits=%ld num_vars=%lu num_clauses=%lu\n", s->num_lits, s->num_vars, s->num_clauses);
	for(int i = 1; i <= s->num_vars; i++) {
		Var* var = s->vars[i];
		printf("var %lu instantiated %d\n", var->index, var->instantiated);
		printf("negLit %ld posLit %ld \n\n", var->neg_lit->index, var->pos_lit->index);
	}
	// printouts below should match intput CNF format
	for(int i = 1; i <= s->num_clauses; i++) {
		Clause clause = s->CNF[i];
		for(int j = 0; j < clause.num_lits-1; j++) {
			printf("%ld ", clause.literals[j]->index);
		}
		printf("%ld 0\n", clause.literals[clause.num_lits-1]->index);
	}
	// print literals and list of clauses belonging to them
	for(int i = 1; i <= s->num_vars; i++) {
		Lit* lit = s->lits[i];
		printf("lit %ld clauses ", lit->index);
		clauseNode* claws = lit->clauses->head;
		while(claws != NULL) {
			printf("%lu ", claws->node_clause->index);
			claws = claws->next;
		}
		printf("\n");
	}

	for(int i = -1; i >= -1*(s->num_vars); i--) {
		Lit* lit = s->lits[i];
		printf("lit %ld clauses ", lit->index);
		clauseNode* claws = lit->clauses->head;
		while(claws != NULL) {
			printf("%lu ", claws->node_clause->index);
			claws = claws->next;
		}
		printf("\n");
	}
	*************************************/
	return satState;
}

//frees the SatState
void sat_state_free(SatState* sat_state) {
	//TODO: Free dlitList in each Decision of our decList

	// // Free each Var and Lit struct
	for (c2dLiteral i = 1; i <= sat_state->num_vars; i++) {
		// Positive literal
		// Free clause list
		clauseList* list = sat_state->lits[i]->clauses;
		while(clauseList_pop(list) != NULL);
		free(list);
		
		// Free learned clause list
		clauseList* listLearned = sat_state->lits[i]->learnedClauses;
		while(clauseList_pop(listLearned) != NULL);
		free(listLearned);
		
		// Negative literal
		// Free clause list
		list = sat_state->lits[i*-1]->clauses;
		while(clauseList_pop(list) != NULL);
		free(list);
		
		// Free learned clause list
		listLearned = sat_state->lits[i*-1]->learnedClauses;
		while(clauseList_pop(listLearned) != NULL);
		free(listLearned);
		
		// Free Var and Lit structs
		free(sat_state->vars[i]);
		free(sat_state->lits[i]);
		free(sat_state->lits[-1 * i]);
	}

	// Free list of Var/Lit pointers in sat_state
	free(sat_state->vars);
	free(sat_state->lits - sat_state->num_vars);

	// Free list of Lit pointers in each clause
	for (c2dSize i = 1; i <= sat_state->num_clauses; i++) {
		free(sat_state->CNF[i].literals);
	}
	
	// Free list of Clause pointers in sat_state
	free(sat_state->CNF);
	
	// Free learned clause list in satState
	clauseList* listLearned = sat_state->learnedClauses;
	while(clauseList_pop(listLearned) != NULL);
	free(listLearned);
	
	// Free decisions list in satState
	decList* decisionList = sat_state->decisions;
	while(decList_pop(decisionList) != NULL);
	free(decisionList);

	// Free sat_state struct itself
	free(sat_state);
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
	printf("\nstart unit resolution at decision_level = %d\n", sat_state->decision_level);
	//Run a special unit resolution if no decision has been made
	if (sat_state->decision_level == 1) {
		printf("run initial unit resolution\n");
		return initial_unit_resolution(sat_state);
	}

	//Create a litNode to traverse the decision's list of unit literals
	Decision* decision = get_latest_decision(sat_state);
	if(decision->units != NULL) {
		dlitNode* trav = decision->units->head;
		//While not at the end of the literal list
		while (trav != NULL)
		{	
			printf("We found a unit literal to resolve! lit %ld\n", trav->node_lit->index);
			//Set the literal and return 0 if unit resolution returns a contradiction clause
			if (set_literal(trav->node_lit, sat_state) != NULL)
				return 0;

			trav = trav->next;
		}
	}
	printf("end unit resolution\n\n");
	return 1;		//Resolution completed without returning a contradiction clause i.e. unit resolution succeeded
}

//Check all of the initial clauses in the CNF are unit, and perform unit resolution if they are
//DOES NOT CHECK LEARNED CLAUSES
//@param sat_state: the SatState to investigate
//@return 0 if a contradiction is found, 1 otherwise
BOOLEAN initial_unit_resolution(SatState* sat_state)
{
	//Loop through each clause in the CNF
	for (int i = 0; i < sat_state->num_clauses; i++)
	{
		//If the clause has one free literal, perform unit resolution
		if (sat_state->CNF[i].free_lits == 1)
		{
			//Check if a contradiction is found when setting the unit literal
			if (set_literal(get_unit_lit(&sat_state->CNF[i]), sat_state) != NULL)
				return 0;
		}
	}
	return 1;
}

//undoes sat_unit_resolution(), leading to un-instantiating variables that have been instantiated
//after sat_unit_resolution()
void sat_undo_unit_resolution(SatState* sat_state) {
	//Special case if at decision level 1
	if (sat_state->decision_level == 1)
		undo_all_resolution(sat_state);
	
	//Create a litNode to traverse the decision's list of unit literals
	dlitNode* trav = get_latest_decision(sat_state)->units->tail;

	//While not at the end of the literal list
	while (trav != NULL)
	{
		//Undo the unit resolution on the current literal
		undo_set_literal((Lit*)trav->node_lit, sat_state);
		
		//Go to the next unit to undo resolution on
		trav = trav->prev;
	}
}

//Undoes resolution at decision level 0 (i.e. before a decision is made)
//DOES NOT CHECK LEARNED CLAUSES
//@param sat_state: the SatState to undo resolution on
void undo_all_resolution(SatState* sat_state)
{
	//Loop through all clauses
	for (int i = 0; i < sat_state->num_clauses; i++)
	{
		//If the clause contains 1 literal, undo the setting of that literal
		if (sat_state->CNF[i].num_lits == 1)
			undo_set_literal(sat_state->CNF[i].literals[0], sat_state);
	}
}

//Free the decision and all its underlying pointers
//@param undo_dec: decision to free
void free_decision(Decision* undo_dec)
{
	//Literal that stores which unit literal is currently being inspected
	Lit* free_lit;

	//Free the units list and all literal information in each literal that became unit based on this decision
	while (undo_dec->units->head != NULL)
	{
		free_lit = dlitList_pop(undo_dec->units);
		free_lit->unit_on = NULL;
		free_lit->in_contradiction_clause = 0;

		while (free_lit->unit_children != NULL)
			litList_pop(free_lit->unit_children);
	}

	//Free the implication graph list
	while (undo_dec->implication_graph->head != NULL)
		dlitList_pop(undo_dec->implication_graph);

	//Free the decision itself
	free(undo_dec);
}


//returns 1 if the decision level of the sat state equals to the assertion level of clause,
//0 otherwise
//
//this function is called after sat_decide_literal() or sat_assert_clause() returns clause.
//it is used to decide whether the sat state is at the right decision level for adding clause.
BOOLEAN sat_at_assertion_level(const Clause* clause, const SatState* sat_state) {
	if (clause->dec_level == sat_state->decision_level)
		return 1;
	return 0;
}

// Returns the latest decision made in the current SatState
Decision* get_latest_decision(SatState* sat_state) {
	if(sat_state == NULL)
		return NULL;
	else if(sat_state->decisions->head == NULL)
		return NULL;
	else {
		return sat_state->decisions->head->node_dec;
	}
}

void debug_print_clauses(SatState* sat_state) {
	/************* DEBUG PRINTOUTS ********************/
	printf("Clauses at decision level %d\n", sat_state->decision_level);
	for(c2dSize i = 1; i < sat_state->num_clauses; i++) {
		Clause* clause = sat_index2clause(i, sat_state);
		printf("Clause %lu has %d free lits currently, and subsumed = %d\n", clause->index, clause->free_lits, clause->subsumed);
		for(c2dSize j = 0; j < clause->num_lits; j++) {
			printf("%ld ", clause->literals[j]->index);
		}
		printf("\n");
	}
}

//Gets the uip
//@param contradiction: the contradiction clause found at the current level
//@param sat_state: the SatState to search
//@return the shared literal/uip of the implication graph
Lit* sat_get_uip(Clause* contradiction, SatState* sat_state)
{
	//Get list of literals at this decision level on the path to the contradiction clause
	find_uip_lits(contradiction, sat_state);

	//If there is only one literal at  this decision level in the contradiction clause, then it is the contradiction literal
	if (sat_state->decisions->head->node_dec->contradiction_lits == 1)
		return sat_state->decisions->head->node_dec->implication_graph->head->node_lit;

	//Node for traveling the implication graph literals in reverse order
	dlitNode* trav = sat_state->decisions->head->node_dec->implication_graph->head;

	//Counter to track which literal is being inspected (for skipping the literals in the contradiction clause)
	int lit_counter = 0;

	//For each literal in the implication graph until the decision literal is reached (reverse order)
	while (trav->node_lit != sat_state->decisions->head->node_dec->dec_lit)
	{
		//No need to inspect the literals in the contradiction clause since there is more than 1 (cannot be the uip)
		if (lit_counter >= sat_state->decisions->head->node_dec->contradiction_lits)
		{
			//Flag the literal to be ignored by DFS
			trav->node_lit->DFS_ignore = 1;

			//If the DFS cannot reach the contradiction clause, this is the uip
			if (uip_DFS(sat_state) == 0)
				return trav->node_lit;

			//Remove the DFS flag
			trav->node_lit->DFS_ignore = 0;
		}

		//Move to the next literal
		trav = trav->next;
		lit_counter++;
	}

	//The decision literal has been reached, it must be the implcation literal
	return sat_state->decisions->head->node_dec->dec_lit;
}

//Gets a queue of literals leading to the contradiciton clause in reverse order
//@param contradiction: the contradiction clause found at the current level
//@param sat_state: the SatState to search
void find_uip_lits(Clause* contradiction, SatState* sat_state)
{
	dlitNode* curr;			//Literal currently being inspected
	sat_state->decisions->head->node_dec->contradiction_lits = 0;	//Reset the number of literals at this decision level in the contradiction clause

	//Check each literal in the contradiction clause
	for (int i = 0; i < contradiction->num_lits; i++)
	{
		//If the literal is at the current decision level, add it to the list of literals to inspect
		if (sat_literal_var(contradiction->literals[i])->decision_level == sat_state->decision_level)
		{
			//Add the literal to the list of literals to inspect
			dlitList_push_back(sat_state->decisions->head->node_dec->implication_graph, contradiction->literals[i]);

			//This literal is in the contradiction clause, mark it as such
			contradiction->literals[i]->in_contradiction_clause = 1;

			//Increment the number of literals at this decision level in the contradiction clause
			sat_state->decisions->head->node_dec->contradiction_lits++;
		}
	}

	//Get the first literal to inspect
	curr = sat_state->decisions->head->node_dec->implication_graph->head;

	//Repeat until all literals that lead to the contradiction clause at its decision level have been inspected
	while (curr != NULL)
	{
		//Check every literal in the clause that led to the unit resolution of the currently inspected literal
		for (int i = 0; i < curr->node_lit->unit_on->num_lits; i++)
		{
			//Add the literal to the list of literals to inspect if it is at this decision level
			if (sat_literal_var(curr->node_lit->unit_on->literals[i])->decision_level == sat_state->decision_level)
				dlitList_push_back(sat_state->decisions->head->node_dec->implication_graph, curr->node_lit->unit_on->literals[i]);
		}
		curr = curr->next;
	}
}

//Perform DFS from the decision literal to the contradiciton clause
//@param sat_state: the SatState to search
//@return false if the contradiction clause could not be reached
BOOLEAN uip_DFS(SatState* sat_state)
{
	//Create a DFS stack and add the first decision literal to it
	litList* stack_DFS = (litList*)malloc(sizeof(litList));
	litList_push(stack_DFS, sat_state->decisions->head->node_dec->dec_lit);

	//Literal to check whether or not is in the contradiction clause
	Lit* in_cc;

	//Repeat until the stack is empty
	while (stack_DFS->head != NULL)
	{
		//Check the next literal
		in_cc = litList_pop(stack_DFS);

		//If the literal is in the contradiction clause, the contradiciton clause can be reached
		if (in_cc->in_contradiction_clause == 1)
			return 1;
	}

	//Contradiction clause could not be reached (
	return 0;
}

//Get the assertion clause
//@param contradiction: the contradiction clause found at the current level
//@param sat_state: the SatState to search
//@return the assertion clause
Clause* get_assertion_clause(Clause* contradiction, SatState* sat_state)
{
	//Create the assertion clause
	Clause* assertion = (Clause*)malloc(sizeof(Clause));

	//Initialize the clause
	assertion->subsumed = 0;		
	assertion->subsumed_on = NULL;
	assertion->index = sat_state->num_learned;

	//List to store literals in assertion clause (clause has an array of literals, so this is temporary storage until size is found)
	litList* temp_assert_lits = (litList*)malloc(sizeof(litList));

	//Number of literals in the assertion clause (for creating the array of literals in the assertion clause)
	assertion->num_lits = 1;

	/**NON-DECISION LEVEL LITERALS IN CONTRADICTION CLAUSE**/
	//Add opposite of every non-decision literal in the contradiction clause to the assertion clause
	for (int i = 0; i < contradiction->num_lits; i++)
	{ 
		//If the literal was not learned at this decision level add its opposite to the assertion clause
		if (sat_literal_var(contradiction->literals[i])->decision_level != sat_state->decision_level)
			litList_push(temp_assert_lits, opp_lit(contradiction->literals[i]));
	}

	/**UIP**/
	//Get the uip
	Lit* uip = sat_get_uip(contradiction, sat_state);

	//Add opposite of uip to assertion clause
	litList_push(temp_assert_lits,opp_lit(uip));

	/**NON-DECISION LEVEL PARENTS OF UNIT CHILDREN OF UIP**/
	//Node for traversing the list of unit children of uip
	litNode* trav = uip->unit_children->head;

	//Traverse every unit child of the uip
	while (trav != NULL)
	{
		//Add opposite of non-decision level parents of uip's children to assertion clause
		for (int i = 0; i < trav->node_lit->unit_on->num_lits; i++)
		{
			//If the literal was not learned at this decision level add its opposite to the assertion clause
			if (sat_literal_var(trav->node_lit->unit_on->literals[i])->decision_level != sat_state->decision_level)
				litList_push(temp_assert_lits, opp_lit(trav->node_lit->unit_on->literals[i]));
		}
		trav = trav->next;	//Move to the next unit child literal
	}
	
	//Create the list of literals in the clause
	assertion->literals = (Lit**)malloc(assertion->num_lits * sizeof(Lit *));
	assertion->free_lits = assertion->num_lits;
	 
	//Place each literal into the list of literals in the clause
	for (int i = 0; i < assertion->num_lits; i++)
		assertion->literals[i] = litList_pop(temp_assert_lits);

	free(temp_assert_lits);

	return assertion;
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
