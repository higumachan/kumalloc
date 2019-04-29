#include <stdio.h>
#include <assert.h>
#include "kumalloc.h"
#include "kumalloc_test_utility.h"

int main()
{
    assert(size_manage_areas() == 0);

    int* p = kumalloc(sizeof(int));

    *p = 0xD;

    assert(manage_head_ptr != NULL);
    assert(manage_head_ptr->size == sizeof(int));
    assert(*(int *)(heap + sizeof(MEMORY_MANAGE_AREA)) == 0xD);
    assert(size_manage_areas() == 2);

    {
        int *q = kumalloc(sizeof(int) * 10);
        assert(manage_head_ptr != NULL);
        assert(manage_head_ptr->size == sizeof(int));
        assert(manage_head_ptr->next->size == sizeof(int) * 10);
        assert(manage_head_ptr->next->flag == USE);
        assert(manage_head_ptr->next->next->flag == NOUSE);

        kufree(q);
        assert(manage_head_ptr->next->flag == NOUSE);
    }

    int*q = kumalloc(sizeof(int) * 5);

    assert(manage_head_ptr->next->size == sizeof(int) * 5);
    assert(manage_head_ptr->next->flag == USE);
    assert(manage_head_ptr->next->next->size == sizeof(heap) - sizeof(MEMORY_MANAGE_AREA) * 3 - sizeof(int) - sizeof(int) * 5);
    assert(manage_head_ptr->next->next->flag == NOUSE);

    kufree(q);
    kufree(p);

    assert(size_manage_areas() == 1);
    assert(manage_head_ptr->size == sizeof(heap) - sizeof(MEMORY_MANAGE_AREA));

    return 0;
}