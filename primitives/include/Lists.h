#ifndef LISTS_H_
#define LISTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count

/******************************************************************************
* LIST OF  c2dSize variables
******************************************************************************/

//List node for c2dSize lists
typedef struct c2dSizeNode {
	c2dSize node_c2dSize;			//Index of variable the node refers to
	Var* variable;					//Pointer to the variable itself
	struct c2dSizeNode *next;		//Next literal node in the list
};

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
c2dSize c2dSizeList_pop(struct c2dSizeNode* head)
{
	if (head == NULL)
		return NULL;

	//Get the literal pointed to by the head of the list
	c2dSize head_c2dSize = head->node_c2dSize;

	//Get the next node pointed to by the head of the list
	struct c2dSizeNode* temp_next = head->next;

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
	//Create a node for the new literal
	struct c2dSizeNode *new_node = (struct c2dSizeNode*) malloc(sizeof(struct c2dSizeNode));
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
};

//Pop the head from the list
//@return the literal pointed to by the former head of the list (i.e. the removed node's literal)
struct Lit* litList_pop(struct litNode* head)
{
	//Get the literal pointed to by the head of the list
	struct Lit* head_lit = head->node_lit;

	//Get the next node pointed to by the head of the list
	struct litNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_lit;
}

//This function should ONLY BE CALLED IF THE LIST DOES NOT HAVE A TAIL
void litList_push(struct litNode* head, struct Lit* new_lit)
{
	//Create a node for the new literal
	struct litNode *new_node = (struct litNode*)malloc(sizeof(struct litNode));
	new_node->node_lit = new_lit;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

//This function should ONLY BE CALLED IF THE LIST HAS A HEAD AND A TAIL
void litList_push_back(struct litNode* head, struct litNode* tail, struct Lit* new_lit)
{
	//Create a node for the new literal
	struct litNode *new_node = (struct litNode*)malloc(sizeof(struct litNode));
	new_node->node_lit = new_lit;

	if (head == NULL)
		head = tail = new_lit;
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
};

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
Decision* decList_pop(struct decNode* head)
{
	//Get the literal pointed to by the head of the list
	struct Decision* head_dec = head->node_dec;

	//Get the next node pointed to by the head of the list
	struct decNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_dec;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void decList_push(struct decNode* head, struct Decision* new_dec)
{
	//Create a node for the new literal
	struct decNode *new_node = (struct decNode*)malloc(sizeof(struct decNode));
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
	struct clause *node_clause;		//Literal the node refers to
	struct clauseNode *next;		//Next literal node in the list
};

//Pop the head from the list
//@return the decision pointed to by the former head of the list (i.e. the removed node's literal)
Clause* clauseList_pop(struct clauseNode* head)
{
	//Get the literal pointed to by the head of the list
	struct Clause* head_clause = head->node_clause;

	//Get the next node pointed to by the head of the list
	struct clauseNode* temp_next = head->next;

	//Free the memory allocated in the head node
	free(head);

	//Set the head node to the second node in the list
	head = temp_next;

	//Return the literal pointed to by the former head of the list
	return head_clause;
}

//Push a new node onto the list
//@param new_dec: the decision to be pushed to the list
void clauseList_push(struct clauseNode* head, struct Clause* new_clause)
{
	//Create a node for the new literal
	struct clauseNode *new_node = (struct clauseNode*)malloc(sizeof(struct clauseNode));
	new_node->node_clause = new_clause;

	//Set the new node's successor to the head of the list
	new_node->next = head;

	//Set the head of the list to the new node
	head = new_node;
}

#endif