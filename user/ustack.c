#include "ustack.h"

struct header {
    uint len; // Length of the buffer
    uint dealloc_page; // Page to deallocate when buffer is freed
    struct header* prev; // Pointer to the previous header
};

struct header *current = 0; // Pointer to the current buffer's header


void *ustack_malloc(uint len) {
    // Check if requested buffer size is within limit
    if(len > 512) {
        return (void *)-1;
    }

    // Add the size of the header to the length
    uint total_len = len + sizeof(struct header);

    // Round up to nearest page size if necessary
    if (total_len % PGSIZE != 0) {
        total_len = (total_len + PGSIZE - 1) & ~(PGSIZE - 1);
    }

    // Try to increase the program break to create space for the new buffer
    // If this fails, return -1
    if(sbrk(total_len) == (void *)-1) {
        return (void *)-1;
    }

    // Get the current program break and cast it to the header struct pointer
    struct header *new_header = (struct header *) (sbrk(0) - total_len);

    // Check if there's a previously allocated buffer, if yes then link it
    if(current) {
        new_header->prev = current;
    } else {
        new_header->prev = 0; // If not, this is the first allocated buffer
    }

    // Set the length of the buffer
    new_header->len = len;

    // Set the dealloc_page value
    new_header->dealloc_page = (uint)sbrk(0);

    // Set the current buffer to the new buffer
    current = new_header;

    // Return a pointer to the start of the buffer
    return (void *)(new_header + 1);
}


int ustack_free() {
    // Check if there's a buffer to free, if not return -1
    if(current == 0) {
        return -1;
    }

    // Store the length of the buffer to return later
    int len = current->len;

    // Check if we need to deallocate a page
    if((uint)sbrk(0) > current->dealloc_page) {
        // Decrease the program break by a page size, effectively deallocating the page
        sbrk(-PGSIZE);
    }

    // Decrease the program break to the start of the previous buffer, effectively deallocating the buffer
    sbrk((uint)current->prev - (uint)sbrk(0));

    // Set the current buffer to the previous buffer
    current = current->prev;

    // Return the length of the deallocated buffer
    return len;
}
