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

formatter = cut(form, "size_t vstrnfmt(", "static char *format_buf = NULL;")
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
#include <limits.h>
#include "app/src/main/cpp/curses/unicode_utf8.h"
#include "app/src/main/cpp/angband/zh-tw-format-utf8.h"
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
/* Match the exact Android hook registered in common/angdroid.c. */
static int text_wcsz(void){return 5;}
static int text_wctomb(char *out,wchar_t wc){
 size_t n=angdroid_encode_wide_utf8(&wc,1,out,5);
 return n==(size_t)-1?-1:(int)n;
}
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
static int utf8_valid(const char *str) {
 mbstate_t state={0}; wchar_t out; size_t size;
 while(*str) {
  size=mbrtowc(&out,str,MB_LEN_MAX,&state);
  if(size==(size_t)-1 || size==(size_t)-2) return 0;
  if(size==0) break;
  str+=size;
 }
 return 1;
}
int main(void){
 char lore[7000],buf[15000];
 wchar_t wide_lore[1601];
 textblock tb;
 size_t n,i;
 const char *locale=setlocale(LC_CTYPE,"C.UTF-8");
 if(!locale)locale=setlocale(LC_CTYPE,"en_US.UTF-8");
 if(!locale)locale=setlocale(LC_CTYPE,"");
 CHECK(locale!=NULL);
 for(i=0;i<1500;++i){memcpy(lore+i*3,"中",3);wide_lore[i]=L'中';}
 lore[4500]=0;wide_lore[1500]=0;

 /* Real %%ls is UTF-8, unlimited in wchar count and retains formatting. */
 n=format_to(buf,sizeof(buf),"Name: %ls!",wide_lore);
 CHECK(n==4507);
 CHECK(strncmp(buf,"Name: ",6)==0);
 CHECK(memcmp(buf+6,lore,4500)==0);
 CHECK(strcmp(buf+4506,"!")==0);
 n=format_to(buf,sizeof(buf),"%-8.3ls",L"中文");
 CHECK(n==8); CHECK(strcmp(buf,"中     ")==0);
 n=format_to(buf,sizeof(buf),"%8.2ls",L"中文");
 CHECK(n==8); CHECK(strcmp(buf,"        ")==0);
 n=format_to(buf,sizeof(buf),"%*.*ls",10,3,L"中文");
 CHECK(n==10); CHECK(strcmp(buf,"       中")==0);
 /* Negative dynamic precision must be omitted, as in ISO C99 printf. */
 n=format_to(buf,sizeof(buf),"%.*ls",-1,L"中文");
 CHECK(n==6); CHECK(strcmp(buf,"中文")==0);
 n=format_to(buf,sizeof(buf),"%*.*ls",-8,-1,L"中文");
 CHECK(n==8); CHECK(strcmp(buf,"中文  ")==0);
 n=format_to(buf,sizeof(buf),"%*.*s",-8,-1,"中文");
 CHECK(n==8); CHECK(strcmp(buf,"中文  ")==0);
 n=format_to(buf,sizeof(buf),"%.*ls",0,L"中文");
 CHECK(n==0); CHECK(strcmp(buf,"")==0);
 n=format_to(buf,sizeof(buf),"%-8.*ls",-1,L"中文");
 CHECK(n==8); CHECK(strcmp(buf,"中文  ")==0);
 /* Verify real vstrnfmt dynamic '*' parsing against libc's C99 rules,
  * including negative precision, zero precision and negative width. */
 {
  int width, precision;
  char expected[128];
  for(width=-40;width<=40;++width) {
   for(precision=-20;precision<=40;++precision) {
    int n_expected=snprintf(expected,sizeof(expected),
      "%*.*ls",width,precision,L"ABC");
    n=format_to(buf,sizeof(buf),"%*.*ls",width,precision,L"ABC");
    CHECK(n_expected>=0);
    CHECK(n==(size_t)n_expected);
    CHECK(strcmp(buf,expected)==0);
    n_expected=snprintf(expected,sizeof(expected),
      "%*.*s",width,precision,"example");
    n=format_to(buf,sizeof(buf),"%*.*s",width,precision,"example");
    CHECK(n_expected>=0);
    CHECK(n==(size_t)n_expected);
    CHECK(strcmp(buf,expected)==0);
   }
  }
 }
 n=format_to(buf,sizeof(buf),"Cat: %ls",L"貓🐱");
 CHECK(strcmp(buf,"Cat: 貓🐱")==0);
 n=format_to(buf,10,"%ls",wide_lore);
 CHECK(n==9 && buf[9]==0);
 /* Repeated precision/width combinations exercise the actual %ls parser
  * and active Unicode hook; no intermediate-byte truncation is permitted. */
 for(i=0;i<2000;++i){
  char spec[40];
  unsigned width=(unsigned)(i%33);
  unsigned precision=(unsigned)((i*29)%75);
  snprintf(spec,sizeof spec,"%%%u.%uls",width,precision);
  n=format_to(buf,sizeof(buf),spec,wide_lore);
  CHECK(n==strlen(buf));
  CHECK(n>=width);
  CHECK(utf8_valid(buf));
 }

 /* Fixed-capacity output must not terminate halfway through UTF-8.
  * These calls exercise the real vstrnfmt() implementation for format
  * literals, regular %s and wide %ls, not just a helper model. */
 n=format_to(buf,3,"中文");
 CHECK(n==2 && strcmp(buf,"??")==0);
 n=format_to(buf,5,"%s","中文");
 CHECK(n==4 && strcmp(buf,"中?")==0);
 n=format_to(buf,6,"%ls",L"中文");
 CHECK(n==5 && strcmp(buf,"中??")==0);
 n=format_to(buf,4,"🐱");
 CHECK(n==3 && strcmp(buf,"???")==0);
 n=format_to(buf,5,"🐱");
 CHECK(n==4 && strcmp(buf,"🐱")==0);
 n=format_to(buf,1,"中文");
 CHECK(n==0 && strcmp(buf,"")==0);
 n=format_to(buf,3,"AB中");
 CHECK(n==2 && strcmp(buf,"AB")==0);

 /* New narrow %s path in the *real* formatter may fill large output. */
 n=format_to(buf,sizeof(buf),"Name: %s (%d)",lore,42);
 CHECK(n==4511);
 CHECK(strncmp(buf,"Name: ",6)==0);
 CHECK(memcmp(buf+6,lore,4500)==0);
 CHECK(strcmp(buf+4506," (42)")==0);
 /* Native UTF-8 text in the format string itself must also survive. */
 n=format_to(buf,sizeof(buf),"怪物：%s（%d）",lore,42);
 CHECK(n==4517);
 CHECK(memcmp(buf,"怪物：",9)==0);
 CHECK(memcmp(buf+9,lore,4500)==0);
 CHECK(strcmp(buf+4509,"（42）")==0);

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
 /* Narrow %s precision is byte-based but must not cut UTF-8 scalars. */
 n=format_to(buf,sizeof(buf),"%.1s","中文");
 CHECK(n==0 && strcmp(buf,"")==0);
 n=format_to(buf,sizeof(buf),"%.2s","中文");
 CHECK(n==0 && strcmp(buf,"")==0);
 n=format_to(buf,sizeof(buf),"%.3s","中文");
 CHECK(n==3 && strcmp(buf,"中")==0);
 n=format_to(buf,sizeof(buf),"%.4s","中文");
 CHECK(n==3 && strcmp(buf,"中")==0);
 n=format_to(buf,sizeof(buf),"%.5s","中文");
 CHECK(n==3 && strcmp(buf,"中")==0);
 n=format_to(buf,sizeof(buf),"%.6s","中文");
 CHECK(n==6 && strcmp(buf,"中文")==0);
 n=format_to(buf,sizeof(buf),"%8.2s","中文");
 CHECK(n==8 && strcmp(buf,"        ")==0);
 n=format_to(buf,sizeof(buf),"%-8.4s","中文");
 CHECK(n==8 && strcmp(buf,"中     ")==0);
 n=format_to(buf,sizeof(buf),"%.*s",2,"A中文");
 CHECK(n==1 && strcmp(buf,"A")==0);
 n=format_to(buf,sizeof(buf),"%.*s",4,"A中文");
 CHECK(n==4 && strcmp(buf,"A中")==0);
 n=format_to(buf,sizeof(buf),"%.*s",3,"🐱貓");
 CHECK(n==0 && strcmp(buf,"")==0);
 n=format_to(buf,sizeof(buf),"%.*s",4,"🐱貓");
 CHECK(n==4 && strcmp(buf,"🐱")==0);
 n=format_to(buf,sizeof(buf),"%.2s","abc");
 CHECK(n==2 && strcmp(buf,"ab")==0);
 /* Deliberately malformed input must not leak broken UTF-8 from
  * precision-limited %s: bad bytes are replaced with ASCII '?'. */
 n=format_to(buf,sizeof(buf),"%.*s",7,"A\x80" "中\xE4\xB8");
 CHECK(n==7 && strcmp(buf,"A?中??")==0);
 CHECK(utf8_valid(buf));
 n=format_to(buf,sizeof(buf),"%.*s",4,"A\x80" "中\xE4\xB8");
 CHECK(n==2 && strcmp(buf,"A?")==0);
 n=format_to(buf,sizeof(buf),"%8.*s",4,"A\x80" "中\xE4\xB8");
 CHECK(n==8 && strcmp(buf,"      A?")==0);
 n=format_to(buf,sizeof(buf),"%.*s",3,"�");
 CHECK(n==3 && strcmp(buf,"�")==0);
 n=format_to(buf,sizeof(buf),"%.*s",1,"\xE4\xB8");
 CHECK(n==1 && strcmp(buf,"?")==0);

 n=format_to(buf,sizeof(buf),"%*.*s",-8,4,"中文");
 CHECK(n==8 && strcmp(buf,"中     ")==0);
 n=format_to(buf,sizeof(buf),"%.*s",2501,lore);
 CHECK(n==2499 && utf8_valid(buf));
 /* The formatter is source-extracted from z-form.c.  Exercise the
  * 4-byte emoji boundary, mixed text, and padded widths repeatedly. */
 {
  static const char *sample="A中文🐱B";
  int precision_bytes, field_width;
  for(precision_bytes=0;precision_bytes<=20;++precision_bytes){
   for(field_width=-20;field_width<=20;++field_width){
    n=format_to(buf,sizeof(buf),"%*.*s",
       field_width,precision_bytes,sample);
    CHECK(n==strlen(buf));
    CHECK(utf8_valid(buf));
    CHECK(n>=(size_t)(field_width<0?-field_width:field_width));
   }
  }
 }


 /* The former tmp[1024] clipped wide numeric, pointer and float
  * fields even when vformat() or a textblock could grow the buffer. */
 n=format_to(buf,sizeof(buf),"Value:%1500d!",42);
 CHECK(n==1507);
 CHECK(strncmp(buf,"Value:",6)==0);
 CHECK(strcmp(buf+1504,"42!")==0);
 n=format_to(buf,sizeof(buf),"%02000u",37u);
 CHECK(n==2000);
 CHECK(buf[0]=='0' && strcmp(buf+1998,"37")==0);
 n=format_to(buf,sizeof(buf),"%.1300f",1.5);
 CHECK(n==1302);
 CHECK(strncmp(buf,"1.5",3)==0);
 CHECK(buf[1301]=='0');
 n=format_to(buf,sizeof(buf),"%1200c",'Z');
 CHECK(n==1200);
 CHECK(buf[0]==' ' && buf[1199]=='Z');
 n=format_to(buf,sizeof(buf),"%1400p",(void *)&tb);
 CHECK(n==1400);
 CHECK(buf[0]==' ' && buf[1399]!=' ');
 n=format_to(buf,10,"%1500d",42);
 CHECK(n==9 && buf[9]==0);
 n=format_to(buf,sizeof(buf),"HP=%d CD=%.1f",23,1.5);
 CHECK(strcmp(buf,"HP=23 CD=1.5")==0);

 /* In-place %s formatting used to be safe because arg2[] staged
  * the source. Direct snprintf() must not erase an aliased argument. */
 strcpy(buf,"Beast");
 n=format_to(buf,sizeof(buf),"%s",buf);
 CHECK(n==5 && strcmp(buf,"Beast")==0);
 strcpy(buf,"A中文B");
 n=format_to(buf,sizeof(buf),"%s",buf+1);
 CHECK(n==7 && strcmp(buf,"中文B")==0);
 strcpy(buf,"中");
 n=format_to(buf,sizeof(buf),"%8s",buf);
 CHECK(n==8 && strcmp(buf,"     中")==0);
 strcpy(buf,"ABCDEF");
 n=format_to(buf,sizeof(buf),"%.4s",buf);
 CHECK(n==4 && strcmp(buf,"ABCD")==0);
 strcpy(buf,lore);
 n=format_to(buf,sizeof(buf),"%s",buf);
 CHECK(n==4500 && strcmp(buf,lore)==0);
 {
  char joined[64]="中文";
  size_t end=strlen(joined);
  /* Both the source's NUL and destination start at joined+end.
   * Use the actual production strnfcat(), not a model append. */
  strnfcat(joined,sizeof(joined),&end,"%s",joined);
  CHECK(end==12 && strcmp(joined,"中文中文")==0);
  strnfcat(joined,sizeof(joined),&end,"%s",joined);
  CHECK(end==24 && strcmp(joined,"中文中文中文中文")==0);
 }

 /* This passes actual vstrnfmt() through real textblock_vappend_c(). */
 tb=make_tb();
 textblock_append(&tb,"Name: %s (%d)",lore,42);
 CHECK(tb.strlen==1511);
 CHECK(tb.text[0]==L'N');
 CHECK(tb.text[6]==L'中'&&tb.text[1505]==L'中');
 CHECK(tb.text[1510]==L')');
 textblock_append_c(&tb,6," %s (%d)","龍",5);
 CHECK(tb.attrs[1511]==6);
 CHECK(tb.strlen==1517);
 free_tb(&tb);
 tb=make_tb();
 textblock_append(&tb,"中文：%ls",wide_lore);
 CHECK(tb.strlen==1503);
 CHECK(tb.text[0]==L'中' && tb.text[1]==L'文');
 CHECK(tb.text[2]==L'：' && tb.text[3]==L'中');
 CHECK(tb.text[1502]==L'中');
 free_tb(&tb);

 /* Real textblock and colored output entry points must also retain
  * large numeric fields now that the inner 1024-byte staging is gone. */
 tb=make_tb();
 textblock_append(&tb,"%1500d",42);
 CHECK(tb.strlen==1500);
 CHECK(tb.text[0]==L' ');
 CHECK(tb.text[1498]==L'4' && tb.text[1499]==L'2');
 free_tb(&tb);
 text_out_c(9,"Value:%1500d!",42);
 CHECK(strlen(captured)==1507);
 CHECK(strcmp(captured+1504,"42!")==0);
 CHECK(captured_attr==9);

 text_out_c(4,"Wide:%ls!",wide_lore);
 CHECK(strlen(captured)==4506);
 CHECK(memcmp(captured+5,lore,4500)==0);
 CHECK(captured_attr==4);

 text_out("Name: %s (%d)",lore,42);
 CHECK(strlen(captured)==4511);
 CHECK(strncmp(captured,"Name: ",6)==0);
 CHECK(strcmp(captured+4506," (42)")==0);
 CHECK(captured_attr==COLOUR_WHITE);
 text_out("怪物：%s（%d）",lore,42);
 CHECK(strlen(captured)==4517);
 CHECK(memcmp(captured,"怪物：",9)==0);
 CHECK(strcmp(captured+4509,"（42）")==0);
 text_out_c(7,"Name: %s (%d)",lore,99);
 CHECK(strlen(captured)==4511);
 CHECK(strcmp(captured+4506," (99)")==0);
 CHECK(captured_attr==7);
 text_out("%s",lore);
 CHECK(strlen(captured)==4500 && strcmp(captured,lore)==0);
 text_out("Damage %d (%s)",12,"fire");
 CHECK(strcmp(captured,"Damage 12 (fire)")==0);
 CHECK(capture_count==7);
 printf("PASS: %u actual native compound-format UTF-8 assertions\n",checks);
 return 0;
}
'''
dest=Path(sys.argv[1]) if len(sys.argv)>1 else root/"build/test_compound_format.generated.c"
dest.parent.mkdir(parents=True,exist_ok=True)
dest.write_text(prefix + formatter + resize_append + append + output + suffix,encoding="utf-8")
print("Extracted real C99 z-form formatter and z-textblock append/output:",dest)
