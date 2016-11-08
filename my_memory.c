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
#include <limits.h>
#include "memalloc.h"

typedef struct Hole_Node_
{
    int size;
    char* location;
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
char* memory_start;

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
// need to test if increment by 4 or 1
void *my_malloc(int size) {
    int request = size+4;
    Hole_Node* chosen_hole = NULL;
    Hole_Node* chosen_prev = NULL;

    if (malloc_type == FIRST_FIT){
        Hole_Node* curr = hole_list;
        Hole_Node* prev = NULL;
        while (curr->size < request)
        {
            prev = curr;
            curr = curr->next;
            if (curr == NULL) // end of list, no holes were big enough
            {
                return (void*)-1;
            }
        }
        chosen_hole = curr;
        chosen_prev = prev;
    }
    if (malloc_type == BEST_FIT)
    {
        Hole_Node* curr = hole_list;
        Hole_Node* prev = NULL;
        Hole_Node* best = NULL;
        Hole_Node* best_prev = NULL;
        int best_difference = INT_MAX;
        while (curr != NULL)
        {
            if ((curr->size >= request) && (curr->size - request < best_difference))
            {
                best = curr;
                best_prev = prev;
                best_difference = curr->size - request;
            }
            prev = curr;
            curr = curr->next;
        }
        if (best == NULL)
        {
            return (void*)-1;
        }
        chosen_hole = best;
        chosen_prev = best_prev;
    }

    if (malloc_type == WORST_FIT)
    {
        Hole_Node* curr = hole_list;
        Hole_Node* prev = NULL;
        Hole_Node* worst = NULL;
        Hole_Node* worst_prev = NULL;
        int worst_difference = -1;
        while (curr != NULL)
        {
            if ((curr->size >= request) && (curr->size - request > worst_difference))
            {
                worst = curr;
                worst_prev = prev;
                worst_difference = curr->size - request;
            }
            prev = curr;
            curr = curr->next;
        }
        if (worst == NULL)
        {
            return (void*)-1;
        }
        chosen_hole = worst;
        chosen_prev = worst_prev;
    }

    //if we made it here then chosen_hole is address to be used
    memcpy(chosen_hole->location, &size, sizeof(size));
    char* loc_to_return = chosen_hole->location+4;

    //remove or update chosen_hole
    if (chosen_hole->size == request)
    {
        if (chosen_prev == NULL) //it's the head
        {
            hole_list = chosen_hole->next;
            malloc(chosen_hole);
        }
        else
        {
            chosen_prev->next = chosen_hole->next;
            malloc(chosen_hole);
        }
    }
    else
    {
        chosen_hole->size -= (request);
        chosen_hole->location += (request);
    }
    return (void*)loc_to_return;

    return (void*)-1;
}

void my_free(void *ptr) {   
    //given pointer to where the allocation starts
    //must find if the previous space is a hole and if so merge
    //and check if the next space is a hole and if so, merge
    
    //start address of allocated memory
    char* alloc_start = ptr - sizeof(int);
    
    //get size of allocated space
    int size;
    memcpy(&size, alloc_start, sizeof(int));
    
    //find end of allocated space
    char* alloc_end = (char*)ptr + size;
    
    //initialize search node
    Hole_Node* search = hole_list;

	//check if first hole is contiguous with mem to to be freed
	if (search->location+search->size == alloc_start) {
		search->size += size + 4;
	}

	//else, find hole directly before memory to be freed
	else {
		while (search->next != NULL && search->next->location + search->next->size < alloc_start) {
			search = search->next;
		}

		//check if hole directly before memory to be freed is contiguous
		if (search->location + search->size == alloc_start) {
			search->size += size + 4;
		}

		//else, make a new hole
		else {
			Hole_Node* new_hole = malloc(sizeof(Hole_Node));
			new_hole->location = alloc_start;
			new_hole->size = size + 4;
			
			//check if hole_list points after memory to be freed
			if (hole_list->location > alloc_start) {
				//insert at front of hole list
				new_hole->next = hole_list;
				hole_list = new_hole;
			}
			
			//else, add into proper spot
			else {
				new_hole->next = search->next;
				search->next = new_hole;
			}
		}
	}
	
	//check if hole is contiguous with next hole
	if (search->next != NULL && (search->location + search->size == search->next->location)) {
		search->size += search->next->size;
		search->next = search->next->next;
	}
}

int num_free_bytes() {
    Hole_Node* curr = hole_list;
    int size = 0;
    while (curr != NULL)
    {
        size += curr->size;
        curr = curr->next;
    }
    return size;
}

int num_holes() {
    Hole_Node* curr = hole_list;
    int num = 0;
    while (curr != NULL)
    {
        curr = curr->next;
        num++;
    }
    return num;
}
