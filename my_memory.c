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
    if (malloc_type = BEST_FIT){
        Hole_Node* curr = hole_list;
        Hole_Node* prev = NULL;
        while (curr->size <= size+4)
        {
            prev = curr;
            curr = curr->next;
            if (curr == NULL) // end of list, no holes were big enough
            {
                return (void*)-1;
            }
        }
        //this hole is big enough so add header and store location to return
        memcpy(curr->location, &size, sizeof(size));
        char* loc_to_return = curr->location+4;

        //remove or update curr
        if (curr->size == size+4)
        {
            if (prev = NULL) //it's the head
            {
                hole_list = curr->next;
                malloc(curr);
            }
            else
            {
                prev->next = curr->next;
                malloc(curr);
            }
        }
        else
        {
            curr->size -= (size+4);
            curr->location += (size+4);
        }
        return (void*)loc_to_return;
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
    int size;
    memcpy(&size, alloc_start, sizeof(int));
    
    //find end of allocated space
    char* alloc_end = (char*)ptr + size;
    
    //initialize search node
    Hole_Node* search = hole_list;
    
    //check if first hole is contiguous with memory to be freed
    if (((search->location)+(search->size)) == alloc_start) {
        //add memory to be freed to first hole
        search->size += size + 4;
    }
    
    //otherwise, search for hole directly before the memory to be freed
    else {

        //iterate through LL and stop if location+size >= alloc_start
        while ((search->next != NULL) && (((search->next->location)+(search->next->size)) < alloc_start)) {
            search = search->next;
        }

        //check if the previous hole is contiguous with the space to free
        if ((search->next != NULL) && (((search->next->location)+(search->next->size)) == alloc_start)) {
            //merge these holes
            search->next->size += size + 4;
		} 
		//otherwise, make a new hole
        else {
	    	Hole_Node* new_hole = malloc(sizeof(Hole_Node));
			new_hole->location = alloc_start;
			new_hole->size = size + 4;
			new_hole->next = search->next;
			search->next = new_hole;
		}
    }

    //search->next should be at the hole before the memory to be freed
    //so, advance search once
    if (search->next != NULL) {
		search = search->next;
    }
    
    //then, check if the end of that hole is the beginning of the next hole
    if ((search->next != NULL) && (((search->location)+(search->size)) == (search->next->location))) {
        //merge these holes
        search->size += search->next->size;
        
        //update linked list
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
