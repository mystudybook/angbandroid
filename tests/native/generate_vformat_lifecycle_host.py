#!/usr/bin/env python3
"""Host regression for actual z-form.c vformat(), vformat_kill() and strnfmt().

Extracts production functions directly; stubs only allocator, formatted
byte writer and fatal error handler. No Android/NDK build required.
"""
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[2]
source = (root / "app/src/main/cpp/angband/z-form.c").read_text(encoding="utf-8")
start = source.index("static char *format_buf = NULL;")
end = source.index("/**\n * Do a vstrnfmt() into (see above)", start)
production = source[start:end]
assert "void vformat_kill(void)" in production
assert "format_buf = NULL;" in production
assert "format_len = 0;" in production
assert "if (capacity > ((size_t)-1) / 2)" in production
assert "angdroid_vstrnfmt_has_aliased_source(" in production

preflight_start = source.index("static bool angdroid_vstrnfmt_has_aliased_source(")
preflight_end = source.index("/* The existing parser/formatter",preflight_start)
preflight = source[preflight_start:preflight_end]
header = (root / "app/src/main/cpp/angband/zh-tw-format-utf8.h").read_text(encoding="utf-8")
alias_start = header.index("static inline int angdroid_format_string_overlaps_output(")
alias_end = header.index("/* Repair an incomplete scalar",alias_start)
alias_helper = header[alias_start:alias_end]

prefix = r'''/* Auto-generated from production z-form.c, do not edit directly. */
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
static unsigned allocations, reallocations, frees, fatal_calls, formatter_calls;
static bool simulate_saturated_output;
static jmp_buf exit_to_test;
static void *mem_zalloc(size_t n) {
 void *p=calloc(1,n); assert(p); ++allocations; return p;
}
static void *mem_alloc(size_t n) {
 void *p=malloc(n); assert(p); ++allocations; return p;
}
static void *mem_realloc(void *p,size_t n) {
 void *new_ptr; assert(n);
 new_ptr=realloc(p,n); assert(new_ptr); ++reallocations; return new_ptr;
}
static void mem_free(void *p) {
 if(p)++frees;
 free(p);
}
static void quit(const char *reason) {
 assert(reason != NULL && reason[0]);
 ++fatal_calls;
 longjmp(exit_to_test,1);
}
static size_t vstrnfmt(char *buf,size_t capacity,const char *fmt,va_list args) {
 int written;
 ++formatter_calls;
 if(simulate_saturated_output) return capacity-1;
 written=vsnprintf(buf,capacity,fmt,args);
 assert(written>=0);
 return (size_t)written>=capacity ? capacity-1 : (size_t)written;
}
'''
suffix = r'''
static unsigned checks;
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}++checks;}while(0)
static char *invoke(const char *fmt,...) {
 va_list args;
 char *result;
 va_start(args,fmt);
 result=vformat(fmt,args);
 va_end(args);
 return result;
}
int main(void) {
 char long_text[7001];
 unsigned previous_frees, previous_allocs, previous_reallocs;
 unsigned i;
 for(i=0;i<7000;++i)long_text[i]=(i%2==0)?'A':'B';
 long_text[7000]=0;
 CHECK(format_buf==NULL && format_len==0);
 CHECK(strcmp(invoke(NULL),"")==0);
 CHECK(allocations==1 && format_len==1024);
 CHECK(strcmp(invoke("%s","貓咪"),"貓咪")==0);
 CHECK(strcmp(invoke(NULL),"貓咪")==0);
 CHECK(strcmp(invoke("HP=%d",88),"HP=88")==0);
 CHECK(strcmp(invoke("%s",long_text),long_text)==0);
 CHECK(format_len>=7001);
 CHECK(reallocations>=3);
 previous_frees=frees;
 vformat_kill();
 CHECK(frees==previous_frees+1);
 CHECK(format_buf==NULL && format_len==0);
 previous_frees=frees;
 vformat_kill();
 CHECK(frees==previous_frees);
 CHECK(format_buf==NULL && format_len==0);
 previous_allocs=allocations;
 CHECK(strcmp(invoke("再次啟動：%d",3),"再次啟動：3")==0);
 CHECK(format_len==1024 && allocations==previous_allocs+1);
 CHECK(strcmp(invoke("%s",long_text),long_text)==0);
 previous_frees=frees;
 vformat_kill();
 CHECK(frees==previous_frees+1);
 CHECK(format_buf==NULL && format_len==0);
 /* Force a near-SIZE_MAX state without asking for huge allocations.
  * The mock returns 'capacity - 1' before touching the small buffer. */
 format_buf=mem_zalloc(32);
 format_len=(size_t)-1;
 simulate_saturated_output=true;
 previous_reallocs=reallocations;
 if(setjmp(exit_to_test)==0) {
  invoke("%d",1);
  CHECK(0);
 }
 CHECK(fatal_calls==1);
 CHECK(reallocations==previous_reallocs);
 CHECK(format_len==(size_t)-1);
 CHECK(format_buf!=NULL);
 simulate_saturated_output=false;
 vformat_kill();
 CHECK(format_buf==NULL && format_len==0);
 CHECK(strcmp(invoke("%s","恢復"),"恢復")==0);
 vformat_kill();
 CHECK(format_buf==NULL && format_len==0);
 /* Exercise restart/cleanup repeatedly, including a NULL format that
  * returns the current cached text instead of creating an extra buffer. */
 for(i=0;i<128;++i) {
  char expected[64];
  snprintf(expected,sizeof(expected),"第 %u 次",i);
  CHECK(strcmp(invoke("第 %u 次",i),expected)==0);
  CHECK(format_buf!=NULL && format_len==1024);
  CHECK(strcmp(invoke(NULL),format_buf)==0);
  vformat_kill();
  CHECK(format_buf==NULL && format_len==0);
 }
 CHECK(allocations==frees);
 /* Force the old 1024-byte static buffer to grow with a %s argument
  * that points into it. The original must remain alive until all retries
  * complete, including the first saturated formatting attempt. */
 {
  char seed[1016];
  char expected[1200];
  char *alias, *old_cache;
  memset(seed,'X',1015); seed[1015]=0;
  vformat_kill();
  alias=invoke("%s",seed);
  CHECK(strlen(alias)==1015 && format_len==1024);
  old_cache=alias;
  snprintf(expected,sizeof(expected),"名稱：%s [HP %d]",seed,17);
  CHECK(strcmp(invoke("名稱：%s [HP %d]",alias,17),expected)==0);
  CHECK(format_len>=strlen(expected)+1);
  CHECK(format_buf!=old_cache);
  CHECK(strcmp(invoke(NULL),expected)==0);
  vformat_kill();
 }
 /* A format string itself can point into the static buffer. The
  * resulting 1500-character field must survive output growth. */
 {
  char *fmt_ptr;
  char expected[2000];
  unsigned before_grow=reallocations;
  vformat_kill();
  fmt_ptr=invoke("%s","Health:%01500d");
  CHECK(format_len==1024);
  snprintf(expected,sizeof(expected),"Health:%01500d",7);
  CHECK(strcmp(invoke(fmt_ptr,7),expected)==0);
  CHECK(strlen(format_buf)==1507);
  CHECK(reallocations>before_grow);
  vformat_kill();
 }
 /* The aliased content itself may be 338 UTF-8 Chinese scalars.
  * Growing to make room for a translated prefix must preserve all bytes. */
 {
  char chinese[1015], expected[1200];
  char *cached;
  unsigned j;
  vformat_kill();
  for(j=0;j<338;++j)memcpy(chinese+j*3,"中",3);
  chinese[1014]=0;
  cached=invoke("%s",chinese);
  CHECK(strlen(cached)==1014 && format_len==1024);
  snprintf(expected,sizeof(expected),"資料：%s（%d）",chinese,5);
  CHECK(strcmp(invoke("資料：%s（%d）",cached,5),expected)==0);
  CHECK(strlen(format_buf)==strlen(expected));
  vformat_kill();
 }
 /* fmt can point to the middle, not just the start, of the cache. */
 {
  char expected[2000];
  char *cached;
  vformat_kill();
  cached=invoke("%s","Ignore:Score:%01500d");
  CHECK(format_len==1024);
  snprintf(expected,sizeof(expected),"Score:%01500d",6);
  CHECK(strcmp(invoke(cached+7,6),expected)==0);
  CHECK(format_len>=strlen(expected)+1);
  vformat_kill();
 }
 /* Non-overlapping formats keep the existing static allocation. */
 {
  char *before;
  unsigned alloc_before;
  vformat_kill();
  before=invoke("First");
  alloc_before=allocations;
  CHECK(strcmp(invoke("Second"),"Second")==0);
  CHECK(format_buf==before);
  CHECK(allocations==alloc_before);
  vformat_kill();
 }
 CHECK(allocations==frees);
 CHECK(formatter_calls>=136);
 printf("PASS: %u actual vformat lifecycle assertions\n",checks);
 return 0;
}
'''
target = Path(sys.argv[1]) if len(sys.argv) > 1 else root / "build/test_vformat_lifecycle.generated.c"
target.parent.mkdir(parents=True, exist_ok=True)
target.write_text(prefix+alias_helper+preflight+production+suffix, encoding="utf-8")
print("Extracted vformat lifecycle source:",target)
