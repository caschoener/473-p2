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
}

int num_free_bytes() {
    return 0;
}

int num_holes() {
    return 0;
}
