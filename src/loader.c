#include <stdio.h>
#include <stdlib.h>
#include <linux/elf.h>
#include <string.h>
#include <sys/mman.h>
#include <malloc.h>
#include <errno.h>
#include <dlfcn.h>

#include "section_table.h"
#include "program_table.h"


enum {H,P,SEC,A,SYM,STR,R,OPC};
int opt[OPC] = {0};

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
    if (memcmp(ehdr.e_ident,ELFMAG,4) != 0) {
        printf("not a single ELF.\n");
        abort();
    }
    if (opt[H] || opt[A]) {
         printf("ELF header:\n magic: ");
        for (int i = 0; i<EI_NIDENT;i++) {
            printf("%02x ",ehdr.e_ident[i]);
        }
        printf("\n");
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
        printf(" 文件类型                  %s",ET[ehdr.e_type]);
        printf("\n 种类                      ELF%d",ehdr.e_ident[EI_CLASS]==1?32:64);
        printf("\n 字节序                    %s", ehdr.e_ident[EI_DATA]==1?"小端":"大端");
    //printf("\n机器                      %d", ehdr.e_machine);
        printf("\n 入口地址                  0x%llx", ehdr.e_entry);
        printf("\n 程序头表偏移地址          %llu 字节", ehdr.e_phoff);
        printf("\n 节头表偏移地址            %llu 字节",ehdr.e_shoff);
        printf("\n 此头部大小                %d 字节",ehdr.e_ehsize);
        printf("\n 程序头数量                %d",ehdr.e_phnum);
        printf("\n 节头数量                  %d",ehdr.e_shnum);
        printf("\n 程序头大小                %d 字节",ehdr.e_phentsize);
        printf("\n 节头大小                  %d 字节",ehdr.e_shentsize);
        printf("\n 节头字符串表下标          %d\n\n",ehdr.e_shstrndx);
    }
   
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
    return max - min+1;
}


void * find_in_libc(char * sym)
{
    return dlsym(dlopen("libc.so",RTLD_NOW),sym);
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
    if (opt[P] || opt[A])
    printf("需要%zu字节来加载段\n",load_size);

    char * loaded = (char*)malloc(load_size);
    /* 遍历程序头表 */
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (opt[P] || opt[A])
        printf("P[%d] 0x%016llx to 0x%016llx",i,phdr[i].p_offset, phdr[i].p_filesz+phdr[i].p_offset);
        fseek(file,phdr[i].p_offset,SEEK_SET);
        /* 尝试加载到内存中  没有重定位 没有执行权限 估计不能运行 */
        if (phdr[i].p_type == PT_INTERP) {
            char buff[1024] = {0};
            fread(buff,phdr[i].p_filesz,1,file);
            if (opt[P] || opt[A])
            printf("\n%s",buff);
        }
        else if (phdr[i].p_type == PT_LOAD) {
            int fz = phdr[i].p_filesz;
            int mz = phdr[i].p_memsz;
            if(fz > mz) printf("错误的加载段");
            fread(loaded + phdr[i].p_vaddr,phdr[i].p_filesz,1,file);
            //Phdr64(phdr[i]);
        }
        if (opt[P] || opt[A])
        printf("\n");
    }
    char *exec = mmap((void*)start,load_size,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,0,0);
    if (!exec) perror("mmap");
    else
    memcpy(exec,loaded,load_size);
    
   
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
    char dym_name [1024][256] = {0}; 
    int dym_c = 0;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        if (shdr[i].sh_addr != 0x0) {
            if (opt[SEC] || opt[A])
            printf("[%08llx]%s\n",shdr[i].sh_addr,names+shdr[i].sh_name);
        }
        /* 读取整个节 */
        char * sts = (char *)malloc(shdr[i].sh_size);
        fseek(file,shdr[i].sh_offset,SEEK_SET);
        fread(sts,shdr[i].sh_size,1,file);
        if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type==SHT_DYNSYM) {
            int sc = shdr[i].sh_size/sizeof(Elf64_Sym);
            if (opt[A] || opt[SYM])
            printf("SYMTAB[%d]\n",sc);
            int strndx = shdr[i].sh_link;
            fseek(file, shdr[strndx].sh_offset,SEEK_SET);
            char * strings = (char *)malloc(shdr[strndx].sh_size);
            fread(strings,shdr[strndx].sh_size,1,file);
            if(opt[SYM] || opt[A])
            printf("ndx\tvalue\t\ttype\t\tname\n");
            for(int j = 0; j<sc; j++) {
                Elf64_Sym *sym = (Elf64_Sym*)sts+j;
                if (strings[sym->st_name] == 0) continue;
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
                    sb = ELF32_ST_BIND(sym->st_info);
                    sit = ELF32_ST_TYPE(sym->st_info);
                }
                else  if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) {
                    sb = ELF64_ST_BIND(sym->st_info);
                    sit = ELF64_ST_TYPE(sym->st_info);
                }
            if(opt[SYM] || opt[A])
                printf("%d\t%08llx\t", sym->st_shndx,sym->st_value);
            if(opt[SYM] || opt[A])
                if (sb <=2) {
                    printf("%s",SB[sb]);
                }else if (sit == 13) {
                    printf("%s",SB[3]);
                }else if (sit == 15) {
                    printf("%s",SB[4]);
                }
                if(opt[SYM] || opt[A])
                printf("_");
                if(opt[SYM] || opt[A])
                if (sit <=4) {
                    printf("%s",SIT[sit]);
                }else if (sit == 13) {
                    printf("%s",SIT[5]);
                }else if (sit == 15) {
                    printf("%s",SIT[5]);
                }
                if(opt[SYM] || opt[A])
                printf("\t%s\n",strings + sym->st_name);
                if (shdr[i].sh_type == SHT_DYNSYM) {
                    strcpy(dym_name[dym_c++],strings+sym->st_name);
                    if(opt[SYM] || opt[A])
                    printf("%s:%p\n",strings+sym->st_name,find_in_libc(strings+sym->st_name));
                }
                typedef int (*fp)(int argc,char *arg[]);
                if(!strcmp(strings+sym->st_name,"main")){
                    fp ptr = (fp)(exec+sym->st_value);
                    printf("%s:[%p]\n","main",ptr);
                    char pargv [5][25] = {"fuck!","hello"};
                    int res = ptr(argc-1,argv+1);
                    printf("\nres(%d)\n",res);
                }

            }
            free(strings);
        }else if (shdr[i].sh_type == SHT_REL) {
            if (opt[A] || opt[R])
            printf("REL\n");
        }else if (shdr[i].sh_type == SHT_RELA) {
            if (ehdr.e_machine != EM_AARCH64 ) continue;
            int sc = shdr[i].sh_size/sizeof(Elf64_Rela);

            if (opt[R] || opt[A]) {
                printf("RELA[%d]\n",sc);
                printf("offset\t\ttype\t\tsym\n");
            }
            for (int j= 0; j<sc;j++) {
                Elf64_Rela * rela = (Elf64_Rela*)sts+j;
                if (opt[A] || opt[R])
                printf("%08llx\t%llx\t\t%s\n",rela->r_offset,ELF64_R_TYPE(rela->r_info),(char*)dym_name[j]);
                *((int64_t*)(exec+rela->r_offset)) = find_in_libc(dym_name[j]);

            }

        } else if (shdr[i].sh_type == SHT_STRTAB && ( opt[STR] || opt[A])) {
            for(int j=0;j<shdr[i].sh_size;j++) {
                if (sts[j] == 0) printf("\\0");
                else printf(" %c",sts[j]);
                if ((j+1)%20 == 0) printf("\n");
            }
            printf("\n");
        }
    }

    free(names);

    
    fclose(file);

}

