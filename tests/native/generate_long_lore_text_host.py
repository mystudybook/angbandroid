#!/usr/bin/env python3
"""Build host C99 tests from real z-textblock.c plain-%s output functions.

The production resizing, append and output functions are extracted verbatim.
Only external allocations, UTF-8 conversion, formatting and UI hooks are mocked.
"""
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[2]
source = (root / "app/src/main/cpp/angband/z-textblock.c").read_text(encoding="utf-8")

def extract(start, end):
    at = source.index(start)
    stop = source.index(end, at)
    return source[at:stop]

resize = extract("static void textblock_resize_if_needed(", "/**\n * Add a graphics tile")
append = extract("void textblock_append(textblock *tb, const char *fmt, ...)",
                 "/**\n * Return a pointer to the text inputted")
out = extract("void text_out(const char *fmt, ...)",
              "/**\n * Given a \"formatted\" chunk")

prefix = r'''/* GENERATED: production z-textblock.c append and text_out functions. */
#include <assert.h>
#include <locale.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#define COLOUR_WHITE 1
#define TEXTBLOCK_LEN_INITIAL 128
#define TEXTBLOCK_LEN_INCR(x) ((x)+128)
typedef struct {wchar_t *text;uint8_t *attrs;size_t strlen,size;} textblock;
static void *mem_zalloc(size_t bytes){void *p=calloc(1,bytes);assert(p);return p;}
static void *mem_realloc(void *old,size_t bytes){void *p=realloc(old,bytes);assert(p);return p;}
#define mem_free(p) free(p)
static int text_mbstowcs(wchar_t *dst,const char *src,size_t cap)
{
 size_t n=mbstowcs(dst,src,cap);
 assert(n!=(size_t)-1);
 return (int)n;
}
static unsigned formatter_calls;
static size_t vstrnfmt(char *dst,size_t cap,const char *fmt,va_list ap)
{
 int written;size_t n;
 ++formatter_calls;
 written=vsnprintf(dst,cap,fmt,ap);
 assert(written>=0);
 n=(size_t)written;
 return n<cap?n:cap-1;
}
static const char *last_text;
static char last_copy[8000];
static uint8_t last_attr;
static unsigned hook_calls;
static void capture(uint8_t attr,const char *text)
{
 size_t bytes=strlen(text);
 assert(bytes<sizeof(last_copy));
 memcpy(last_copy,text,bytes+1);
 last_attr=attr;last_text=text;++hook_calls;
}
static void (*text_out_hook)(uint8_t,const char*)=capture;
'''

suffix = r'''
static unsigned checks;
#define CHECK(cond) do {if(!(cond)){fprintf(stderr,"FAIL at %d: %s\n",__LINE__,#cond);exit(1);}++checks;}while(0)

static textblock make_tb(void)
{
 textblock tb;
 tb.size=TEXTBLOCK_LEN_INITIAL;tb.strlen=0;
 tb.text=mem_zalloc(tb.size*sizeof(wchar_t));
 tb.attrs=mem_zalloc(tb.size);
 return tb;
}
static void free_tb(textblock *tb){free(tb->text);free(tb->attrs);}

int main(void)
{
 textblock tb;
 char utf8[7200];
 size_t i;
 CHECK(setlocale(LC_CTYPE,"C.UTF-8")!=NULL);
 for(i=0;i<2000;++i)memcpy(utf8+i*3,"中",3);
 utf8[6000]=0;

 /* From real append(): full 2000-char lore, not an inner 1024-byte clip. */
 tb=make_tb();
 formatter_calls=0;
 textblock_append(&tb,"%s",utf8);
 CHECK(tb.strlen==2000);
 CHECK(wcslen(tb.text)==2000);
 CHECK(tb.text[0]==L'中'&&tb.text[1999]==L'中');
 CHECK(tb.attrs[0]==COLOUR_WHITE&&tb.attrs[1999]==COLOUR_WHITE);
 CHECK(formatter_calls==0);
 textblock_append_c(&tb,6,"%s","獸");
 CHECK(tb.strlen==2001&&tb.text[2000]==L'獸'&&tb.attrs[2000]==6);
 CHECK(formatter_calls==0);
 textblock_append(&tb,"%d",314);
 CHECK(tb.strlen==2004&&tb.text[2001]==L'3'&&tb.text[2003]==L'4');
 CHECK(formatter_calls==1);
 free_tb(&tb);

 /* Neither uncolored nor colored text_out() may truncate plain %s. */
 formatter_calls=0;
 text_out("%s",utf8);
 CHECK(last_text==utf8);
 CHECK(last_attr==COLOUR_WHITE);
 CHECK(strlen(last_text)==6000);
 CHECK(formatter_calls==0);
 text_out_c(7,"%s",utf8);
 CHECK(last_text==utf8&&last_attr==7);
 CHECK(formatter_calls==0);
 text_out("%s",(const char *)NULL);
 CHECK(last_text!=NULL&&strcmp(last_copy,"")==0);
 CHECK(formatter_calls==0);
 text_out_c(8,"%s",(const char *)NULL);
 CHECK(last_text!=NULL&&strcmp(last_copy,"")==0&&last_attr==8);
 CHECK(formatter_calls==0);
 text_out("Score: %d",42);
 CHECK(strcmp(last_copy,"Score: 42")==0);
 CHECK(formatter_calls==1);
 text_out_c(3,"HP=%d",15);
 CHECK(strcmp(last_copy,"HP=15")==0&&last_attr==3);
 CHECK(formatter_calls==2);
 CHECK(hook_calls==6);
 printf("PASS: %u production plain-%%s lore assertions\n",checks);
 return 0;
}
'''

out_path = Path(sys.argv[1]) if len(sys.argv) > 1 else root/"build/test_long_lore.generated.c"
out_path.parent.mkdir(parents=True,exist_ok=True)
out_path.write_text(prefix + resize + append + out + suffix, encoding="utf-8")
print("Generated real z-textblock.c function test:", out_path)
