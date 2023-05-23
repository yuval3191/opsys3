#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

#define MAX_BUFFERS 1000 // or whatever maximum you see fit
#define MAX_BUFFER_SIZE 512
#define PAGE_SIZE 4096

struct buffer {
    void *start;
    uint len;
};

int get_ptr();
void* ustack_malloc(uint len);
int ustack_free();