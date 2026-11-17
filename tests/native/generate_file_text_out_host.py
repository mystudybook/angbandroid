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
 {
  /* A negative line width previously caused an infinite newline loop
   * because the ASCII scanner never advanced its input pointer. */
  text_out_wrap=75;text_out_indent=0;
  clear();text_out_wrap=-7;
  text_out_to_file(0,"ABC");
  CHECK(strcmp(mock.data,"ABC")==0);
 }
 {
  /* The first row must count its indent against the total width.
   * Old code produced "  ABCDE" (7 cells), despite a wrap of 5. */
  text_out_wrap=5;text_out_indent=2;
  clear();text_out_to_file(0,"ABCDEFGHIJK");
  CHECK(strcmp(mock.data,"  ABC\n  DEF\n  GHI\n  JK")==0);
 }
 {
  /* Unusable indentation is capped so at least one content cell
   * remains, rather than emitting an oversized prefix per line. */
  text_out_wrap=3;text_out_indent=99;
  clear();text_out_to_file(0,"ABC");
  CHECK(strcmp(mock.data,"  A\n  B\n  C")==0);
 }
 {
  /* Width=1 cannot reserve a separate indent column. */
  text_out_wrap=1;text_out_indent=1;
  clear();text_out_to_file(0,"ABC");
  CHECK(strcmp(mock.data,"A\nB\nC")==0);
 }
 {
  text_out_wrap=3;text_out_indent=-10;
  clear();text_out_to_file(0,"ABCD");
  CHECK(strcmp(mock.data,"ABC\nD")==0);
 }
 {
  /* A new chunk finishing exactly in the final cell must not force a
   * newline; prior code mistook n == len for more text beyond len. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCD");text_out_to_file(0,"E");
  CHECK(strcmp(mock.data,"ABCDE")==0);
 }
 {
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCD");text_out_to_file(0,"E");
  text_out_to_file(0,"F");
  CHECK(strcmp(mock.data,"ABCDE\nF")==0);
 }
 {
  /* Preserve the caller's one explicit newline at an exact boundary. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCD");text_out_to_file(0,"E\n");
  CHECK(strcmp(mock.data,"ABCDE\n")==0);
 }
 {
  /* Text beginning with CJK takes two columns; a narrow following
   * ASCII scalar still fits in column five across separate callbacks. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"中文");text_out_to_file(0,"A");
  CHECK(strcmp(mock.data,"中文A")==0);
 }
 {
  /* Conversely, a two-column CJK scalar must wrap if the preceding
   * ASCII fragment has only one column of space. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCD");text_out_to_file(0,"中文");
  CHECK(strcmp(mock.data,"ABCD\n中文")==0);
 }
 {
  /* Single-cell terminal: second call begins a new line. */
  text_out_wrap=1;text_out_indent=0;
  clear();text_out_to_file(0,"A");text_out_to_file(0,"B");
  CHECK(strcmp(mock.data,"A\nB")==0);
 }
 {
  /* Prior-indented fragment should also consume the last column. */
  text_out_wrap=5;text_out_indent=2;
  clear();text_out_to_file(0,"AB");text_out_to_file(0,"C");
  CHECK(strcmp(mock.data,"  ABC")==0);
 }
 {
  /* A newline exactly at the boundary must not produce a spare line. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCDE\n");
  CHECK(strcmp(mock.data,"ABCDE\n")==0);
 }
 {
  /* Exhaustively check consecutive chunks at exact column boundaries,
   * including indentation and one-cell content regions. */
  int width,margin;
  for(width=1;width<=32;++width) {
    for(margin=0;margin<width&&margin<=8;++margin) {
      char first[48],expected[96];
      int remaining=width-margin-1;
      int index;
      memset(first,'x',(size_t)remaining);
      first[remaining]=0;
      for(index=0;index<margin;++index)expected[index]=' ';
      for(index=0;index<remaining;++index)expected[margin+index]='x';
      expected[width-1]='Z';
      expected[width]=0;

      text_out_wrap=75;text_out_indent=0;
      clear();
      text_out_wrap=width;text_out_indent=margin;
      text_out_to_file(0,first);
      text_out_to_file(0,"Z");
      CHECK(strcmp(mock.data,expected)==0);
      text_out_to_file(0,"Q");
      expected[width]='\n';
      for(index=0;index<margin;++index)expected[width+1+index]=' ';
      expected[width+1+margin]='Q';
      expected[width+2+margin]=0;
      CHECK(strcmp(mock.data,expected)==0);
    }
  }
 }
 {
  /* A separate whitespace-only callback after an exact-fit line must
   * not append a sixth display cell to a five-column row. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCDE");text_out_to_file(0," ");
  CHECK(strcmp(mock.data,"ABCDE\n")==0);
 }
 {
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCDE");text_out_to_file(0,"   ");
  text_out_to_file(0,"FG");
  CHECK(strcmp(mock.data,"ABCDE\nFG")==0);
 }
 {
  /* Multiple separators arrive in the same callback as the next word. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCDE");text_out_to_file(0,"   FG");
  CHECK(strcmp(mock.data,"ABCDE\nFG")==0);
 }
 {
  /* Single-column and heavily indented lines must follow the same rule. */
  text_out_wrap=1;text_out_indent=0;
  clear();text_out_to_file(0,"A");text_out_to_file(0,"   ");
  CHECK(strcmp(mock.data,"A\n")==0);
 }
 {
  text_out_wrap=3;text_out_indent=2;
  clear();text_out_to_file(0,"A");text_out_to_file(0," ");
  CHECK(strcmp(mock.data,"  A\n")==0);
 }
 {
  /* Preserve existing punctuation handling: comma and period may be
   * placed one cell beyond wrap, unlike whitespace. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCDE");text_out_to_file(0,",");
  CHECK(strcmp(mock.data,"ABCDE,")==0);
 }
 {
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"ABCDE");text_out_to_file(0,".");
  CHECK(strcmp(mock.data,"ABCDE.")==0);
 }
 {
  /* The Unicode exporter already wraps and discards boundary spaces;
   * ASCII output now observes the same whole-line behavior. */
  text_out_wrap=5;text_out_indent=0;
  clear();text_out_to_file(0,"中文A");text_out_to_file(0," ");
  CHECK(strcmp(mock.data,"中文A\n")==0);
 }
 printf("PASS: real text_out_to_file UTF-8 behavior (%u assertions)\n",checks);
 return 0;
}
'''
out = Path(sys.argv[1]) if len(sys.argv)>1 else root/'build/test_file_text_out.generated.c'
out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(prefix+code+suffix,encoding="utf-8")
print('Extracted real text_out_to_file from '+str(root))
