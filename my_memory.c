/*****************************************************************
 *
 * File             : my_memory.c
 * Description      :
 *
 * Author           : @author(s)
 * Last Modified    : @date
 *
**/
// Include files
#include <stdio.h>
#include <stdlib.h>
#include "memalloc.h"

#define FIRST_FIT       0
#define BEST_FIT        1
#define WORST_FIT       2
#define BUDDY_SYSTEM    3

typedef struct Hole_Node_
{
    int size;
    int location;
    struct Hole_Node_* next;
} Hole_Node;

void setup(int, int, void*);
void *my_malloc(int);
void my_free(void*);
int num_free_bytes();
int num_holes();

//global vars for memory space information
int memory_size;
int malloc_type;
void* memory_start;

//linked list of holes, initially one node for entire space
Hole_Node* hole_list;

void setup( int malloc_type_, int mem_size, void* start_of_memory ) {
    malloc_type = malloc_type_;
    memory_size = mem_size;
    memory_start = start_of_memory;

    Hole_Node* starter_node = malloc(sizeof(Hole_Node));
    starter_node->size = memory_size;
    starter_node->location = memory_start;
    hole_list = starter_node;
}
// searches through holes based on type, adds header at start of hole, updates holes list, returns location+1
void *my_malloc(int size) {
    if (malloc_type = BEST_FIT){
        Hole_Node* curr = hole_list;
        while (hole_list->next!= NULL)
        {
            
        }
    }

    return (void*)-1;
}

void my_free(void *ptr) {
	//given pointer to where the allocation starts
	//must find if the previous space is a hole and if so merge
	//and check if the next space is a hole and if so, merge
	
	//start address of allocated memory
	char* alloc_start = ptr - sizeof(int);
	
	//get size of allocated space
	int size = *alloc_start;
	
	//find end of allocated space
	char* alloc_end = ptr + size;
	
	//initialize search node
	Hole_Node* search = hole_list;
	
	//check if first hole is directly before the space to free
	if (((search->location)+(search->size)) == alloc_start) {
		//merge these holes
		search->size += size + 1;
		
	//otherwise, search for hole directly before the memory to be freed
	} else {
		//iterate through LL and stop if location+size >= alloc_start
		while (((search->next->location)+(search->next->size)) < alloc_start) {
			search = search->next;
		}
		
		//check if the previous hole is directly before the space to free
		if (((search->next->location)+(search->next->size)) == alloc_start) {
			//merge these holes
			search->next->size += size + 1;
		}
	}

	//search->next should be at the hole before the memory to be freed
	//so, advance search once
	search = search->next;
	
	//then, check if the end of that hole is the beginning of the next hole
	if (((search->location)+(search->size)) == (search->next->location)) {
		//merge these holes
		search->size += search->next->size;
		
		//update linked list
		search->next = search->next->next;
	}
}

int num_free_bytes() {
    return 0;
}

int num_holes() {
    return 0;
}
