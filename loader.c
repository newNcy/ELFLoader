#include <stdio.h>
#include <linux/elf.h>
#include <string.h>
#include <malloc.h>


int sizeoffile(FILE *f)
{
    unsigned int o = ftell(f);
    fseek(f,0,SEEK_END);
    int s = ftell(f);
    fseek(f,o,SEEK_SET);
    return s;
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
        printf("%u\n",type);
    }
}

/**
 * 打印程序头flag
 */
void p_flag(Elf64_Word flag)
{
    printf("%p ",flag);
    if (flag &  PF_R ){
        printf("PF_R ");
    }
    if (flag & PF_W) {
        printf("PF_W ");
    }
    if (flag & PF_X) {
        printf("PF_X ");
    }
    printf("\n");
}

int main (int argc, char * argv[])
{

    FILE * file = fopen(argv[0],"rb");
    if (file == NULL) return 0;

    printf("文件大小 %d\n",sizeoffile(file));
    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));

    fread(&ehdr, sizeof(ehdr), 1, file);

    printf("program header数量:%d\n", ehdr.e_phnum);
    printf("section数量:%d\n", ehdr.e_shnum);
    printf("程序入口:%p\n",(void*)ehdr.e_entry);
    printf("程序表大小:%d\n",ehdr.e_phentsize);
    printf("程序表头位置:%p\n\n",(void*)ehdr.e_phoff);
    fseek(file, ehdr.e_phoff, SEEK_SET);
    int phdrlen = ehdr.e_phnum * sizeof(Elf64_Phdr);
    Elf64_Phdr * pdhr = (Elf64_Phdr *)malloc(phdrlen);
    fread(pdhr, phdrlen, 1, file);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (pdhr[i].p_offset == 0 || pdhr[i].p_filesz == 0)
            continue;
        printf("[%d]类型 ",i);
        p_type(pdhr[i].p_type);
        printf("flags ");
        p_flag(pdhr[i].p_flags);
        printf("偏移[%u] ", pdhr[i].p_offset);
        printf("虚拟地址[%u] ", pdhr[i].p_vaddr);
        printf("物理地址[%u]\n", pdhr[i].p_paddr);
        printf("文件大小[%u] ", pdhr[i].p_filesz);
        printf("内存大小[%u]\n", pdhr[i].p_memsz);
        printf("对齐方式[%u] ", pdhr[i].p_align);
        printf("段起始位置[%u]\n", pdhr[i].p_offset + ehdr.e_phentsize);
        printf("[...%llu个字节...]\n",pdhr[i].p_filesz - ehdr.e_phentsize);

        printf("\n");
    }
    fclose(file);

}

