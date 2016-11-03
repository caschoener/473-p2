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
