#!/usr/bin/env python3
"""Exercise real ui-output.c textblock rendering and paging with fake Term.

Call from the repository root:
  python3 tests/native/generate_textblock_ui_host.py build/textblock-ui-host.c
  cc -std=c99 -Wall -Wextra -Werror -pedantic -I. build/textblock-ui-host.c -o build/textblock-ui-host
  build/textblock-ui-host

No fake production routines: extract actual display_area(), textui_textblock_place(),
and textui_textblock_show() at current checkout, compile all three verbatim.
Only the terminal, key input and textblock data collection are mocked.
"""
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[2]
source = (root / "app/src/main/cpp/angband/ui-output.c").read_text(encoding="utf-8")
first = source.index("static void display_area(")
last = source.index("\n/**\n * ------------------------------------------------------------------------\n * text_out hook", first)
code = source[first:last]
for name in ("static void display_area(", "void textui_textblock_place(",
             "struct keypress textui_textblock_show("):
    if code.count(name) != 1:
        raise ValueError("Expected exactly one function: " + name)

prefix = r'''/* GENERATED, COMPILES ACTUAL PRODUCTION UI TEXTBLOCK FUNCTIONS. */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "app/src/main/cpp/curses/unicode_layout.h"
#include "app/src/main/cpp/angband/textblock-column-layout.h"
#include "app/src/main/cpp/angband/textblock-pager-layout.h"
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define COLOUR_L_BLUE 4
#define ESCAPE 27
#define ARROW_UP 1001
#define ARROW_DOWN 1002
#define KEYPRESS_NULL {0}
#define SCREEN_REGION (region){0,0,7,3}
#define mem_free(p) free(p)
#define W 18
#define H 9
typedef struct {int col,row,width,page_rows;} region;
struct keypress {int code;};
typedef struct {const wchar_t *text;const uint8_t *attrs;size_t length;} textblock;
static wchar_t cells[H][W];
static int cx,cy,unusable,painted,cleared,save_count,load_count,footer_count;
static const int *input_keys;
static size_t input_count,input_at;
static wchar_t seen[32][H];
static int seen_count;
static void reset(void)
{
 int x,y;
 cx=cy=unusable=painted=cleared=save_count=load_count=footer_count=0;
 input_keys=NULL;input_count=input_at=0;seen_count=0;
 for(y=0;y<H;++y)for(x=0;x<W;++x)cells[y][x]=L'.';
}
static int Term_erase(int x,int y,int count)
{
 int i;
 assert(x>=0&&x<W&&y>=0&&y<H&&count>=0);
 for(i=x;i<x+count&&i<W;++i)cells[y][i]=L' ';
 ++cleared;
 return 0;
}
static int Term_gotoxy(int x,int y)
{
 assert(x>=0&&x<W&&y>=0&&y<H);
 cx=x;cy=y;unusable=0;return 0;
}
static int Term_addch(int attr,wchar_t ch)
{
 int span=angdroid_is_wide_text(ch,attr)?2:1;
 assert(!unusable && ch!=0);
 if(cx+span>W){ch=L'?';span=1;}
 cells[cy][cx]=ch;
 if(span==2)cells[cy][cx+1]=ANGDROID_TEXT_CONTINUATION;
 cx+=span;++painted;
 if(cx==W)unusable=1;
 return unusable;
}
static void c_put_str(int attr,const char *text,int y,int x)
{
 (void)attr;(void)text;
 assert(x>=0&&x<W&&y>=0&&y<H);
 ++footer_count;
}
static void screen_save(void){++save_count;}
static void screen_load(void){++load_count;}
static const char *angdroid_game_ui_text(const char *s,int enabled)
{ (void)enabled; return s; }
static int angdroid_zh_tw_enabled(void){return 1;}
static struct keypress inkey(void)
{
 struct keypress key;
 int y;
 assert(seen_count<32);
 for(y=0;y<H;++y)seen[seen_count][y]=cells[y][0];
 ++seen_count;
 key.code=input_at<input_count?input_keys[input_at++]:ESCAPE;
 return key;
}
static region region_calculate(region input){return input;}
static const wchar_t *textblock_text(textblock *tb){return tb->text;}
static const uint8_t *textblock_attrs(textblock *tb){return tb->attrs;}
static size_t textblock_calculate_lines(textblock *tb,size_t **starts,
                                       size_t **lengths,size_t width)
{
 size_t n=0,at=0;
 *starts=NULL;*lengths=NULL;
 if(!tb || !tb->text || !width)return 0;
 while(at<tb->length){
  angdroid_textblock_line line=angdroid_textblock_next_line(
   tb->text,tb->attrs,tb->length,at,width);
  size_t *s=(size_t*)realloc(*starts,(n+1)*sizeof(size_t));
  size_t *l=(size_t*)realloc(*lengths,(n+1)*sizeof(size_t));
  assert(s&&l&&line.next>at);
  *starts=s;*lengths=l;
  (*starts)[n]=at;(*lengths)[n++]=line.length;
  at=line.next;
 }
 return n;
}
'''

suffix = r'''
static unsigned checks;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);abort();}++checks;}while(0)
static textblock make_block(const wchar_t *s)
{
 static uint8_t attrs[1024];
 textblock tb;
 size_t i;
 tb.text=s;tb.length=wcslen(s);tb.attrs=attrs;
 assert(tb.length<sizeof attrs);
 for(i=0;i<tb.length;++i)attrs[i]=3;
 return tb;
}
int main(void)
{
 {
  textblock tb=make_block(L"甲乙A天");
  region area={0,0,8,2};
  reset();
  textui_textblock_place(&tb,area,NULL);
  CHECK(cells[0][0]==L'甲');
  CHECK(cells[0][1]==ANGDROID_TEXT_CONTINUATION);
  CHECK(cells[0][2]==L'乙');
  CHECK(cells[0][3]==ANGDROID_TEXT_CONTINUATION);
  CHECK(cells[0][4]==L'A');
  CHECK(cells[0][5]==L'天');
  CHECK(cells[0][6]==ANGDROID_TEXT_CONTINUATION);
  CHECK(cells[0][7]==L' ');
  CHECK(save_count==0&&load_count==0);
 }
 {
  textblock tb=make_block(L"甲\n乙\n丙\n丁");
  const int inputs[]={ARROW_DOWN,ARROW_DOWN,ARROW_UP,' ',ESCAPE};
  struct keypress key;
  region area={0,0,7,2};
  reset();input_keys=inputs;input_count=sizeof(inputs)/sizeof(inputs[0]);
  key=textui_textblock_show(&tb,area,NULL);
  CHECK(key.code==ESCAPE);
  CHECK(seen_count==5);
  CHECK(seen[0][0]==L'甲'&&seen[0][1]==L'乙');
  CHECK(seen[1][0]==L'乙'&&seen[1][1]==L'丙');
  CHECK(seen[2][0]==L'丙'&&seen[2][1]==L'丁');
  CHECK(seen[3][0]==L'乙'&&seen[3][1]==L'丙');
  CHECK(seen[4][0]==L'丙'&&seen[4][1]==L'丁');
  CHECK(footer_count==0);
  CHECK(save_count==1&&load_count==1);
 }
 {
  textblock tb=make_block(L"甲\n乙\n丙");
  const int inputs[]={ARROW_DOWN,ESCAPE};
  region area={0,0,7,3};
  reset();input_keys=inputs;input_count=2;
  (void)textui_textblock_show(&tb,area,NULL);
  CHECK(seen_count==2);
  CHECK(seen[0][0]==L'甲');
  CHECK(seen[1][0]==L'乙');
  CHECK(footer_count==1);
 }
 {
  textblock tb=make_block(L"甲\n乙");
  const int inputs[]={ESCAPE};
  region area={0,0,7,1};
  reset();input_keys=inputs;input_count=1;
  (void)textui_textblock_show(&tb,area,"Heading");
  CHECK(seen_count==1);
  CHECK(footer_count==1);
  CHECK(painted==0);
  CHECK(save_count==1&&load_count==1);
 }
 {
  textblock tb=make_block(L"甲\n乙");
  struct keypress key;
  region area={0,0,0,1};
  reset();key=textui_textblock_show(&tb,area,NULL);
  CHECK(key.code==0);
  CHECK(save_count==0&&load_count==0);
  textui_textblock_place(&tb,area,NULL);
  CHECK(cleared==0);
 }
 puts("PASS: actual production textblock renderer/scroll/header/tiny pages");
 printf("Assertions: %u\n",checks);
 return 0;
}
'''
out = Path(sys.argv[1]) if len(sys.argv)>1 else root/'build/test_textblock_ui.generated.c'
out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(prefix+code+suffix,encoding='utf-8')
print('Generated from real ui-output.c: '+str(out))
