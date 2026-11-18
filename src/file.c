
#include "../ft_nm.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdint.h>


static int file_error(const char *path, const char *message)
{
    fprintf(stderr, "ft_nm: %s: %s\n", path, message);
    return (1);
}


// map the file in mem
int open_file(const char *path, t_file *file)
{
    struct stat st;
    void        *data;
    int         fd;
    int         error;

    file->name = path;
    file->data = NULL;
    file->size = 0;

    fd = open(path, O_RDONLY);
    if (fd == -1)
        return (file_error(path, strerror(errno)));

    if (fstat(fd, &st) == -1)
    {
        error = errno;
        close(fd);
        return (file_error(path, strerror(errno)));
    }

    if (!S_ISREG(st.st_mode))
    {
        close(fd);
        return (file_error(path, "not a regular file"));
    }

    if (st.st_size <= 0 || (uintmax_t)st.st_size > SIZE_MAX)
    {   if (st.st_size <= 0 || (uintmax_t)st.st_size > SIZE_MAX)
        {
            close(fd);
            return (file_error(path, "invalid file size"));
        }
    }

    data = mmap(NULL, (size_t)st.st_size,PROT_READ, MAP_PRIVATE, fd, 0);
    error = errno;
    close(fd);

    if (data == MAP_FAILED)
        return (file_error(path, strerror(error)));

    file->data = data;
    file->size = (size_t)st.st_size;
    return (0);
}

/*
** Release mapped memory.
*/
void close_file(t_file *file)
{
    if (file->data != NULL)
        munmap((void *)file->data, file->size);

    file->data = NULL;
    file->size = 0;
}

/*
** Process a single file.
*/
int process_file(const char *path)
{
    t_file      file;
    t_elf       elf;
    t_section   symtab;
    t_section   strtab;
    t_symbol    *symbols;
    size_t      count;
    const char  *error;

    if (open_file(path, &file) != 0)
        return (1);

    error = NULL;
    if (!validate_elf(&file, &elf, &error))
    {
        close_file(&file);
        return (file_error(path, error));
    }

    if (!find_symtab(&file, &elf, &symtab, &strtab, &error))
    {
        close_file(&file);
        file_error(path, error);
        return (strcmp(error, "no symbols") != 0);
    }

    symbols = NULL;
    count = 0;
    if (!extract_symbols(&file, &elf, &symtab, &strtab,
                         &symbols, &count, &error))
    {
        close_file(&file);
        return (file_error(path, error));
    }

    sort_symbols(symbols, count);
    print_symbols(&file, &elf, symbols, count);

    free(symbols);
    close_file(&file);
    return (0);
}