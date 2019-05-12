//
// Created by 日熊悠太 on 2019-04-28.
//

#ifndef KUMALLOC_KUMALLOC_H
#define KUMALLOC_KUMALLOC_H


#include <stdlib.h>

static const int HEAP_SIZE = 10000000;
static const char NOUSE = 0;
static const char USE = 1;

typedef struct MemoryManageArea
{
    char flag;
    size_t size;
    struct MemoryManageArea* next;
    struct MemoryManageArea* prev;
} MEMORY_MANAGE_AREA;

void* kumalloc(size_t size);
void* kurealloc(void *p, size_t size);
void kufree(void* p);

extern MEMORY_MANAGE_AREA* manage_head_ptr;
extern unsigned char heap[HEAP_SIZE];

#endif //KUMALLOC_KUMALLOC_H
