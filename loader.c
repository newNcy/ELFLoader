#include <stdio.h>
#include <linux/elf.h>
#include <string.h>
#include <malloc.h>


int main (int argc, char * argv[])
{

    FILE * file = fopen(argv[0],"rb");
    if (file == NULL) return 0;

    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));

    fread(&ehdr, sizeof(ehdr), 1, file);
    printf("程序入口:%p\n",(void*)ehdr.e_entry);
    printf("程序表头位置:%p\n",(void*)ehdr.e_phoff);
    fseek(file, ehdr.e_phoff, SEEK_SET);
    int phdrlen = ehdr.e_phnum * sizeof(Elf64_Phdr);
    Elf64_Phdr * pdhr = (Elf64_Phdr *)malloc(phdrlen);
    fread(pdhr, phdrlen, 1, file);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        printf("表类型 %p\n", pdhr[i].p_type);
    }
    fclose(file);

}

