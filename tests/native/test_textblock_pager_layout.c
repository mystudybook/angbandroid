/* Host C99 checks for mobile narrow-window textblock paging. */
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/textblock-pager-layout.h"
static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)
int main(void)
{
    int rows, header;
    for (rows = -2; rows <= 100; ++rows) {
        for (header = 0; header <= 1; ++header) {
            int footer = -1;
            int visible = angdroid_textblock_pager_content_rows(rows,
                header, &footer);
            int available = rows > 0 ? rows - header : 0;
            if (available < 0) available = 0;
            CHECK(visible >= 0);
            CHECK(footer == 0 || footer == 1);
            CHECK(visible + 2 * footer == available);
            CHECK(footer == (available >= 3));
            CHECK(angdroid_textblock_pager_content_rows(rows,header,NULL)
                  == visible);
            CHECK(angdroid_textblock_pager_clamp_start(100, 10, visible)
                  <= 10);
        }
    }
    CHECK(angdroid_textblock_pager_content_rows(1,0,NULL)==1);
    CHECK(angdroid_textblock_pager_content_rows(1,1,NULL)==0);
    CHECK(angdroid_textblock_pager_content_rows(2,0,NULL)==2);
    CHECK(angdroid_textblock_pager_content_rows(2,1,NULL)==1);
    CHECK(angdroid_textblock_pager_content_rows(3,0,NULL)==1);
    CHECK(angdroid_textblock_pager_content_rows(3,1,NULL)==2);
    CHECK(angdroid_textblock_pager_content_rows(4,1,NULL)==1);
    CHECK(angdroid_textblock_pager_content_rows(24,0,NULL)==22);
    CHECK(angdroid_textblock_pager_content_rows(24,1,NULL)==21);
    CHECK(angdroid_textblock_pager_clamp_start(0,0,2)==0);
    CHECK(angdroid_textblock_pager_clamp_start(5,5,0)==0);
    CHECK(angdroid_textblock_pager_clamp_start(5,5,6)==0);
    CHECK(angdroid_textblock_pager_clamp_start(5,5,2)==3);
    CHECK(angdroid_textblock_pager_clamp_start(1,5,2)==1);
    CHECK(angdroid_textblock_pager_clamp_start((size_t)-1,5,2)==3);
    CHECK(angdroid_textblock_pager_clamp_start((size_t)-1,(size_t)-1,1)
        == (size_t)-2);
    CHECK(angdroid_textblock_pager_clamp_start(0,(size_t)-1,1)==0);
    CHECK(angdroid_textblock_pager_content_rows(INT_MIN,1,NULL)==0);
    CHECK(angdroid_textblock_pager_content_rows(INT_MAX,0,NULL)==INT_MAX-2);
    printf("PASS: %u pager layout / scroll range assertions\n",checks);
    return 0;
}
