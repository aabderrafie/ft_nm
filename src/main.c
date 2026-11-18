
#include "../ft_nm.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    int i;
    int status;
setlocale(LC_COLLATE, "");
    if (argc == 1)
        return (process_file("a.out"));

    i = 1;
    status = 0;
    while (i < argc)
    {
        if (argc > 2)
            printf("\n%s:\n", argv[i]);

        if (process_file(argv[i]) != 0)
            status = 1;
        i++;
    }
    return (status);
}
