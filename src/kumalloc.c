//
// Created by 日熊悠太 on 2019-04-28.
//

#include <stdio.h>
#include <stddef.h>
#include "kumalloc.h"
#include "kumalloc_test_utility.h"

typedef unsigned char BYTE;
BYTE heap[HEAP_SIZE];
MEMORY_MANAGE_AREA* manage_head_ptr = NULL;

static void merge_manage_free_areas(MEMORY_MANAGE_AREA *this, const MEMORY_MANAGE_AREA *next);
void* mymemcpy(BYTE* dest, BYTE* src, size_t size);

void* kumalloc(size_t size)
{
    size *= 16;
    if (manage_head_ptr == NULL) {
        manage_head_ptr = (MEMORY_MANAGE_AREA *)heap;
        manage_head_ptr->flag = NOUSE;
        manage_head_ptr->prev = manage_head_ptr->next = NULL;
        manage_head_ptr->size = HEAP_SIZE - sizeof(MEMORY_MANAGE_AREA);
    }

    MEMORY_MANAGE_AREA* p = manage_head_ptr;

    while (p != NULL) {
        if (p->flag == NOUSE && p->size >= size) {
            break;
        }
        p = p->next;
    }

    if (p == NULL) {
        return NULL;
    }

    int remain_size = p->size - size;
    if (remain_size >= sizeof(MEMORY_MANAGE_AREA) + 1) {
        MEMORY_MANAGE_AREA* q = (MEMORY_MANAGE_AREA *)((unsigned char *)(p + 1) + size);
        MEMORY_MANAGE_AREA* temp_next = p->next;
        q->prev = p;
        q->size = remain_size - sizeof(MEMORY_MANAGE_AREA);
        q->flag = NOUSE;
        q->next = temp_next;
        p->next = q;
        p->size = size;
    }
    p->flag = USE;

    printf("%d %d\n", (BYTE *)(p + 1) - heap, p + 1);
    return (void *)(p + 1);
}

void kufree(void* p)
{
    MEMORY_MANAGE_AREA* manage_area = get_manage_area_ptr(p);

    manage_area->flag = NOUSE;

    /*
    if (manage_area->next != NULL) {
        MEMORY_MANAGE_AREA* next = manage_area->next;
        if (next->flag == NOUSE) {
            merge_manage_free_areas(manage_area, next);
        }
    }

    if (manage_area->prev != NULL) {
        MEMORY_MANAGE_AREA *prev = manage_area->prev;
        if (prev->flag == NOUSE) {
            merge_manage_free_areas(prev, manage_area);
        }
    }
     */
}


void* kurealloc(void *p, size_t size)
{
    if (p == NULL) {
        return kumalloc(size);
    }

    MEMORY_MANAGE_AREA* manage_area = get_manage_area_ptr(p);
    BYTE *new_allocated_ptr;
    if (manage_area->next != NULL && manage_area->next->flag == NOUSE && manage_area->size + manage_area->next->size >= size + 1) {
        manage_area->size = size;
        MEMORY_MANAGE_AREA* new_manage_area = (MEMORY_MANAGE_AREA *)((BYTE *)p + size);
        new_manage_area->size = manage_area->size + manage_area->next->size - size;
        new_manage_area->flag = NOUSE;
        new_manage_area->next = manage_area->next->next;
        new_manage_area->prev = manage_area;
        manage_area->next = new_manage_area;
        new_allocated_ptr = p;
    }
    else {
        new_allocated_ptr = (BYTE *) kumalloc(size);
        mymemcpy(new_allocated_ptr, (BYTE *) p, get_manage_area_ptr(p)->size);

        kufree(p);
    }
    return new_allocated_ptr;
}


int size_manage_areas()
{
    if (manage_head_ptr == NULL) {
        return 0;
    }

    int num = 0;
    for (MEMORY_MANAGE_AREA* p = manage_head_ptr; p != NULL; p = p->next) {
        num++;
    }
    return num;
}

static void merge_manage_free_areas(MEMORY_MANAGE_AREA *this, const MEMORY_MANAGE_AREA *next)
{
    this->next = next->next;
    this->size += next->size + sizeof(MEMORY_MANAGE_AREA);
    if (this->next != NULL) {
        this->next->prev = this;
    }
}

void* mymemcpy(BYTE* dest, BYTE* src, size_t size)
{
    for (int i = 0; i < size; i++) {
        *dest++ = *src++;
    }

    return dest;
}


MEMORY_MANAGE_AREA* get_manage_area_ptr(void* p)
{
    return ((MEMORY_MANAGE_AREA *)p) - 1;
}
