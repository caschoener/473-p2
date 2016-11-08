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
#include <math.h>
#include "memalloc.h"

typedef struct Hole_Node_
{
    int size;
    char* location;
    struct Hole_Node_* next;
} Hole_Node;

typedef struct Buddy_Node_ {
	int isAllocated;
	int size;
	void* location;
	struct Buddy_Node_* left;
	struct Buddy_Node_* right;
} Buddy_Node;

void setup(int, int, void*);
void *my_malloc(int);
void my_free(void*);
int num_free_bytes();
int num_holes();
void *buddy_allocate(int, Buddy_Node*);
void split_node(Buddy_Node*);
int DFS_size(Buddy_Node*);
int DFS_num(Buddy_Node*);




//global vars for memory space information
int memory_size;
int malloc_type;
char* memory_start;

//linked list of holes, initially one node for entire space
Hole_Node* hole_list;

//root of buddy system tree
Buddy_Node* buddy_root;

void *buddy_allocate(int size, Buddy_Node* node) {
	void* return_location = (void*)-1;
	
	//check if this node is allocated
	if (node->isAllocated == 1) {
		return (void*)-1;
	}
	
	//else, node is not allocated, check if this node is the correct size
	else if (node->size == size) {
		//check if node is split
		if (node->left==NULL) {
			//allocate here
			node->isAllocated = 1;
			return (void*)node->location;
		}
		
		//else, node is split
		else {
			return (void*)-1;
		}
	}
	
	//else, node is too big, check if already split
	else if (node->left!=NULL) {
		//check left node
		return_location = buddy_allocate(size, node->left);
		if (return_location == (void*)-1) {
			//check right node
			return_location = buddy_allocate(size, node->right);
		} //at this point, either left location, right location, or -1 is in return_location
		return return_location;
	}
	
	//else, split this node
	else {
		split_node(node);
		
		//try this node again
		return buddy_allocate(size, node);
	}
    return (void*)-1;
}

void split_node(Buddy_Node* node) {
	int childSize = (node->size)/2;
	
	Buddy_Node* new_left = malloc(sizeof(Buddy_Node));
	new_left->isAllocated = 0;
	new_left->size = childSize;
	new_left->location = node->location;
	
	Buddy_Node* new_right = malloc(sizeof(Buddy_Node));
	new_right->isAllocated = 0;
	new_right->size = childSize;
	new_right->location = node->location + childSize;
	
	node->left = new_left;
	node->right = new_right;	
}

void setup( int malloc_type_, int mem_size, void* start_of_memory ) {
    malloc_type = malloc_type_;
    memory_size = mem_size;
    memory_start = start_of_memory;

    Hole_Node* starter_node = malloc(sizeof(Hole_Node));
    starter_node->size = memory_size;
    starter_node->location = memory_start;
    hole_list = starter_node;
	
	Buddy_Node* root_init = malloc(sizeof(Buddy_Node));
	root_init->isAllocated = 0;
	root_init->size = memory_size;
	root_init->location = memory_start;
	root_init->left = NULL;
	root_init->right = NULL;
	buddy_root = root_init;
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

	if (malloc_type == BUDDY_SYSTEM) {
		int size_of_block = 2;
		
		//get minimum buddy block size
		while (size_of_block < request) {
			size_of_block *= 2;
		}
		
		//get return location
		void *loc_to_return;
		loc_to_return = buddy_allocate(size_of_block, buddy_root);
		
		if (loc_to_return != (void*)-1) {
			memcpy(loc_to_return, &size_of_block, sizeof(size_of_block));
			loc_to_return += sizeof(int);
			return loc_to_return;
		}
		else {
			return (void*)-1;
		}
		
	}
    //this code is identical for first, worst, best fit
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
int DFS_buddy_free(void *ptr, Buddy_Node* curr) {
    if (!curr)
        return 0;
    if (curr->left==NULL && (curr->location + 4 == ptr))
    {
        curr->isAllocated = 0;
        return 1;
    }
    if (DFS_buddy_free(ptr, curr->left) || DFS_buddy_free(ptr, curr->right))
    {
        if (curr->left->left==NULL && curr->right->left==NULL && curr->left->isAllocated == 0 && curr->right->isAllocated == 0)
        {
            free(curr->left);
            free(curr->right);
            curr->left = NULL;
            curr->right = NULL;
        }

        return 1;
    }
    return 0;
}

void my_free(void *ptr) {
    if (malloc_type == BUDDY_SYSTEM)
        DFS_buddy_free(ptr, buddy_root);

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
int DFS_size(Buddy_Node* curr){ 
    if (!curr) 
        return 0; 
    if (curr->isAllocated)
        return 0;
    if (curr->left!=NULL)
        return DFS_size(curr->left) + DFS_size(curr->right); 
    return curr->size;
} 
int num_free_bytes() { 
    if (malloc_type == BUDDY_SYSTEM)
    { 
        return DFS_size(buddy_root); 
    } 
    Hole_Node* curr = hole_list;
    int size = 0;
    while (curr != NULL)
    {
        size += curr->size;
        curr = curr->next;
    }
    return size;
}
int DFS_num(Buddy_Node* curr){ 
    if (!curr) 
        return 0; 
    if (curr->isAllocated)
        return 0;
    if (curr->left==NULL)
        return 1;
    return DFS_num(curr->left) + DFS_num(curr->right);
} 
int num_holes() {
    if (malloc_type == BUDDY_SYSTEM)
    {
        return DFS_num(buddy_root);
    }
    Hole_Node* curr = hole_list;
    int num = 0;
    while (curr != NULL)
    {
        curr = curr->next;
        num++;
    }
    return num;
}


