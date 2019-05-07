#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include "kumalloc.h"
#include "kumalloc_test_utility.h"

int main()
{
    printf("%lu\n", sizeof(MEMORY_MANAGE_AREA));
    assert(sizeof(MEMORY_MANAGE_AREA) == 32);
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

    int* q = kumalloc(sizeof(int) * 5);

    assert(manage_head_ptr->next->size == sizeof(int) * 5);
    assert(manage_head_ptr->next->flag == USE);
    assert(manage_head_ptr->next->next->size == sizeof(heap) - sizeof(MEMORY_MANAGE_AREA) * 3 - sizeof(int) - sizeof(int) * 5);
    assert(manage_head_ptr->next->next->flag == NOUSE);

    kufree(q);
    kufree(p);

    assert(size_manage_areas() == 1);
    assert(manage_head_ptr->size == sizeof(heap) - sizeof(MEMORY_MANAGE_AREA));

    {
        int* qs[5];

        for (int i = 0; i < 5; i++) {
            qs[i] = kumalloc(sizeof(int));
        }
        assert(size_manage_areas() == 6);
        kufree(qs[1]);
        kufree(qs[3]);
        assert(size_manage_areas() == 6);
        kufree(qs[2]);
        assert(size_manage_areas() == 4);
        kufree(qs[0]);
        kufree(qs[4]);
    }

    {
        int* qs[5];

        for (int i = 0; i < 5; i++) {
            qs[i] = kumalloc(sizeof(int));
        }
        assert(size_manage_areas() == 6);
        kufree(qs[3]);
        assert(size_manage_areas() == 6);
        int* r;
        r = kumalloc(sizeof(int));
        assert(size_manage_areas() == 6);
        kufree(r);
        kufree(qs[0]);
        kufree(qs[1]);
        kufree(qs[2]);
        kufree(qs[4]);
    }
    {
        assert(size_manage_areas() == 1);
        char *buf = kumalloc(sizeof(MEMORY_MANAGE_AREA) + 2);
        assert(size_manage_areas() == 2);
        int *sentinel = kumalloc(sizeof(int));
        assert(size_manage_areas() == 3);
        kufree(buf);
        assert(size_manage_areas() == 3);
        char *mini_buf = kumalloc(1);
        assert(size_manage_areas() == 4);
        assert(manage_head_ptr->flag == USE);
        assert(manage_head_ptr->size == 1);
        assert(manage_head_ptr->next->flag == NOUSE);
        assert(manage_head_ptr->next->size == 1);
        kufree(mini_buf);
        kufree(sentinel);
    }

    {
        assert(size_manage_areas() == 1);
        char *buf = kumalloc(sizeof(MEMORY_MANAGE_AREA) + 1);
        assert(size_manage_areas() == 2);
        int *sentinel = kumalloc(sizeof(int));
        assert(size_manage_areas() == 3);
        kufree(buf);
        assert(size_manage_areas() == 3);
        char *mini_buf = kumalloc(1);
        assert(size_manage_areas() == 3);
        assert(manage_head_ptr->flag == USE);
        assert(manage_head_ptr->size == sizeof(MEMORY_MANAGE_AREA) + 1);
        assert(manage_head_ptr->next->flag == USE);
        assert(manage_head_ptr->next->size == 4);
        kufree(sentinel);
        kufree(mini_buf);
    }

    {
        assert(size_manage_areas() == 1);
        unsigned char *buf = kumalloc(sizeof(int) * 5);
        assert(get_manage_area_ptr(buf)->size == sizeof(int) * 5);
        int *sentinel = kumalloc(sizeof(int));
        memset(buf, -1, sizeof(int) * 5);
        unsigned char *new_buf = kurealloc(buf, sizeof(int) * 10);
        assert(get_manage_area_ptr(new_buf)->size == sizeof(int) * 10);

        assert(memcmp(buf, new_buf, sizeof(int) * 5) == 0);
        assert(memcmp(buf, new_buf, sizeof(int) * 10) != 0);
        kufree(new_buf);
        kufree(sentinel);
        assert(size_manage_areas() == 1);
    }

    {
        assert(size_manage_areas() == 1);
        unsigned char *buf = kumalloc(sizeof(int) * 5);
        assert(get_manage_area_ptr(buf)->size == sizeof(int) * 5);
        assert(size_manage_areas() == 2);
        unsigned char *new_buf = kurealloc(buf, sizeof(int) * 10);
        assert(get_manage_area_ptr(new_buf)->size == sizeof(int) * 10);
        assert(size_manage_areas() == 2);
        assert(buf == new_buf);

        kufree(new_buf);
        assert(size_manage_areas() == 1);
    }

    return 0;
}