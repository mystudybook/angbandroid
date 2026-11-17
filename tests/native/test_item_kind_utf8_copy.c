/* Original item IDs stay canonical; only Chinese display copies are clipped. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/angband/zh-tw-items.h"
#include "../../app/src/main/cpp/angband/zh-tw-flavor-names.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"

int main(void)
{
    char small[7], tiny[3], out[32], one = 'X';
    const char *known = angdroid_zh_tw_item_kind_name("& Dagger~");
    const char *unknown_appearance = angdroid_zh_tw_flavor_name("Ruby");

    assert(known && strcmp(known, "匕首") == 0);
    assert(unknown_appearance && strcmp(unknown_appearance, "紅寶石") == 0);
    assert(angdroid_zh_tw_item_kind_name("unknown kind") == NULL);
    assert(angdroid_zh_tw_flavor_name("unknown flavor") == NULL);

    assert(angdroid_utf8_copy_complete(out, sizeof(out), known) ==
        strlen(known));
    assert(strcmp(out, "匕首") == 0);
    assert(angdroid_utf8_copy_complete(small, sizeof(small), known) ==
        strlen("匕首"));
    assert(strcmp(small, "匕首") == 0);
    assert(angdroid_utf8_copy_complete(small, 5, known) ==
        strlen("匕"));
    assert(strcmp(small, "匕") == 0);
    assert(angdroid_utf8_copy_complete(tiny, sizeof(tiny), known) == 0);
    assert(tiny[0] == '\0');

    assert(angdroid_utf8_copy_complete(small, sizeof(small),
        unknown_appearance) == strlen("紅寶"));
    assert(strcmp(small, "紅寶") == 0);
    assert(angdroid_utf8_copy_complete(small, 4, unknown_appearance) ==
        strlen("紅"));
    assert(strcmp(small, "紅") == 0);
    assert(angdroid_utf8_copy_complete(small, 1, unknown_appearance) == 0);
    assert(!small[0]);
    assert(angdroid_utf8_copy_complete(&one, 0, known) == 0);
    assert(one == 'X');

    puts("PASS: canonical item kind/flavor mapping and complete UTF-8 display copies");
    return 0;
}
