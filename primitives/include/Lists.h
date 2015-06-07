#ifndef LISTS_H_
#define LISTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count
typedef struct literal Lit;
typedef struct decision Decision;
typedef struct clause Clause;

/******************************************************************************
* LIST OF literals
******************************************************************************/

//List node for literal lists
typedef struct litNode {
	Lit *node_lit;		//Pointer to the literal the node refers to
	struct litNode *next;		//Next literal node in the list
} litNode;

//List of literals
typedef struct litList {
	litNode* head;			//Node pointer to the head of the list
} litList;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
Lit* litList_pop(litList* list)
{
	// Check for empty list before popping
	if(list->head == NULL) {
		return NULL;
	}
	
	Lit* head_lit;					//Stores the literal stored in the head of the list
	litNode* temp_next;				//Temporarily stores the second node in the list

	//Get the literal pointed to by the head of the list
	head_lit = list->head->node_lit;

	//Get the next node pointed to by the head of the list
	temp_next = list->head->next;

	//Free the memory allocated in the head node
	free(list->head);

	//Set the head node to the second node in the list
	list->head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_lit;
}

//This function should ONLY BE CALLED IF THE LIST DOES NOT HAVE A TAIL
void litList_push(litList* list, Lit* new_lit)
{
	litNode* new_node;		//Will store the new node

	//Create a node for the new literal
	new_node = (litNode*)malloc(sizeof(litNode));
	new_node->node_lit = new_lit;

	//Set the new node's successor to the head of the list
	new_node->next = list->head;

	//Set the head of the list to the new node
	list->head = new_node;
}

/******************************************************************************
* LIST OF  decision variables
******************************************************************************/

//List node for decision lists (stack structure)
typedef struct decNode {
	Decision *node_dec;		//Literal the node refers to
	struct decNode *next;			//Next literal node in the list
}decNode;

//List of decisions
typedef struct decList {
	decNode* head;			//Node pointer to the head of the list
} decList;

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
Decision* decList_pop(decList* list)
{
	// Check for empty list before popping
	if(list->head == NULL) {
		return NULL;
	}
	
	Decision* head_dec;		//Stores the decision at the head of the list
	decNode* temp_next;				//Stores the second node in the list

	//Get the literal pointed to by the head of the list
	head_dec = list->head->node_dec;

	//Get the next node pointed to by the head of the list
	temp_next = list->head->next;

	//Free the memory allocated in the head node
	free(list->head);

	//Set the head node to the second node in the list
	list->head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_dec;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void decList_push(decList* list, Decision* new_dec)
{
	decNode *new_node;		//Store the new node

	//Create a node for the new literal
	new_node = (decNode*)malloc(sizeof(decNode));
	new_node->node_dec = new_dec;

	//Set the new node's successor to the head of the list
	new_node->next = list->head;

	//Set the head of the list to the new node
	list->head = new_node;
}

/******************************************************************************
* LIST OF clauses
******************************************************************************/

//List node for decision lists (stack structure)
typedef struct clauseNode {
	Clause *node_clause;			//Literal the node refers to
	struct clauseNode *next;			//Next literal node in the list
} clauseNode;

//List of clauses
typedef struct clauseList {
	clauseNode* head;			//Node pointer to the head of the list
} clauseList;

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
Clause* clauseList_pop(clauseList* list)
{
	// Check for empty list before popping
	if(list->head == NULL) {
		return NULL;
	}

	Clause* head_clause;			//Stores the clause in the head of the list
	clauseNode* temp_next;				//Stores the second node in the list

	//Get the literal pointed to by the head of the list
	head_clause = list->head->node_clause;

	//Get the next node pointed to by the head of the list
	temp_next = list->head->next;

	//Free the memory allocated in the head node
	free(list->head);

	//Set the head node to the second node in the list
	list->head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_clause;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void clauseList_push(clauseList* list, Clause* new_clause)
{
	clauseNode* new_node;		//Stores the new node

	//Create a node for the new literal
	new_node = (clauseNode*)malloc(sizeof(clauseNode));
	new_node->node_clause = new_clause;

	//Set the new node's successor to the head of the list
	new_node->next = list->head;

	//Set the head of the list to the new node
	list->head = new_node;
}

/******************************************************************************
* DOUBLY LINKED LIST OF literals
******************************************************************************/

//List node for literal lists
typedef struct dlitNode {
	Lit* node_lit;				//Pointer to the literal the node refers to
	struct dlitNode *next;		//Next literal node in the list
	struct dlitNode *prev;		//Previous literal node in the list
} dlitNode;

//Doubly linked list of literals
typedef struct dlitList {
	dlitNode* head;			//Node pointer to the head of the list
	dlitNode* tail;			//Node pointer to the tail of the list
} dlitList;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
Lit* dlitList_pop(dlitList* list)
{
	//printf("popping from list->head=%p, list->tail=%p\n", list->head, list->tail);
	Lit* head_lit;		//Stores the literal in the head of the list
	dlitNode* temp_next;	//Stores the second node in the list

	//If the list is empty, return NULL
	if (list->head == NULL)
		return NULL;

	//Get the literal pointed to by the head of the list
	head_lit = list->head->node_lit;

	if( list->head == list->tail) {
		free(list->head);
		list->head = NULL;
		list->tail = NULL;
		return head_lit;
	}

	//Get the next node pointed to by the head of the list
	temp_next = list->head->next;

	//Free the memory allocated in the head node
	free(list->head);

	//Set the head node to the second node in the list
	list->head = temp_next;
	
	// Gotta change the new head's prev node to NULL
	if(list->head != NULL) {
		list->head->prev = NULL;
	}
	
	//list->head->prev = NULL; Breaks code
	
	//Return the literal pointed to by the former head of the list
	return head_lit;
}

//Pop the tail from the list
//@return the literal pointed to by the former tail of the list (i.e. the removed node's literal)
Lit* dlitList_pop_tail(dlitList* list)
{
	//printf("popping from TAIL  list->head=%p, list->tail=%p\n", list->head, list->tail);
	Lit* tail_lit;		//Stores the literal in the tail of the list
	dlitNode* temp_prev;	//Stores the second node in the list

	//If the list is empty, return NULL
	if (list->tail == NULL)
		return NULL;

	//Get the literal pointed to by the tail of the list
	tail_lit = list->tail->node_lit;

	// If just one node, handle accordingly
	if( list->head == list->tail) {
		free(list->head);
		list->head = list->tail = NULL;
		return tail_lit;
	}
	
	//Get the next node pointed to by the tail of the list
	temp_prev = list->tail->prev;

	//Free the memory allocated in the tail node
	free(list->tail);

	//Set the tail node to the second to last node in the list
	list->tail = temp_prev;
	
	// Set tail's new node's next to null
	if(list->tail != NULL)
		list->tail->next = NULL;

	//Return the literal pointed to by the former tail of the list
	return tail_lit;
}

//This function should ONLY BE CALLED IF THE LIST HAS A HEAD AND A TAIL
void dlitList_push_back(dlitList* list, Lit* new_lit)
{
	dlitNode *new_node;		//Stores the new node

	//Create a node for the new literal
	new_node = (dlitNode*)malloc(sizeof(dlitNode));
	new_node->node_lit = new_lit;
	new_node->next = NULL;

	if (list->head == NULL)
	{
		list->head = new_node;
		list->tail = new_node;
		new_node->prev = NULL;
	}
	else
	{
		//Set the new node's successor to the head of the list
		list->tail->next = new_node;

		//Set the new node's predecessor to the tail of the list
		new_node->prev = list->tail;

		//Set the head of the list to the new node
		list->tail = new_node;
	}
	//printf("Pushed to list->head=%p, list->tail=%p\n", list->head, list->tail);
}


#endif
