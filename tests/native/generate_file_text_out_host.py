#!/usr/bin/env python3
"""Compile the production UTF-8 file-output function with a mock file API.

The emitted C99 source contains the actual production file-output functions,
not a reimplementation. Only I/O, settings, and the ang_file are mocked.
"""
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[2]
source = (root / "app/src/main/cpp/angband/z-textblock.c").read_text(encoding="utf-8")
start = source.index("static void angdroid_textblock_write_file_bytes(")
end = source.index("\n/**\n * Output text to the screen", start)
code = source[start:end]
for anchor in ("static void angdroid_textblock_write_file_bytes(",
               "static void angdroid_textblock_write_file_char(",
               "void text_out_to_file("):
    assert code.count(anchor) == 1, anchor

prefix = r'''/* GENERATED from production z-textblock.c. */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-file-text-wrap.h"
typedef struct {char data[50000];size_t length;} ang_file;
static ang_file mock;
static ang_file *text_out_file=&mock;
static int text_out_wrap=0;
static int text_out_indent=0;
static void file_write(ang_file *f,const char *s,size_t n)
{assert(f==&mock && n<sizeof(f->data)-f->length);memcpy(f->data+f->length,s,n);f->length+=n;f->data[f->length]=0;}
static void file_writec(ang_file *f,char ch){file_write(f,&ch,1);}
'''

suffix = r'''
static unsigned checks;
#define CHECK(cond) do{if(!(cond)){fprintf(stderr,"FAIL line %d: %s\noutput=%s\n",__LINE__,#cond,mock.data);exit(1);}++checks;}while(0)
static void clear(void)
{
 /* A newline through the real ASCII code path resets its persistent pos. */
 text_out_to_file(0,"\n");mock.length=0;mock.data[0]=0;
}
static int utf8_valid(const char *s)
{
 uint32_t cp;size_t n;
 while(*s){n=angdroid_utf8_next_scalar(s,&cp);
  if(!n || (n==1 && cp==0xFFFDu && (unsigned char)*s>=0x80u))return 0;
  s+=n;
 }
 return 1;
}
int main(void)
{
 text_out_indent=0;text_out_wrap=5;
 clear();text_out_to_file(0,"中文中文");
 CHECK(strcmp(mock.data,"中文\n中文")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=8;
 clear();text_out_to_file(0,"中文 hello");
 CHECK(strcmp(mock.data,"中文\nhello")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=11;
 clear();text_out_to_file(0,"中文 hello");
 CHECK(strcmp(mock.data,"中文 hello")==0);
 text_out_wrap=6;
 clear();text_out_to_file(0,"中文中文");
 CHECK(strcmp(mock.data,"中文中\n文")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=5;
 clear();text_out_to_file(0,"A中文B");
 CHECK(strcmp(mock.data,"A中文\nB")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=7;text_out_indent=2;
 clear();text_out_to_file(0,"中文\nABC中文");
 CHECK(strcmp(mock.data,"  中文\n  ABC中\n  文")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=1;text_out_indent=0;
 clear();text_out_to_file(0,"中文");
 CHECK(strcmp(mock.data,"?\n?")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=10;
 clear();text_out_to_file(0,"中\xE4\xB8");
 CHECK(strcmp(mock.data,"中??")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=5;
 clear();text_out_to_file(0,"中文");text_out_to_file(0,"ABC");
 CHECK(strcmp(mock.data,"中文\nABC")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=4;
 clear();text_out_to_file(0,"字字字字字字字字");
 CHECK(strcmp(mock.data,"字字\n字字\n字字\n字字")==0);CHECK(utf8_valid(mock.data));
 text_out_wrap=5;
 clear();text_out_to_file(0,"ASCII");
 CHECK(strcmp(mock.data,"ASCII")==0);
 {
 /* Long text must not be silently clipped to a 1024-byte buffer. */
  char large[4096];int i;
  for(i=0;i<600;i++)memcpy(large+3*i,"中",3);
  large[1800]=0;text_out_wrap=10;
  clear();text_out_to_file(0,large);
  CHECK(utf8_valid(mock.data));
  CHECK(mock.length>=1800);
 }
 {
  /* ASCII is also entitled to complete exports; the old path used
   * my_strcpy(buf, str, 1024) and lost every byte after byte 1023. */
  char original[2501], reconstructed[2501];
  size_t i, j=0;
  for(i=0;i<2500;++i)original[i]=(char)('A'+(i%26));
  original[2500]=0;
  text_out_wrap=75;text_out_indent=0;
  clear();text_out_to_file(0,original);
  for(i=0;i<mock.length;++i)
    if(mock.data[i]!='\n'){
      CHECK(j<2500);
      reconstructed[j++]=mock.data[i];
    }
  reconstructed[j]=0;
  CHECK(j==2500);
  CHECK(strcmp(reconstructed,original)==0);
  CHECK(mock.length>2500);
  CHECK(utf8_valid(mock.data));
 }
 printf("PASS: real text_out_to_file UTF-8 behavior (%u assertions)\n",checks);
 return 0;
}
'''
out = Path(sys.argv[1]) if len(sys.argv)>1 else root/'build/test_file_text_out.generated.c'
out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(prefix+code+suffix,encoding="utf-8")
print('Extracted real text_out_to_file from '+str(root))
