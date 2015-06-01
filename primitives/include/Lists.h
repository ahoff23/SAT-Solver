#ifndef LISTS_H_
#define LISTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count
struct lit;
struct decision;
struct clause;

/******************************************************************************
* LIST OF  c2dSize variables
******************************************************************************/

//List node for c2dSize lists
typedef struct c2dSizeNode {
	c2dSize node_c2dSize;			//Index of variable the node refers to
	struct var* variable;			//Pointer to the variable itself
	struct c2dSizeNode *next;		//Next literal node in the list
} c2dSizeNode;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
c2dSize c2dSizeList_pop(struct c2dSizeNode* head)
{
	
	c2dSize head_c2dSize;				//Stores the size of the head
	struct c2dSizeNode* temp_next;		//Temporarily stores the head of the list

	if (head == NULL)
		return 0;

	//Get the literal pointed to by the head of the list
	head_c2dSize = head->node_c2dSize;

	//Get the next node pointed to by the head of the list
	temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_c2dSize;
}

//Push a new node onto the list
//@param new_lit: The literal to be pushed to the list
void c2dSizeList_push(struct c2dSizeNode* head, c2dSize new_c2dSize)
{
	struct c2dSizeNode *new_node;		//Will be the new node

	//Create a node for the new literal
	new_node = (struct c2dSizeNode*) malloc(sizeof(struct c2dSizeNode));
	new_node->node_c2dSize = new_c2dSize;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}


/******************************************************************************
* LIST OF literals
******************************************************************************/

//List node for literal lists
typedef struct litNode {
	struct lit *node_lit;		//Pointer to the literal the node refers to
	struct litNode *next;		//Next literal node in the list
} litNode;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
struct lit* litList_pop(struct litNode* head)
{
	struct lit* head_lit;					//Stores the literal stored in the head of the list
	struct litNode* temp_next;		//Temporarily stores the second node in the list

	//Get the literal pointed to by the head of the list
	head_lit = head->node_lit;

	//Get the next node pointed to by the head of the list
	temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_lit;
}

//This function should ONLY BE CALLED IF THE LIST DOES NOT HAVE A TAIL
void litList_push(struct litNode* head, struct lit* new_lit)
{
	struct litNode* new_node;		//Will store the new node

	//Create a node for the new literal
	new_node = (struct litNode*)malloc(sizeof(struct litNode));
	new_node->node_lit = new_lit;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

//This function should ONLY BE CALLED IF THE LIST HAS A HEAD AND A TAIL
void litList_push_back(litNode* head, litNode* tail, struct lit* new_lit)
{
	litNode* new_node;		//Will store the new node

	//Create a node for the new literal
	new_node = (struct litNode*)malloc(sizeof(struct litNode));
	new_node->node_lit = new_lit;

	if (head == NULL)
		head = tail = new_node;
	else
	{
		//Set the new node's successor to the head of the list
		tail->next = new_node;

		//Set the head of the list to the new node
		tail = new_node;
	}
}

/******************************************************************************
* LIST OF  decision variables
******************************************************************************/

//List node for decision lists (stack structure)
typedef struct decNode {
	struct decision *node_dec;		//Literal the node refers to
	struct decNode *next;			//Next literal node in the list
}decNode;

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
struct decision* decList_pop(struct decNode* head)
{
	struct decision* head_dec;		//Stores the decision at the head of the list
	struct decNode* temp_next;		//Stores the second node in the list

	//Get the literal pointed to by the head of the list
	head_dec = head->node_dec;

	//Get the next node pointed to by the head of the list
	temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_dec;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void decList_push(struct decNode* head, struct decision* new_dec)
{
	struct decNode *new_node;		//Store the new node

	//Create a node for the new literal
	new_node = (struct decNode*)malloc(sizeof(struct decNode));
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
typedef struct clauseNode {
	struct clause *node_clause;			//Literal the node refers to
	struct clauseNode *next;		//Next literal node in the list
} clauseNode;

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
struct clause* clauseList_pop(struct clauseNode* head)
{
	struct clause* head_clause;			//Stores the clause in the head of the list
	struct clauseNode* temp_next;	//Stores the second node in the list

	//Get the literal pointed to by the head of the list
	head_clause = head->node_clause;

	//Get the next node pointed to by the head of the list
	temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_clause;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void clauseList_push(struct clauseNode* head, struct clause* new_clause)
{
	struct clauseNode* new_node;		//Stores the new node

	//Create a node for the new literal
	new_node = (struct clauseNode*)malloc(sizeof(struct clauseNode));
	new_node->node_clause = new_clause;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

/******************************************************************************
* DOUBLY LINKED LIST OF literals
******************************************************************************/

//List node for literal lists
typedef struct dlitNode {
	struct lit* node_lit;				//Pointer to the literal the node refers to
	struct dlitNode *next;		//Next literal node in the list
	struct dlitNode *prev;		//Previous literal node in the list
} dlitNode;

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
struct lit* dlitList_pop(struct dlitNode* head)
{
	struct lit* head_lit;		//Stores the literal in the head of the list
	struct dlitNode* temp_next;	//Stores the second node in the list

	//If the list is empty, return NULL
	if (head == NULL)
		return NULL;

	//Get the literal pointed to by the head of the list
	head_lit = head->node_lit;

	//Get the next node pointed to by the head of the list
	temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_lit;
}

//This function should ONLY BE CALLED IF THE LIST HAS A HEAD AND A TAIL
void dlitList_push_back(dlitNode* head, dlitNode* tail, struct lit* new_lit)
{
	struct dlitNode *new_node;		//Stores the new node

	//Create a node for the new literal
	new_node = (struct dlitNode*)malloc(sizeof(struct dlitNode));
	new_node->node_lit = new_lit;

	if (head == NULL)
		head = tail = new_node;
	else
	{
		//Set the new node's successor to the head of the list
		tail->next = new_node;

		//Set the new node's predecessor to the tail of the list
		new_node->prev = tail;

		//Set the head of the list to the new node
		tail = new_node;
	}
}


#endif