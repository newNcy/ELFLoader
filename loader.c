#include <stdio.h>
#include <linux/elf.h>
#include <string.h>
#include <malloc.h>

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
        printf("%p\n",type);
    }
}

int main (int argc, char * argv[])
{

    FILE * file = fopen(argv[0],"rb");
    if (file == NULL) return 0;

    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));

    fread(&ehdr, sizeof(ehdr), 1, file);

    printf("program header数量:%d\n", ehdr.e_phnum);
    printf("section数量:%d\n", ehdr.e_shnum);
    printf("程序入口:%p\n",(void*)ehdr.e_entry);
    printf("程序表大小:%d\n",ehdr.e_phentsize);
    printf("程序表头位置:%p\n",(void*)ehdr.e_phoff);
    fseek(file, ehdr.e_phoff, SEEK_SET);
    int phdrlen = ehdr.e_phnum * sizeof(Elf64_Phdr);
    Elf64_Phdr * pdhr = (Elf64_Phdr *)malloc(phdrlen);
    fread(pdhr, phdrlen, 1, file);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        printf("表[%d]类型 ",i);
        p_type(pdhr[i].p_type);
        printf("表[%d]flafs %u\n",i, pdhr[i].p_flags);
        printf("表[%d]偏移 %u\n",i, pdhr[i].p_offset);
        printf("表[%d]虚拟地址 %u\n",i, pdhr[i].p_vaddr);
        printf("表[%d]物理地址 %u\n",i, pdhr[i].p_paddr);
        printf("表[%d]文件大小 %u\n",i, pdhr[i].p_filesz);
        printf("表[%d]内存大小 %u\n",i, pdhr[i].p_memsz);
        printf("表[%d]对齐方式 %u\n",i, pdhr[i].p_align);
        printf("表[%d]段起始位置 %u\n",i, pdhr[i].p_offset + ehdr.e_phentsize);

        if(pdhr[i].p_type == PT_LOAD) { 
        }
        printf("\n");
    }
    fclose(file);

}

