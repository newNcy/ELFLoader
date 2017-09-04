#include "section_table.h"


void Shdr64(Elf64_Shdr shdr, char *names)
{

    printf("<%010p--%010p>[%010p]",shdr.sh_offset,shdr.sh_size+shdr.sh_offset,shdr.sh_addr);
    printf("%s\n",names+shdr.sh_name);
    printf("link[%010p]\n",shdr.sh_link);
}
void s_type( Elf64_Word type )
{
    printf("ST[");
    switch(type) {
    case 0x00:
        printf("SHT_NULL");
        break;
    case 0x01:
        printf("SHT_PROGBITS");
        break;
    case 0x02:
        printf("SHT_SYMTAB");
        break;
    case 0x03:
        printf("SHT_STRTAB");
        break;
    case 0x04:
        printf("SHT_RELA");
        break;
    case 0x05:
        printf("SHT_HASH");
        break;
    case 0x06:
        printf("SHT_DYNAMICL");
        break;
    case 0x07:
        printf("SHT_NOTE");
        break;
    case 0x08:
        printf("SHT_NOBITS");  
        break;
    case 0x09:
        printf("SHT_REL");
        break;
    case 0x0A:
        printf("SHT_SHLIB");
        break;
    case 0x0B:
        printf("SHT_DYNSYM");
        break;
    case 0x0E:
        printf("SHT_INIT_ARRAY");
        break;
    case 0x0F:
        printf("SHT_FINI_ARRAY");
        break;
    case 0x10:
        printf("SHT_PREINIT_ARRAY");
        break;
    case 0x11:
        printf("SHT_DYNSYM");
        break;
    }
}


