#include "ustack.h"

struct buffer buffer_stack[MAX_BUFFERS];
int stack_ptr = -1; // empty stack

int get_ptr(){
    return stack_ptr;
}


struct header {
    uint len;
    uint page_flag;
    struct header* prev;
};

struct header *current = 0; // Pointer to the current header

static struct header *curr = 0;
static char* stack_pointer;

//static header *freep ;


void* ustack_malloc(uint len){
   
    char *p;
    struct header *new_header;

    
    if (len > MAX_BUFFER_SIZE){
        return (void*)-1; // Exceeded maximum allowed size
    }

    if(curr == 0){ //initialization
        p = sbrk(PGSIZE);
        if(p == (char*)-1)
            return (void*)-1;
        new_header = (struct header*)p;
    }
    else if (PGROUNDDOWN((uint64)(stack_pointer + len)) !=  (uint64)sbrk(0)) //need to alloc new page
    {
        p = sbrk(PGSIZE);
        if(p == (char*)-1)
            return (void*)-1;
        new_header = (struct header*)p;
        new_header->page_flag = 1;
    }
    else{
        new_header = (struct header*)(stack_pointer );
    }

    new_header->prev = curr;
    new_header->len = len;
    curr = new_header;
    stack_pointer =(char*) new_header + len;
    return (void*)stack_pointer;
}



int ustack_free(void){
    if (curr){
        char *p;
        int del_size = curr->len;
        curr = curr->prev;
        stack_pointer = stack_pointer - del_size;
        if (curr->page_flag == 1)
        {
            p = sbrk(-PGSIZE);
            if(p == (char*)-1)
                return -1;
        }
        return del_size; 
    }
    return -1;
}
