#include "program_table.h"

void Phdr64(Elf64_Phdr pdhr)
{
    printf("[-------]\n");
    p_type(pdhr.p_type);
    p_flag(pdhr.p_flags);
    printf("OF[%p]\n", pdhr.p_offset);
    printf("VA[%p]\n", pdhr.p_vaddr);
    printf("PA[%p]\n", pdhr.p_paddr);
    printf("FS[%p]\n", pdhr.p_filesz);
    printf("MS[%p]\n", pdhr.p_memsz);
    printf("AI[%p]\n", pdhr.p_align);
    printf("\n");
}
/**
 * 打印程序头类型 
 */
void p_type( Elf64_Word type )
{

    switch (type) {
    case PT_NULL:
        printf("PT_NULL\n");
        break;
    case PT_LOAD:
        printf("PT_LOAD\n");
        break;
    case PT_DYNAMIC:
        printf("PT_DYNAMIC\n");
        break;
    case PT_INTERP:
        printf("PT_INTERP\n");
        break;
    case PT_NOTE:
        printf("PT_NOTE\n");
        break;
    case PT_SHLIB:
        printf("PT_SHLIB\n");
        break;
    case PT_PHDR:
        printf("PT_PHDR\n");
        break;
    case PT_TLS:
        printf("PT_TLS\n");
        break;
    case PT_LOOS:
        printf("PT_LOOS\n");
        break;
    case PT_HIOS:
        printf("PT_HIOS\n");
        break;
    case PT_LOPROC:
        printf("PT_LOPROC\n");
        break;
    case PT_HIPROC:
        printf("PT_HIPROC\n");
        break;
    case PT_GNU_EH_FRAME:
        printf("PT_GNU_EH_FRAME\n");
        break;
    case PT_GNU_STACK:
        printf("PT_GNU_STACK\n");
        break;
    default:
        printf("0x%p\n",type);
    }
}

/**
 * 打印程序头flag
 */
void p_flag(Elf64_Word flag)
{
    printf("[");
    if (flag &  PF_R ){
        printf("R");
    }
    if (flag & PF_W) {
        printf("W");
    }
    if (flag & PF_X) {
        printf("X");
    }
    printf("]\n");
}



