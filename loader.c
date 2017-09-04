#include <stdio.h>
#include <linux/elf.h>
#include <string.h>
#include <malloc.h>

#include "section_table.h"
#include "program_table.h"

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



/* 求加载段需要的内存 */
size_t get_load_size(Elf64_Phdr *phdr,
                     size_t phnum,
                     Elf64_Addr *start,
                     Elf64_Addr *end)
{
    Elf64_Addr min = 0xffffffff;
    Elf64_Addr max = 0x00000000;

    for(int i = 0; i < phnum; i++) {
        if (phdr[i].p_type != PT_LOAD )
            continue;
        
        if (phdr[i].p_vaddr < min) 
            min = phdr[i].p_vaddr;

        if (phdr[i].p_vaddr + phdr[i].p_memsz > max) 
            max = phdr[i].p_vaddr + phdr[i].p_memsz;
    }
    
    *start = min;
    *end = max;
    return max - min;
}


int main (int argc, char * argv[])
{

    FILE * file = fopen(argv[1],"rb");
    if (file == NULL) return 0;

    printf("文件大小 %d\n",sizeoffile(file));
    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));

    /* 解析文件头 */
    fread(&ehdr, sizeof(ehdr), 1, file);
    Ehdr64(ehdr);

   
    /* 程序头表 */
    fseek(file, ehdr.e_phoff, SEEK_SET);
    int phdrlen = ehdr.e_phnum * ehdr.e_phentsize;
    Elf64_Phdr * phdr = (Elf64_Phdr *)malloc(phdrlen);
    fread(phdr, phdrlen, 1, file);



    Elf64_Addr start,end;
    size_t load_size = get_load_size(phdr,ehdr.e_phnum,&start,&end);
    printf("需要%p字节来加载段\n",load_size);

    char * loaded = (char*)malloc(end-start);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        fseek(file,phdr[i].p_offset,SEEK_SET);
        fread(loaded + phdr[i].p_vaddr,phdr[i].p_filesz,1,file);
        if (phdr[i].p_type == PT_INTERP) {
            printf("interpreter");
        }
        if (phdr[i].p_type == PT_LOAD) {
            int l = phdr[i].p_filesz;
            Phdr64(phdr[i]);
            for (int i = 0; i < l ;i +=4 ) {
                //printf("%04x%04x ",segment[i],segment[1+i]);
                if (i %20 == 0) {
                    //printf("\n");
                }
            } 
        }
    }
    
   
    /* 节头  */
    /* 读取shstrndx表项 */
    fseek(file,ehdr.e_shoff+ehdr.e_shstrndx* ehdr.e_shentsize,SEEK_SET);
    Elf64_Shdr shstrndx;
    fread(&shstrndx,ehdr.e_shentsize,1,file);
    /* 读取shstrndx section */
    fseek(file,shstrndx.sh_offset,SEEK_SET);
    char * names = (char *)malloc(shstrndx.sh_size);    
    fread(names,shstrndx.sh_size,1,file);
    

    /* 读取解析整个section头表 */
    fseek(file, ehdr.e_shoff, SEEK_SET);
    int shdrlen = ehdr.e_shentsize *ehdr.e_shnum;
    Elf64_Shdr * shdr = (Elf64_Shdr*)malloc(shdrlen);
    fread(shdr,shdrlen,1,file);

    /* 逐个解析 */
    for (int i = 0; i < ehdr.e_shnum; i++) {
        //printf("%p\n",ftell(file));
        //Shdr64(shdr[i],names);
        if (shdr[i].sh_type == SHT_SYMTAB) {
            int sc = shdr[i].sh_size/sizeof(Elf64_Sym);
            printf("%d个全局符号\n",sc);
            int strndx = shdr[i].sh_link;
            printf("连接到[%010p]\n",strndx);
            fseek(file, shdr[strndx].sh_offset,SEEK_SET);
            char * strings = (char *)malloc(shdr[strndx].sh_size);
            fread(strings,shdr[strndx].sh_size,1,file);


            fseek(file,shdr[i].sh_offset,SEEK_SET);;
            for(int j = 0; j<sc; j++) {
                Elf64_Sym sym;
                fread(&sym,sizeof(sym),1,file);
                printf("[%s]%010p\n",strings + sym.st_name, sym.st_value);
            }
            
        }else if (shdr[i].sh_type == SHT_STRTAB) {
        }
        if (shdr[i].sh_addr != 0x0) {
            printf("[%010p]%s\n",shdr[i].sh_addr,names+shdr[i].sh_name);
            for (int j = shdr[i].sh_addr; j < shdr[i].sh_size + shdr[i].sh_addr; j+= 4) {
                printf("%02x%02x%02x%02x \n",
                       loaded[j+3],
                       loaded[j+2],
                       loaded[j+1],
                       loaded[j+0]);
            }
            printf("\n");
        }

              
    }
    free(names);

    
    fclose(file);

}

