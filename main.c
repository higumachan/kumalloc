#include <stdio.h>
#include <assert.h>
#include "kumalloc.h"

int main()
{
    int* p = kumalloc(sizeof(int));

    *p = 0xD;

    assert(manage_head_ptr != NULL);
    assert(manage_head_ptr->size == sizeof(int));
    assert(*(int *)(heap + sizeof(MEMORY_MANAGE_AREA)) == 0xD);

    {
        int *q = kumalloc(sizeof(int) * 10);
        assert(manage_head_ptr != NULL);
        assert(manage_head_ptr->size == sizeof(int));
        assert(manage_head_ptr->next->size == sizeof(int) * 10);
        assert(manage_head_ptr->next->flag == USE);
        assert(manage_head_ptr->next->next->flag == NOUSE);

        free(q);
        assert(manage_head_ptr->next->flag == NOUSE);
    }

    int*q = kumalloc(sizeof(int) * 5);

    assert(manage_head_ptr->next->size == sizeof(int) * 5);
    assert(manage_head_ptr->next->flag == USE);
    assert(manage_head_ptr->next->next->size == sizeof(int) * 5 - sizeof(MEMORY_MANAGE_AREA));
    assert(manage_head_ptr->next->next->flag == NOUSE);

    free(q);
    free(p);

    assert(manage_head_ptr->size == sizeof(heap - sizeof(MEMORY_MANAGE_AREA)));

    return 0;
}