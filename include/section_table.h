#ifndef _SECTION_TABLE_H
#define _SECTION_TABLE_H

#include <stdio.h>
#include <linux/elf.h>

/* 解析节头 */
void Shdr64(Elf64_Shdr shdr,char *);
/* 节类型 */
void s_type( Elf64_Word type );
/* 节标志 */
void s_flag(Elf64_Word flag);

#endif
