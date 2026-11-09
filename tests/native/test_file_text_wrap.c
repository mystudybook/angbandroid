#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../../app/src/main/cpp/angband/zh-tw-file-text-wrap.h"
static char output[6000];static size_t used;static unsigned checks;
#define CHECK(x) do { if (!(x)) {fprintf(stderr,"FAIL %d: %s [got '%s']\n",__LINE__,#x,output);return 1;}++checks;}while(0)
static void emit(void *ctx,const char *s,size_t n){(void)ctx;assert(used+n<sizeof(output));memcpy(output+used,s,n);used+=n;output[used]=0;}
static void put(void *ctx,char c){emit(ctx,&c,1);}
static void reset(void){used=0;output[0]=0;}
static int valid(const char *str){uint32_t cp;size_t n;while(*str){n=angdroid_utf8_next_scalar(str,&cp);if(!n || (n==1 && cp==0xFFFDu && (unsigned char)*str>=128))return 0;str+=n;}return 1;}
int main(void){int pos;
 CHECK(!angdroid_text_has_non_ascii("ASCII words only"));
 CHECK(angdroid_text_has_non_ascii("A中文"));
 reset();pos=0;angdroid_utf8_text_file_wrap("中文中文",&pos,6,0,NULL,emit,put);
 CHECK(strcmp(output,"中文中\n文")==0);CHECK(pos==2);CHECK(valid(output));
 reset();pos=0;angdroid_utf8_text_file_wrap("中文中文",&pos,5,0,NULL,emit,put);
 CHECK(strcmp(output,"中文\n中文")==0);CHECK(pos==4);CHECK(valid(output));
 reset();pos=0;angdroid_utf8_text_file_wrap("A中文B",&pos,5,0,NULL,emit,put);
 CHECK(strcmp(output,"A中文\nB")==0);CHECK(pos==1);
 reset();pos=0;angdroid_utf8_text_file_wrap("中文\nABC中文",&pos,7,2,NULL,emit,put);
 CHECK(strcmp(output,"  中文\n  ABC中\n  文")==0);CHECK(pos==4);CHECK(valid(output));
 reset();pos=0;angdroid_utf8_text_file_wrap("中文",&pos,1,0,NULL,emit,put);
 CHECK(strcmp(output,"?\n?")==0);CHECK(valid(output));
 reset();pos=0;angdroid_utf8_text_file_wrap("中\xE4\xB8",&pos,10,0,NULL,emit,put);
 CHECK(strcmp(output,"中??")==0);CHECK(valid(output));
 reset();pos=0;angdroid_utf8_text_file_wrap("中文",&pos,5,0,NULL,emit,put);
 angdroid_utf8_text_file_wrap("ABC",&pos,5,0,NULL,emit,put);
 CHECK(strcmp(output,"中文A\nBC")==0);CHECK(valid(output));
 reset();pos=0;angdroid_utf8_text_file_wrap("字字字字字字字字",&pos,4,0,NULL,emit,put);
 CHECK(strcmp(output,"字字\n字字\n字字\n字字")==0);CHECK(valid(output));
 {
  /* Deterministic long/short UTF-8 stress across wrapping widths. */
  static const char *pieces[]={"中","界","A"," ","\n","😀","\x80","Ω","B"};
  uint32_t rng=0x19B194u;
  size_t attempt;
  for(attempt=0;attempt<2000;++attempt){
   char source[512];size_t size=0,i,columns=0;int width=(int)(attempt%24)+1;
   for(i=0;i<30;++i){
    const char *part;size_t bytes;
    rng=rng*1664525u+1013904223u;
    part=pieces[(rng>>16)%9u];bytes=strlen(part);
    CHECK(size+bytes<sizeof(source));
    memcpy(source+size,part,bytes);size+=bytes;
   }
   source[size]=0;reset();pos=0;
   angdroid_utf8_text_file_wrap(source,&pos,width,0,NULL,emit,put);
   CHECK(valid(output));CHECK(pos>=0 && pos<=width);
   for(i=0;i<used;){
    size_t n;uint32_t codepoint;
    n=angdroid_utf8_next_scalar(output+i,&codepoint);
    CHECK(n>0);
    if(codepoint=='\n'){
     CHECK(columns<=(size_t)width);columns=0;
    }else columns+=(size_t)angdroid_terminal_columns(codepoint);
    i+=n;
   }
   CHECK(columns<=(size_t)width);
  }
 }
 printf("PASS: %u UTF-8 file-wrap assertions\n",checks);return 0;
}
