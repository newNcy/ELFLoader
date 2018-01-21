#include <stdio.h>
#include <dlfcn.h>

int foo()
{
    printf("foo()->\n");
    return 9;
}

int  foo2()
{
    printf("foo2()->");
    return 3+foo();
}


int main(int argc,char *argv[])
{
    
    for (int i=0;i<argc;i++) {
        printf("argv[%d]:%s\n",i,argv[i]);
    }
    foo2();
    FILE * f = fopen(argv[1],"r");
    char ch;
    while (1) {
        ch = fgetc(f);
        if (feof(f)) break;
        putchar(ch);
    }
    printf("\n");
    fclose(f);
    return 16;
}

