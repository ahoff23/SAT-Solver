#ifndef LISTS_H_
#define LISTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count

typedef struct var Var;
typedef struct literal Lit;
typedef struct clause Clause;
typedef struct sat_state_t SatState;
typedef struct decision Decision;

/******************************************************************************
* LIST OF  c2dSize variables
******************************************************************************/


/*********** Removing for compiling sake

//List node for c2dSize lists
typedef struct c2dSizeNode_t {
	c2dSize node_c2dSize;			//Index of variable the node refers to
	Var* variable;					//Pointer to the variable itself
	struct c2dSizeNode_t *next;		//Next literal node in the list
} c2dSizeNode;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
c2dSize c2dSizeList_pop(c2dSizeNode* head)
{
	if (head == NULL)
		return NULL;

	//Get the literal pointed to by the head of the list
	c2dSize head_c2dSize = head->node_c2dSize;

	//Get the next node pointed to by the head of the list
	c2dSizeNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_c2dSize;
}

//Push a new node onto the list
//@param new_lit: The literal to be pushed to the list
void c2dSizeList_push(c2dSizeNode* head, c2dSize new_c2dSize)
{
	//Create a node for the new literal
	c2dSizeNode *new_node = (c2dSizeNode*) malloc(sizeof(c2dSizeNode));
	new_node->node_c2dSize = new_c2dSize;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

*////////// End of c2dSizeNode


/******************************************************************************
* LIST OF literals
******************************************************************************/

//List node for literal lists
typedef struct litNode_t {
	Lit *node_lit;		//Index of the literal the node refers to
	Lit* literal;				//Pointer to the literal
	struct litNode_t *next;		//Next literal node in the list
} litNode;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
Lit* litList_pop(litNode* head)
{
	//Get the literal pointed to by the head of the list
	Lit* head_lit = head->node_lit;

	//Get the next node pointed to by the head of the list
	litNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_lit;
}

void litList_push(litNode* head, Lit* new_lit)
{
	//Create a node for the new literal
	litNode *new_node = (litNode*)malloc(sizeof(litNode));
	new_node->node_lit = new_lit;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

/******************************************************************************
* LIST OF  decision variables
******************************************************************************/

//List node for decision lists (stack structure)
typedef struct decNode_t {
	Decision *node_dec;		//Literal the node refers to
	struct decNode_t *next;			//Next literal node in the list
} decNode;

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
Decision* decList_pop(decNode* head)
{
	//Get the literal pointed to by the head of the list
Decision* head_dec = head->node_dec;

	//Get the next node pointed to by the head of the list
	decNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_dec;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void decList_push(decNode* head, Decision* new_dec)
{
	//Create a node for the new literal
	decNode *new_node = (decNode*)malloc(sizeof(decNode));
	new_node->node_dec = new_dec;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

/******************************************************************************
* LIST OF clauses
******************************************************************************/

//List node for decision lists (stack structure)
typedef struct clauseNode_t {
	Clause *node_clause;		//Literal the node refers to
	struct clauseNode_t *next;		//Next literal node in the list
} clauseNode;

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
Clause* clauseList_pop(clauseNode* head)
{
	//Get the literal pointed to by the head of the list
	Clause* head_clause = head->node_clause;

	//Get the next node pointed to by the head of the list
	clauseNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_clause;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void clauseList_push(clauseNode* head, Clause* new_clause)
{
	//Create a node for the new literal
	clauseNode *new_node = (clauseNode*)malloc(sizeof(clauseNode));
	new_node->node_clause = new_clause;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

#endif
