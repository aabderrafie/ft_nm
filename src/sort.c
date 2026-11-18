#include "../ft_nm.h"
#include <stdlib.h>
#include <string.h>

static int compare_symbols(const void *a, const void *b)
{
    const t_symbol *s1 = a;
    const t_symbol *s2 = b;

    return (strcoll(s1->name, s2->name));
}

void sort_symbols(t_symbol *symbols, size_t count)
{
    if (!symbols || count < 2)
        return;

    qsort(symbols, count, sizeof(t_symbol), compare_symbols);
}