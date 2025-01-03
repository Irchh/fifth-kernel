#include "elf.h"
#include "printf.h"

Elf64_Shdr* elf_get_shent(const Elf64_Ehdr *elf_header, const Elf64_Half index) {
    if (elf_header->e_shoff == 0 || index >= elf_header->e_shnum) {
        return nullptr;
    }
    Elf64_Shdr* section_header_table = (void*)((size_t)elf_header + elf_header->e_shoff + index * elf_header->e_shentsize);
    return section_header_table;
}

Elf64_Phdr* elf_get_phent(const Elf64_Ehdr *elf_header, const Elf64_Half index) {
    if (elf_header->e_phoff == 0 || index >= elf_header->e_phnum) {
        return nullptr;
    }
    Elf64_Phdr* program_header_table = (void*)((size_t)elf_header + elf_header->e_phoff + index * elf_header->e_phentsize);
    return program_header_table;
}

char* elf_get_str(const Elf64_Ehdr *elf_header, const Elf64_Word index) {
    if (elf_header->e_shstrndx == SHN_UNDEF) {
        return nullptr;
    }
    return nullptr;
}

void* load_dynamic_elf(const Elf64_Ehdr* elf_header) {
    return nullptr;
}

void* load_exec_elf(const Elf64_Ehdr* elf_header) {
    const auto string_section = elf_get_shent(elf_header, elf_header->e_shstrndx);
    if (string_section == NULL) {
        return nullptr;
    }
    const auto string_table = (char*)((size_t)elf_header + string_section->sh_offset);
    printf("elf_header->e_shstrndx: %d\n", (int)elf_header->e_shstrndx);
    printf("elf_header->e_shoff: %#zx\n", elf_header->e_shoff);
    printf("elf_header->e_shentsize: %x\n", (int)elf_header->e_shentsize);
    printf("elf_header->e_ehsize: %x\n", (int)elf_header->e_ehsize);
    printf("string_table addr: %p\n", string_table);
    printf("string_table end: %p\n", string_table + string_section->sh_size);
    if (elf_header->e_shstrndx == SHN_UNDEF) {
        printf("NO STRINGS\n");
        return nullptr;
    }
    for (Elf64_Half i = 0; i < elf_header->e_shnum; i++) {
        const auto section_header = elf_get_shent(elf_header, i);
        if (section_header) {
            printf("Section name: %s\n", &string_table[section_header->sh_name]);
        }
    }
    uint64_t lowest_virt_addr = 0xFFFFFFFFFFFFFFFFL;
    uint64_t load_segments = 0;
    printf("Program headers:\n");
    printf("  Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align\n");
    for (Elf64_Half i = 0; i < elf_header->e_phnum; i++) {
        const auto program_header = elf_get_phent(elf_header, i);
        if (program_header) {
            switch (program_header->p_type) {
                case PT_NULL:
                    printf("  NULL\n");
                    continue;
                case PT_LOAD:
                    printf("  LOAD          ");
                    load_segments++;
                    if (program_header->p_vaddr < lowest_virt_addr) {
                        lowest_virt_addr = program_header->p_vaddr;
                    }
                    break;
                case PT_DYNAMIC:
                    printf("  DYNAMIC       ");
                    break;
                case PT_INTERP:
                    printf("  INTERP        ");
                    break;
                case PT_NOTE:
                    printf("  NOTE          ");
                    break;
                case PT_SHLIB:
                    printf("  SHLIB         ");
                    break;
                case PT_PHDR:
                    printf("  PHDR          ");
                    break;
                case PT_TLS:
                    printf("  TLS           ");
                    break;
                case PT_RISCV_ATTRIBUTES:
                    printf("  RISCV_ATTRIBUT");
                    break;
                default:
                    printf("  unknown (%#zx) ", program_header->p_type);
                    break;
            }
            char flags[4] = {' ', ' ', ' ', '\0'};
            if (program_header->p_flags & 0b100) flags[0] = 'R';
            if (program_header->p_flags & 0b010) flags[1] = 'W';
            if (program_header->p_flags & 0b001) flags[2] = 'E';
            printf(
                " 0x%06zx 0x%016zx 0x%016zx 0x%06zx 0x%06zx %s 0x%zx\n",
                program_header->p_offset,
                program_header->p_vaddr,
                program_header->p_paddr,
                program_header->p_filesz,
                program_header->p_memsz,
                flags,
                program_header->p_align
            );
        }
    }
    // TODO: load segments and keep track of loaded pages so those can be turned into a process when that is implemented
    return nullptr;
}

void *load_elf(const Elf64_Ehdr *elf_header) {
    if (elf_header->e_ident[EI_MAG0] != ELFMAG0
        || elf_header->e_ident[EI_MAG1] != ELFMAG1
        || elf_header->e_ident[EI_MAG2] != ELFMAG2
        || elf_header->e_ident[EI_MAG3] != ELFMAG3
    ) {
        printf("ELF Magic Mismatch\n");
        return nullptr;
    }
    if (elf_header->e_ident[EI_CLASS] != ELFCLASS64) {
        printf("Elf not 64-bit executable\n");
        return nullptr;
    }
    if (elf_header->e_ident[EI_DATA] != ELFDATA2LSB) {
        printf("Elf incompatible endianness\n");
        return nullptr;
    }
    if (elf_header->e_ident[EI_VERSION] != EV_CURRENT) {
        printf("ELF version mismatch\n");
        return nullptr;
    }
    if (elf_header->e_machine != EM_RISCV) {
        printf("ELF is not for RISC-V\n");
        return nullptr;
    }
    // Check for ET_REL when that is implemented
    if (elf_header->e_type == ET_EXEC) {
        printf("ELF is EXEC\n");
        return load_exec_elf(elf_header);
    }
    if (elf_header->e_type != ET_DYN) {
        printf("ELF is DYNAMIC\n");
        return load_dynamic_elf(elf_header);
    }
    printf("ELF file is not an executable\n");
    return nullptr;
}
