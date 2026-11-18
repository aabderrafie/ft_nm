
#include "../ft_nm.h"
#include <elf.h>

int find_symtab(const t_file *file, const t_elf *elf,t_section *symtab, t_section *strtab, const char **error)
{
    t_section section;
    size_t    i;
    size_t    entry_size;

    i = 0;
    while (i < elf->section_count)
    {
        if (!get_section(file, elf, i, &section))
            return (*error = "invalid section header", 0);

        if (section.type == SHT_SYMTAB)
        {
            if (section.link >= elf->section_count)
                return (*error = "invalid string table index", 0);

            if (!get_section(file, elf, section.link, strtab))
                return (*error = "invalid string table header", 0);

            if (strtab->type != SHT_STRTAB)
                return (*error = "invalid string table type", 0);

            if (elf->elf_class == ELFCLASS64)
                entry_size = sizeof(Elf64_Sym);
            else
                entry_size = sizeof(Elf32_Sym);

            if (section.entsize != entry_size || section.size % entry_size != 0)
                return (*error = "invalid symbol entry size", 0);

            if (section.offset > file->size
                || section.size > file->size - section.offset)
                return (*error = "invalid symbol table bounds", 0);

            if (strtab->offset > file->size || strtab->size > file->size - strtab->offset)
                return (*error = "invalid string table bounds", 0);

            *symtab = section;
            return (1);
        }
        i++;
    }
    return (*error = "no symbols", 0);
}


int get_symbol(const t_file *file, const t_elf *elf,
               const t_section *symtab, const t_section *strtab,
               size_t index, t_symbol *symbol)
{
    size_t      offset;
    size_t      count;
    uint32_t    name_offset;

    if (symtab->entsize == 0)
        return (0);

    count = symtab->size / symtab->entsize;
    if (index >= count)
        return (0);

    // find_symtab() has already validated both table ranges.
    offset = symtab->offset + index * symtab->entsize;

    if (elf->elf_class == ELFCLASS64)
    {
        Elf64_Sym s;

        memcpy(&s, file->data + offset, sizeof(s));
        name_offset = s.st_name;
        symbol->value = s.st_value;
        symbol->size = s.st_size;
        symbol->info = s.st_info;
        symbol->other = s.st_other;
        symbol->shndx = s.st_shndx;
    }
    else if (elf->elf_class == ELFCLASS32)
    {
        Elf32_Sym s;

        memcpy(&s, file->data + offset, sizeof(s));
        name_offset = s.st_name;
        symbol->value = s.st_value;
        symbol->size = s.st_size;
        symbol->info = s.st_info;
        symbol->other = s.st_other;
        symbol->shndx = s.st_shndx;
    }
    else
        return (0);

    if (name_offset >= strtab->size)
        return (0);

    symbol->name = (const char *)file->data
        + strtab->offset + name_offset;

    if (!memchr(symbol->name, '\0', strtab->size - name_offset))
        return (0);

    return (1);
}




int extract_symbols(const t_file *file, const t_elf *elf, const t_section *symtab, const t_section *strtab, t_symbol **symbols, size_t *count,const char **error)
{
    size_t      total;
    size_t      i;
    size_t      n;
    t_symbol    *list;
    t_symbol    symbol;

    if (symtab->entsize == 0)
        return (*error = "invalid symbol entry size", 0);

    total = symtab->size / symtab->entsize;
    if (total > SIZE_MAX / sizeof(t_symbol))
        return (*error = "too many symbols", 0);

    list = malloc(total * sizeof(t_symbol));
    if (!list && total != 0)
        return (*error = "memory allocation failed", 0);

    i = 0;
    n = 0;
    while (i < total)
    {
        if (!get_symbol(file, elf, symtab, strtab, i, &symbol))
        {
            free(list);
            return (*error = "invalid symbol entry", 0);
        }

        if (symbol.name[0] != '\0'
            && ELF64_ST_TYPE(symbol.info) != STT_FILE
            && ELF64_ST_TYPE(symbol.info) != STT_SECTION)
        {
            list[n] = symbol;
            n++;
        }
        i++;
    }

    *symbols = list;
    *count = n;
    return (1);
}
