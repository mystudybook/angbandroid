/* Host C99 regressions for 2-cell CJK in long monster lore textblocks. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/textblock-column-layout.h"
static unsigned checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)
static void pieces(const wchar_t *src, const uint8_t *attrs, size_t width,
                   const wchar_t *const *expected, size_t count)
{
    size_t i=0, n=wcslen(src), j=0;
    while (i < n) {
        angdroid_textblock_line row = angdroid_textblock_next_line(src,attrs,n,i,width);
        wchar_t actual[256];
        size_t col=0,k;
        CHECK(row.next>i && row.next<=n);
        CHECK(row.length<256);
        memcpy(actual,src+i,row.length*sizeof(wchar_t));
        actual[row.length]=0;
        CHECK(j<count);
        CHECK(wcscmp(actual,expected[j])==0);
        for(k=0;k<row.length;++k) col +=
            (size_t)(angdroid_is_wide_text(src[i+k],attrs?attrs[i+k]:0)?2:1);
        CHECK(col<=width || (width==1 && row.length==1));
        i=row.next;
        ++j;
    }
    CHECK(j==count);
}
int main(void)
{
    static const wchar_t *const a[]={L"天地",L"玄黃"};
    static const wchar_t *const b[]={L"A中B",L"文C"};
    static const wchar_t *const c[]={L"abc",L"def",L"ghi"};
    static const wchar_t *const d[]={L"長",L"篇",L"文",L"字"};
    static const wchar_t *const e[]={L"你",L"",L"好"};
    static const wchar_t *const f[]={L"地",L"x"};
    static const uint8_t tile[]={0x80,0x00};
    static const wchar_t *const g[]={L"中x"};
    pieces(L"天地玄黃",NULL,4,a,2);
    pieces(L"A中B文C",NULL,4,b,2);
    pieces(L"abc def ghi",NULL,7,c,3);
    pieces(L"長篇文字",NULL,1,d,4);
    pieces(L"你\n\n好",NULL,8,e,3);
    pieces(L"地x",NULL,2,f,2);
    pieces(L"中x",tile,2,g,1);
    {
      angdroid_textblock_line r=angdroid_textblock_next_line(NULL,NULL,0,0,8);
      CHECK(r.length==0 && r.next==0);
    }
    puts("PASS: textblock CJK terminal columns, mixed scripts, tiles, and newlines");
    printf("Assertions: %u\n",checks);
    return 0;
}
