#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct array_list_t {
	void **arr;						// Array of pointers of any type
	size_t size;						// Total size of the array currently
	unsigned int amt_filled;	// Amount of the array currently used
} ArrayList;

// Initialize array list
ArrayList* initArrayList() {
	ArrayList* arraylist = (ArrayList*) malloc(sizeof(ArrayList)) ;
	// Initial array of size 5
	arraylist->arr = (void **) malloc(5*sizeof(void *));
	arraylist->size = 5;
	arraylist->amt_filled = 0;
	return arraylist;
}

// Get item at index of array.
void* get(ArrayList* arraylist, int index) {
	// Bounds check
	if(index < 0 || index >= arraylist->amt_filled) {
		return NULL;
	} else {
		return arraylist->arr[index];
	}
}

// Add item to end of array, expanding by 5 spaces if necessary
void push_tail(ArrayList* arraylist, void* item) {
	// Check if we need to expand the array. If so, add 5 more slots into the array.
	if(arraylist->amt_filled == arraylist->size) {
		arraylist->size += 5;
		void ** temp_ptr = realloc(arraylist->arr, arraylist->size * sizeof(void *));
		if(temp_ptr == NULL) {
			fprintf(stderr, "ERROR: realloc failed\n");
		} else {
			arraylist->arr = temp_ptr;
		}
	}
	// Add to end of array
	arraylist->arr[arraylist->amt_filled] = item;
	arraylist->amt_filled++;
}

#endif
