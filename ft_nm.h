
#ifndef FT_NM_H
# define FT_NM_H

# include <stddef.h>
# include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
typedef struct s_file
{
    const char      *name;
    const uint8_t   *data;
    size_t          size;
}   t_file;

typedef struct s_elf
{
    unsigned char   elf_class;
    uint64_t        section_offset;
    uint64_t        section_count;
    uint64_t        section_name_index;
    uint16_t        section_entry_size;
}   t_elf;

typedef struct s_section
{
    uint32_t type;
    uint64_t flags;
    uint64_t offset;
    uint64_t size;
    uint64_t entsize;
    uint32_t link;
}   t_section;

typedef struct s_symbol
{
    const char  *name;
    uint64_t    value;
    uint64_t    size;
    uint8_t     info;
    uint8_t     other;
    uint16_t    shndx;
}   t_symbol;

typedef struct s_options
{
    int a;
    int g;
    int u;
    int r;
    int p;
}   t_options;

int     open_file(const char *path, t_file *file);
void    close_file(t_file *file);
int validate_elf(const t_file *file, t_elf *elf,const char **error);
char symbol_type(const t_file *file, const t_elf *elf,const t_symbol *symbol);
int find_symtab(const t_file *file, const t_elf *elf, t_section *symtab, t_section *strtab,const char **error);
int get_symbol(const t_file *file, const t_elf *elf, const t_section *symtab, const t_section *strtab,size_t index, t_symbol *symbol);

int  get_section(const t_file *file, const t_elf *elf,
                 size_t index, t_section *section);

int  extract_symbols(const t_file *file, const t_elf *elf,
                     const t_section *symtab, const t_section *strtab,
                     t_symbol **symbols, size_t *count,
                     const char **error);

void sort_symbols(t_symbol *symbols, size_t count);

char symbol_type(const t_file *file, const t_elf *elf,
                 const t_symbol *symbol);

void print_symbols(const t_file *file, const t_elf *elf,
                   const t_symbol *symbols, size_t count);
void sort_symbols(t_symbol *symbols, size_t count);
int process_file(const char *path);
#endif
