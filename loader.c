#include <stdio.h>
#include <linux/elf.h>
#include <string.h>
#include <malloc.h>


/* 文件大小 */
int sizeoffile(FILE *f)
{
    unsigned int o = ftell(f);
    fseek(f,0,SEEK_END);
    int s = ftell(f);
    fseek(f,o,SEEK_SET);
    return s;
}


void Ehdr64(Elf64_Ehdr ehdr);
void Shdr64(Elf64_Shdr shdr,char *);
void Phdr64(Elf64_Phdr phdr);
void p_type( Elf64_Word type );
void p_flag(Elf64_Word flag);

void s_type( Elf64_Word type );
void s_flag(Elf64_Word flag);

/**
 * 程序头
 */
void Ehdr64(Elf64_Ehdr ehdr)
{
    putchar(ehdr.e_ident[EI_MAG0]);
    putchar(ehdr.e_ident[EI_MAG1]);
    putchar(ehdr.e_ident[EI_MAG2]);
    putchar(ehdr.e_ident[EI_MAG3]);
    putchar('\n');
    printf("Bit[%d]\n",ehdr.e_ident[EI_CLASS]==1?32:64);
    printf("PN[%d]\n", ehdr.e_phnum);
    printf("SN[%d]\n", ehdr.e_shnum);
    printf("->[%p]\n",(void*)ehdr.e_entry);
    printf("PS[%d]\n",ehdr.e_phentsize);
    printf("PO[%p]\n",(void*)ehdr.e_phoff);
    printf("SS[%d]\n",ehdr.e_shentsize);
    printf("SO[%p]\n\n",(void*)ehdr.e_shoff);
    printf("SIDX[%p]\n\n",(void*)ehdr.e_shstrndx);
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
        printf("SHT_GROUP");
        break;
    case 0x12:
        printf("SHT_SYMTAB_SHNDX");
        break;
    case 0x13:
        printf("SHT_NUM");  
        break;
    case 0x60000000:
        printf("SHT_LOOS");  
        break;
    default:
        printf("%p",type);
    }
    printf("]\n");
}
void s_flag(Elf64_Word flag)
{
   printf("SF[ ");
   if (flag & 0x01) {
       printf("SHF_WRITE ");
   }
   if (flag & 0x02) {
       printf("SHF_ALLOC ");
   }
   if (flag & 0x04) {
       printf("SHF_EXECINSTR ");
   }
   if (flag & 0x10) {
       printf("SHF_MERGE ");
   }
   if (flag & 0x20) {
       printf("SHF_STRINGS ");
   }
   if (flag & 0x40) {
       printf("SHF_INFO_LINK ");
   }
   if (flag & 0x80) {
       printf("SHF_LINK_ORDER ");
   }
   if (flag & 0x100) {
       printf("SHF_OS_NONCONFORMING ");
   }
   if (flag & 0x200) {
       printf("SHF_GROUP ");
   }
   if (flag & 0x400) {
       printf("SHF_TLS ");
   }
   if (flag & 0x0ff00000) {
       printf("SHF_MASKOSR ");
   }
   if (flag & 0xf0000000) {
       printf("SHF_MASKPROC ");
   }
   if (flag & 0x40000000) {
       printf("SHF_ORDERED ");
   }
   if (flag & 0x80000000) {
       printf("SHF_EXCLUDE ");
   }
   
   printf(" ]\n");
}

/**
 * 节头
 */
void Shdr64(Elf64_Shdr shdr,char * shstrtab)
{
    printf("<-------->\n");
    if(shstrtab != 0)
    printf("SN[%s]\n",shstrtab + shdr.sh_name);
    s_type(shdr.sh_type);
    s_flag(shdr.sh_flags);
    printf("SA[%p]\n",shdr.sh_addr);
    printf("SO[%p]\n",shdr.sh_offset);
    printf("SS[%p]\n",shdr.sh_size);
    printf("\n");
}



void Phdr64(Elf64_Phdr pdhr)
{
    printf("[-------]\n");
    p_type(pdhr.p_type);
    p_flag(pdhr.p_flags);
    printf("OF[%u]\n", pdhr.p_offset);
    printf("VA[%u]\n", pdhr.p_vaddr);
    printf("PA[%u]\n", pdhr.p_paddr);
    printf("FS[%u]\n", pdhr.p_filesz);
    printf("MS[%u]\n", pdhr.p_memsz);
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

int main (int argc, char * argv[])
{

    FILE * file = fopen("a","rb");
    if (file == NULL) return 0;

    printf("文件大小 %d\n",sizeoffile(file));
    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));

    fread(&ehdr, sizeof(ehdr), 1, file);
    Ehdr64(ehdr);

   
    fseek(file, ehdr.e_phoff, SEEK_SET);
    int phdrlen = ehdr.e_phnum * ehdr.e_phentsize;
    Elf64_Phdr * phdr = (Elf64_Phdr *)malloc(phdrlen);
    fread(phdr, phdrlen, 1, file);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        //Phdr64(phdr[i]);
       
    }
    
    rewind(file);
    fseek(file,ehdr.e_shoff+ehdr.e_shstrndx* ehdr.e_shentsize,SEEK_SET);
    Elf64_Shdr shstrndx;
    fread(&shstrndx,ehdr.e_shentsize,1,file);
    fseek(file,shstrndx.sh_offset,SEEK_SET);
    char * names = (char *)malloc(shstrndx.sh_size);
    fread(names,shstrndx.sh_size,1,file);


    fseek(file, ehdr.e_shoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_shnum; i++) {

        //printf("%p\n",ftell(file));
        Elf64_Shdr shdr;
        fread(&shdr, sizeof(Elf64_Shdr), 1, file);
        Shdr64(shdr,names);    
    }
    fclose(file);

}

