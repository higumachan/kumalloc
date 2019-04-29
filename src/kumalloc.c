//
// Created by 日熊悠太 on 2019-04-28.
//

#include "kumalloc.h"
#include "kumalloc_test_utility.h"

unsigned char heap[HEAP_SIZE];
MEMORY_MANAGE_AREA* manage_head_ptr = NULL;

void* kumalloc(size_t size)
{
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

    size_t remain_size = p->size - size;
    if (remain_size - sizeof(MEMORY_MANAGE_AREA) >= 1) {
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

    return (void *)(p + 1);
}

void free(void* p)
{
    MEMORY_MANAGE_AREA* manage_area = ((MEMORY_MANAGE_AREA *)p) - 1;

    manage_area->flag = NOUSE;

    if (manage_area->next != NULL) {
        MEMORY_MANAGE_AREA* next = manage_area->next;
        if (next->flag == NOUSE) {
            manage_area->next = next->next;
            manage_area->size += next->size + sizeof(MEMORY_MANAGE_AREA);
            if (manage_area->next != NULL) {
                manage_area->next->prev = manage_area;
            }
        }
    }

    if (manage_area->prev != NULL) {
        MEMORY_MANAGE_AREA *prev = manage_area->prev;
        if (prev->flag == NOUSE) {
            prev->next = manage_area->next;
            prev->size += manage_area->size + sizeof(MEMORY_MANAGE_AREA);
            if (prev->next != NULL) {
                prev->next->prev = prev;
            }
        }
    }
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
