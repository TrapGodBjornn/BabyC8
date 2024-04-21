#define _XOPEN_SOURCE 500 // needed for sbrk() on cslab
#include <stddef.h>
#include <unistd.h>

typedef struct freenode
{
    size_t size;
    struct freenode *next;
} freenode;

#define HEAP_CHUNK_SIZE 4096

freenode *freelist = NULL;

void *malloc(size_t size)
{
	if (size < 1)
    {
		return NULL;
    }

	size += 8;

	if (size < 32)
    {
		size = 32;
    }

	else if (size%16 != 0)
    {
		size = ((size/16)+1)*16;
    }

	freenode *prev_node = NULL;
	freenode *current_node = freelist;

	while (current_node != NULL) {
		if (current_node->size >= size) {
			break;
		}
		prev_node = current_node;
		current_node = current_node->next;
	}
	if (current_node == NULL){
		current_node= (freenode *)sbrk(HEAP_CHUNK_SIZE);
		if (current_node == (void *)-1){
			return NULL;
		}
		current_node->size = HEAP_CHUNK_SIZE;
		current_node->next= NULL;
	}
	
	size_t remaining_size= current_node->size - size;
	void *return_address= (char *) current_node + 8;

	if (remaining_size< 32){
		if (prev_node) {
			prev_node->next= current_node->next;
		} else {
			freelist= current_node -> next;
		}
		}
	else{ 
		freenode *new_node= (freenode *)((char *)current_node + size);
		new_node->size= remaining_size;
		new_node->next= current_node->next;

		if (prev_node) {
			prev_node->next= new_node;
		} else {
			freelist= new_node;
		}
	}
	
	*((size_t *)current_node)= size - 8;

	return return_address;
	}
   



void free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    freenode *new_node = (freenode *)(ptr-8);

   
    new_node->next = freelist;
    freelist = new_node;

    return;
}
