#!/usr/bin/env python3
"""Compile actual z-form.c vstrnfmt() and z-textblock.c with host stubs.

Unlike a logic-equivalent test, the generated C includes the exact source
for the production formatter, textblock appenders and text_out functions.
The only simulated parts are memory, UTF-8 conversion and the output hook.
"""
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[2]
form = (root/"app/src/main/cpp/angband/z-form.c").read_text(encoding="utf-8")
block = (root/"app/src/main/cpp/angband/z-textblock.c").read_text(encoding="utf-8")

def cut(source, start, end):
    a = source.index(start)
    b = source.index(end, a)
    return source[a:b]

formatter = cut(form, "size_t vstrnfmt(", "/**\n * Add a formatted string to the end of a string")
resize_append = cut(block, "static void textblock_resize_if_needed(",
                    "/**\n * Add a graphics tile")
append = cut(block, "void textblock_append(textblock *tb, const char *fmt, ...)",
             "/**\n * Return a pointer to the text inputted")
output = cut(block, "static void angdroid_text_out_vformatted(",
             "/**\n * Given a \"formatted\" chunk")
for marker in ("size_t vstrnfmt(", "void textblock_append(",
               "static void angdroid_text_out_vformatted(", "void text_out("):
    assert (formatter + resize_append + append + output).count(marker) == 1, marker

prefix = r'''/* GENERATED from actual z-form.c and z-textblock.c. */
#include <assert.h>
#include <ctype.h>
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#define COLOUR_WHITE 1
#define TEXTBLOCK_LEN_INITIAL 128
#define TEXTBLOCK_LEN_INCR(x) ((x)+128)
#define text_wcslen wcslen
typedef struct {wchar_t *text;uint8_t *attrs;size_t strlen,size;} textblock;
static void *mem_alloc(size_t n) {void *p=malloc(n);assert(p);return p;}
static void *mem_zalloc(size_t n){void *p=calloc(1,n);assert(p);return p;}
static void *mem_realloc(void *p,size_t n){void *r=realloc(p,n);assert(r);return r;}
#define mem_free(x) free(x)
static int text_mbstowcs(wchar_t *out,const char *in,size_t n){
 size_t res=mbstowcs(out,in,n);assert(res!=(size_t)-1);return (int)res;
}
static char captured[18000];
static uint8_t captured_attr;
static unsigned capture_count;
static void capture(uint8_t a,const char *s){
 size_t n=strlen(s);assert(n<sizeof(captured));
 memcpy(captured,s,n+1);captured_attr=a;++capture_count;
}
static void (*text_out_hook)(uint8_t,const char*)=capture;
'''

suffix = r'''
static unsigned checks;
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}++checks;}while(0)
static textblock make_tb(void) {
 textblock t;t.size=TEXTBLOCK_LEN_INITIAL;t.strlen=0;
 t.text=mem_zalloc(t.size*sizeof(wchar_t));t.attrs=mem_zalloc(t.size);
 return t;
}
static void free_tb(textblock *t){free(t->text);free(t->attrs);}
static size_t format_to(char *buf,size_t cap,const char *format,...) {
 va_list args;size_t result;
 va_start(args,format);result=vstrnfmt(buf,cap,format,args);va_end(args);
 return result;
}
int main(void){
 char lore[7000],buf[15000],expected[8000];
 textblock tb;
 size_t n,i;
 const char *locale=setlocale(LC_CTYPE,"C.UTF-8");
 if(!locale)locale=setlocale(LC_CTYPE,"en_US.UTF-8");
 if(!locale)locale=setlocale(LC_CTYPE,"");
 CHECK(locale!=NULL);
 for(i=0;i<1500;++i)memcpy(lore+i*3,"中",3);
 lore[4500]=0;

 /* New narrow %s path in the *real* formatter may fill large output. */
 n=format_to(buf,sizeof(buf),"Name: %s (%d)",lore,42);
 CHECK(n==4511);
 CHECK(strncmp(buf,"Name: ",6)==0);
 CHECK(memcmp(buf+6,lore,4500)==0);
 CHECK(strcmp(buf+4506," (42)")==0);

 /* Small output buffers must accurately signal saturation for retries. */
 n=format_to(buf,10,"%s / %d",lore,7);
 CHECK(n==9);
 CHECK(buf[9]==0);
 n=format_to(buf,sizeof(buf),"%s-%s",lore,"END");
 CHECK(n==4504);
 CHECK(strcmp(buf+4500,"-END")==0);
 n=format_to(buf,sizeof(buf),"%-8s/%02d","A",4);
 CHECK(strcmp(buf,"A       /04")==0);
 n=format_to(buf,sizeof(buf),"%*s",6,"x");
 CHECK(strcmp(buf,"     x")==0);
 n=format_to(buf,sizeof(buf),"%.3s",lore);
 CHECK(n==3);
 CHECK(memcmp(buf,"中",3)==0);

 /* This passes actual vstrnfmt() through real textblock_vappend_c(). */
 tb=make_tb();
 textblock_append(&tb,"Name: %s (%d)",lore,42);
 CHECK(tb.strlen==1509);
 CHECK(tb.text[0]==L'N');
 CHECK(tb.text[6]==L'中'&&tb.text[1505]==L'中');
 CHECK(tb.text[1508]==L')');
 textblock_append_c(&tb,6," %s (%d)","龍",5);
 CHECK(tb.attrs[1509]==6);
 CHECK(tb.strlen==1515);
 free_tb(&tb);

 text_out("Name: %s (%d)",lore,42);
 CHECK(strlen(captured)==4511);
 CHECK(strncmp(captured,"Name: ",6)==0);
 CHECK(strcmp(captured+4506," (42)")==0);
 CHECK(captured_attr==COLOUR_WHITE);
 text_out_c(7,"Name: %s (%d)",lore,99);
 CHECK(strlen(captured)==4511);
 CHECK(strcmp(captured+4506," (99)")==0);
 CHECK(captured_attr==7);
 text_out("%s",lore);
 CHECK(strlen(captured)==4500 && strcmp(captured,lore)==0);
 text_out("Damage %d (%s)",12,"fire");
 CHECK(strcmp(captured,"Damage 12 (fire)")==0);
 CHECK(capture_count==4);
 printf("PASS: %u actual native compound-format UTF-8 assertions\n",checks);
 return 0;
}
'''
dest=Path(sys.argv[1]) if len(sys.argv)>1 else root/"build/test_compound_format.generated.c"
dest.parent.mkdir(parents=True,exist_ok=True)
dest.write_text(prefix + formatter + resize_append + append + output + suffix,encoding="utf-8")
print("Extracted real C99 z-form formatter and z-textblock append/output:",dest)
