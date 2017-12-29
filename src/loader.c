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
    printf("ELF header:\n magic: ");
    for (int i = 0; i<EI_NIDENT;i++) {
        printf("%02x ",ehdr.e_ident[i]);
    }
    char *ET[7] = {
        "No file type",
        "Relocatable file",
        "Executable file",
        "Shared object file",
        "Core file"
    };
    char *MC[20] = {
        "No machine",
        "AT&T WE 321000",
        "SPARC",
        "Intel Architecture",
        "Motorola 68000",
        "Motorola 88000",
        "Intel 80860",
        "MIPS RS3000 Big-Endian",
        "MIPS RS4000 Big-Endian"
    };
    printf("\n 文件类型                  %s",ET[ehdr.e_type]);
    printf("\n 种类                      ELF%d",ehdr.e_ident[EI_CLASS]==1?32:64);
    printf("\n 字节序                    %s", ehdr.e_ident[EI_DATA]==1?"小端":"大端");
    //printf("\n机器                      %d", ehdr.e_machine);
    printf("\n 入口地址                  0x%x", ehdr.e_entry);
    printf("\n 程序头表偏移地址          %d 字节", ehdr.e_phoff);
    printf("\n 节头表偏移地址            %d 字节",ehdr.e_shoff);
    printf("\n 此头部大小                %d 字节",ehdr.e_ehsize);
    printf("\n 程序头数量                %d",ehdr.e_phnum);
    printf("\n 节头数量                  %d",ehdr.e_shnum);
    printf("\n 程序头大小                %d 字节",ehdr.e_phentsize);
    printf("\n 节头大小                  %d 字节",ehdr.e_shentsize);
    printf("\n 节头字符串表下标          %d\n\n",ehdr.e_shstrndx);
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

  //  char * loaded = (char*)malloc(end-start);
    /* 遍历程序头表 */
    for (int i = 0; i < ehdr.e_phnum; i++) {
        fseek(file,phdr[i].p_offset,SEEK_SET);
        /* 尝试加载到内存中  没有重定位 没有执行权限 估计不能运行 */
        //fread(loaded + phdr[i].p_vaddr,phdr[i].p_filesz,1,file);
        if (phdr[i].p_type == PT_INTERP) {
            printf("解释器解释的段\n");
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
       Shdr64(shdr[i],names);
        if (shdr[i].sh_type == SHT_SYMTAB) {
            int sc = shdr[i].sh_size/sizeof(Elf64_Sym);
            printf("符号表[%d]\n",sc);
            int strndx = shdr[i].sh_link;
            printf("连接到[%010p]字符串表\n",strndx);
            fseek(file, shdr[strndx].sh_offset,SEEK_SET);
            char * strings = (char *)malloc(shdr[strndx].sh_size);
            fread(strings,shdr[strndx].sh_size,1,file);


            fseek(file,shdr[i].sh_offset,SEEK_SET);;
            for(int j = 0; j<sc; j++) {
                Elf64_Sym sym;
                fread(&sym,sizeof(sym),1,file);
                printf("[%s]=[%x]",strings + sym.st_name, sym.st_value);
                char SIT[10][7] = {
                    "NOTYPE",
                    "OBJECT",
                    "FUNC",
                    "SECTION",
                    "FILE",
                    "LOPROC",
                    "HIPROC"
                };
                char SB[10][7] = {
                    "LOCAL",
                    "GLOBAL",
                    "WEAK",
                    "LOPROC",
                    "HIPROC"
                };
                int sit = 0; // TYPE
                int sb = 0; // BIND
                int sif = 0; //INFO
                if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
                    sb = ELF32_ST_BIND(sym.st_info);
                    sit = ELF32_ST_TYPE(sym.st_info);
                }
                else  if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) {
                    sb = ELF64_ST_BIND(sym.st_info);
                    sit = ELF64_ST_TYPE(sym.st_info);
                }
                
                printf("BIND_");
                if (sb <=2) {
                    printf("%s",SB[sb]);
                }else if (sit == 13) {
                    printf("%s",SB[3]);
                }else if (sit == 15) {
                    printf("%s",SB[4]);
                }
                printf(" ");
                if (sit <=4) {
                    printf("TYPE_%s\n",SIT[sit]);
                }else if (sit == 13) {
                    printf("TYPE_%s\n",SIT[5]);
                }else if (sit == 15) {
                    printf("TYPE_%s\n",SIT[5]);
                }
            }
        }else if (shdr[i].sh_type == SHT_REL) {
            printf("重定位表\n");
        } else if (shdr[i].sh_type == SHT_STRTAB) {
        }
        if (shdr[i].sh_addr != 0x0) {
            printf("[%010p]%s\n",shdr[i].sh_addr,names+shdr[i].sh_name);
            for (int j = shdr[i].sh_addr; j < shdr[i].sh_size + shdr[i].sh_addr; j+= 4) {
                //printf("%02x%02x%02x%02x \n",
                       //loaded[j+3],
                       //loaded[j+2],
                       //loaded[j+1],
                       //loaded[j+0]);
            }
            printf("\n");
        }     
    }
    free(names);

    
    fclose(file);

}

