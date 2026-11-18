
#include "../ft_nm.h"
#include <elf.h>
#include <ctype.h>

char symbol_type(const t_file *file, const t_elf *elf,
                 const t_symbol *symbol)
{
    t_section   section;
    unsigned    bind;
    unsigned    kind;
    char        type;

    bind = ELF64_ST_BIND(symbol->info);
    kind = ELF64_ST_TYPE(symbol->info);

    if (bind == STB_WEAK)
    {
        if (kind == STT_OBJECT)
            return (symbol->shndx == SHN_UNDEF ? 'v' : 'V');
        return (symbol->shndx == SHN_UNDEF ? 'w' : 'W');
    }

    if (symbol->shndx == SHN_UNDEF)
        return ('U');
    if (symbol->shndx == SHN_COMMON)
        return ('C');

    if (symbol->shndx == SHN_ABS)
        type = 'a';
    else
    {
        if (!get_section(file, elf, symbol->shndx, &section))
            return ('?');

        if (section.type == SHT_NOBITS)
            type = 'b';
        else if (section.flags & SHF_EXECINSTR)
            type = 't';
        else if (section.flags & SHF_WRITE)
            type = 'd';
        else if (section.flags & SHF_ALLOC)
            type = 'r';
        else
            type = 'n';
    }

    if (bind == STB_GLOBAL)
        type = (char)toupper((unsigned char)type);

    return (type);
}



void print_symbols(const t_file *file, const t_elf *elf,
                   const t_symbol *symbols, size_t count)
{
    size_t i;
    int    width;
    char   type;

    width = (elf->elf_class == ELFCLASS64) ? 16 : 8;
    i = 0;
    while (i < count)
    {
        type = symbol_type(file, elf, &symbols[i]);

        if (symbols[i].shndx == SHN_UNDEF)
            printf("%*s %c %s\n", width, "", type, symbols[i].name);
        else
            printf("%0*" PRIx64 " %c %s\n",
                   width, symbols[i].value, type, symbols[i].name);
        i++;
    }
}
