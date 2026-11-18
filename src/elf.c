
#include "../ft_nm.h"
#include <elf.h>
#include <string.h>


int get_section(const t_file *file, const t_elf *elf, size_t index, t_section *section)
{
    size_t      entry_size;
    size_t      offset;

    if (elf->elf_class == ELFCLASS64)
        entry_size = sizeof(Elf64_Shdr);
    else if (elf->elf_class == ELFCLASS32)
        entry_size = sizeof(Elf32_Shdr);
    else
        return (0);

    if (index >= elf->section_count || elf->section_entry_size != entry_size || elf->section_offset > file->size)
        return (0);

    if (index >= (file->size - elf->section_offset) / entry_size)
        return (0);

    offset = (size_t)elf->section_offset + index * entry_size;

    if (elf->elf_class == ELFCLASS64)
    {
        Elf64_Shdr h;

        memcpy(&h, file->data + offset, sizeof(h));
        section->type = h.sh_type;
        section->flags = h.sh_flags;
        section->offset = h.sh_offset;
        section->size = h.sh_size;
        section->entsize = h.sh_entsize;
        section->link = h.sh_link;
    }
    else
    {
        Elf32_Shdr h;

        memcpy(&h, file->data + offset, sizeof(h));
        section->type = h.sh_type;
        section->flags = h.sh_flags;
        section->offset = h.sh_offset;
        section->size = h.sh_size;
        section->entsize = h.sh_entsize;
        section->link = h.sh_link;
    }

    return (1);
}



static int validate_sections(const t_file *file, const t_elf *elf, const char **error)
{
    size_t expected_size;

    if (elf->elf_class == ELFCLASS64)
        expected_size = sizeof(Elf64_Shdr);
    else
        expected_size = sizeof(Elf32_Shdr);

    if (elf->section_entry_size != expected_size)
        return (*error = "invalid section entry size", 0);

    if (elf->section_count == 0)
        return (*error = "missing or extended section table", 0);

    if (elf->section_offset == 0 || elf->section_offset > file->size)
        return (*error = "invalid section table offset", 0);

    if (elf->section_count > (file->size - elf->section_offset) / expected_size)
        return (*error = "section table outside file", 0);

    if (elf->section_name_index == SHN_XINDEX)
        return (*error = "extended section name index unsupported", 0);

    if (elf->section_name_index != SHN_UNDEF  && elf->section_name_index >= elf->section_count)
        return (*error = "invalid section name index", 0);

    return (1);
}

static int validate_elf64(const t_file *file, t_elf *elf,const char **error)
{
    Elf64_Ehdr h;

    if (file->size < sizeof(h))
        return (*error = "truncated ELF64 header", 0);

    memcpy(&h, file->data, sizeof(h));

    if (h.e_machine != EM_X86_64)
        return (*error = "unsupported architecture", 0);

    if (h.e_type != ET_REL && h.e_type != ET_EXEC && h.e_type != ET_DYN)
        return (*error = "unsupported ELF type", 0);

    if (h.e_version != EV_CURRENT || h.e_ehsize != sizeof(h))
        return (*error = "invalid ELF64 header", 0);

    elf->section_offset = h.e_shoff;
    elf->section_count = h.e_shnum;
    elf->section_entry_size = h.e_shentsize;
    elf->section_name_index = h.e_shstrndx;

    return (1);
}

static int validate_elf32(const t_file *file, t_elf *elf, const char **error)
{
    Elf32_Ehdr h;

    if (file->size < sizeof(h))
        return (*error = "truncated ELF32 header", 0);

    memcpy(&h, file->data, sizeof(h));

    if (h.e_machine != EM_386)
        return (*error = "unsupported architecture", 0);

    if (h.e_type != ET_REL && h.e_type != ET_EXEC && h.e_type != ET_DYN)
        return (*error = "unsupported ELF type", 0);

    if (h.e_version != EV_CURRENT || h.e_ehsize != sizeof(h))
        return (*error = "invalid ELF32 header", 0);

    elf->section_offset = h.e_shoff;
    elf->section_count = h.e_shnum;
    elf->section_entry_size = h.e_shentsize;
    elf->section_name_index = h.e_shstrndx;

    return (1);
}

int validate_elf(const t_file *file, t_elf *elf,
                 const char **error)
{
    const unsigned char *id;

    if (file->size < EI_NIDENT)
        return (*error = "file too small", 0);

    id = file->data;

    if (memcmp(id, ELFMAG, SELFMAG) != 0) 
        return (*error = "not an ELF file", 0);

    if (id[EI_CLASS] != ELFCLASS32 && id[EI_CLASS] != ELFCLASS64)
        return (*error = "unsupported ELF class", 0);

    if (id[EI_DATA] != ELFDATA2LSB)
        return (*error = "unsupported endianness", 0);

    if (id[EI_VERSION] != EV_CURRENT)
        return (*error = "invalid ELF version", 0);

    elf->elf_class = id[EI_CLASS];

if (elf->elf_class == ELFCLASS64)
{
    if (!validate_elf64(file, elf, error)) return (0);
}
else
{
    if (!validate_elf32(file, elf, error)) return (0);
}

return (validate_sections(file, elf, error));
}
