/* Real FAangband source-matched lore flowed into terminal-cell textblock rows. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "../../app/src/main/cpp/angband/textblock-column-layout.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"
#include "../../app/src/main/cpp/angband/zh-tw-faangband-monster-flavor.h"
static unsigned checks;
#define CHECK(x) do {assert(x);++checks;} while(0)
static void test_rows(const char *utf8, size_t width)
{
    wchar_t wide[1024];
    size_t count=0, at=0, start=0, lines=0, painted=0;
    while (utf8[at]) {
        uint32_t cp=0;
        size_t n=angdroid_utf8_next_scalar(utf8+at,&cp);
        CHECK(n>=1 && cp!=0xFFFDu && count<1023);
        wide[count++]=(wchar_t)cp;
        at+=n;
    }
    wide[count]=0;
    while(start<count) {
        size_t j,cols=0;
        angdroid_textblock_line row=angdroid_textblock_next_line(
            wide,NULL,count,start,width);
        CHECK(row.next>start && row.next<=count);
        CHECK(row.length>0);
        for(j=0;j<row.length;++j) {
            cols += (size_t)angdroid_terminal_columns((uint32_t)wide[start+j]);
            ++painted;
        }
        CHECK(cols<=width);
        CHECK(row.next==start+row.length);
        start=row.next;
        ++lines;
    }
    CHECK(lines>=2);
    CHECK(painted==count);
}
int main(void)
{
    const char *src="Greatest and most terrible of the servants of Morgoth.  A sorcerer of dreadful powers, master of shadows and of phantoms, foul in wisdom, cruel in strength, misshaping what he rules, lord of werewolves; his dominion is torment.  He has many servants, and rarely fights without them.";
    const char *zh=angdroid_faangband_monster_flavor("Sauron, the Sorcerer",src);
    const char *changed="Changed upstream flavor text.";
    const char *complainer=angdroid_faangband_monster_flavor("The Complainer", "He has lots of problems, and seems to think you can fix them. ");
    CHECK(zh!=NULL && zh!=src && strcmp(zh,src)!=0);
    CHECK(complainer && strcmp(complainer,"他有數不完的煩惱，而且似乎覺得你能替他一一解決。") == 0);
    CHECK(angdroid_faangband_monster_flavor("Sauron, the Sorcerer",changed)==changed);
    CHECK(angdroid_faangband_monster_flavor("unknown", src)==NULL);
    test_rows(zh,12);
    test_rows(zh,20);
    test_rows(zh,40);
    test_rows(zh,80);
    puts("PASS: FA source-matched long CJK monster lore in 12/20/40/80-cell textblocks");
    printf("Assertions: %u\n",checks);
    return 0;
}
