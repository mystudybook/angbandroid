/* Test the same pure recall viewport helpers used by FAangband. */
#include <limits.h>
#include <stdio.h>
#include "app/src/main/cpp/faangband/ui-recall-viewport.h"
static unsigned long long checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, \
    "FAIL line %d: %s\n", __LINE__, #x); return 1; } ++checks; } while (0)
int main(void)
{
    int w, h, n, selected, inward, event;
    struct angdroid_fa_recall_layout layout;
    CHECK(angdroid_fa_recall_bounds(INT_MAX,INT_MAX,INT_MAX).width==48);
    CHECK(angdroid_fa_recall_bounds(INT_MAX,INT_MAX,INT_MAX).page_rows==INT_MAX);
    for (w=0;w<=130;++w) for (h=0;h<=65;++h)
        for (n=-1;n<=75;++n) {
            int ok=w>=8&&h>=4&&n>0;
            layout=angdroid_fa_recall_bounds(w,h,n);
            CHECK(layout.col>=0&&layout.row==0);
            CHECK(layout.width>=0&&layout.page_rows>=0);
            if (!ok) {
                CHECK(layout.width==0&&layout.page_rows==0);
            } else {
                int cols=w<48?w:48;
                int rows=n>=h-2?h:n+2;
                CHECK(layout.width==cols);
                CHECK(layout.col==(w-cols)/2);
                CHECK(layout.page_rows==rows);
                CHECK(layout.col+layout.width<=w);
                CHECK(layout.page_rows<=h);
                CHECK(layout.width>4&&layout.page_rows>2);
            }
        }
    for (n=-1;n<=78;++n) for (selected=-3;selected<=80;++selected)
        for(inward=0;inward<=1;++inward) for(event=0;event<=1;++event) {
            int expected=inward&&event&&selected>=0&&n>=3&&selected<n-2;
            CHECK(!!angdroid_fa_recall_can_confirm(inward,event,selected,n)
                ==!!expected);
        }
    printf("PASS: %llu recall viewport C99 assertions\n",checks);
    return 0;
}
