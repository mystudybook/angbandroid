#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/knowledge-column-text.h"

int main(void)
{
    char out[128];
    size_t cells;
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "治療藥水", 23, 29, 80);
    assert(cells == 6 && strcmp(out, "治療藥") == 0);
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "治療藥水", 23, 28, 80);
    assert(cells == 4 && strcmp(out, "治療") == 0);
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "治療藥水", 23, 46, 26);
    assert(cells == 2 && strcmp(out, "治") == 0);
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "治療", 23, 46, 24);
    assert(cells == 0 && out[0] == '\0');
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "治療", 23, 20, 80);
    assert(cells == 0 && out[0] == '\0');
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "治療", -1, 45, 80);
    assert(cells == 0 && out[0] == '\0');
    cells = angdroid_knowledge_clip_label(out, 3,
        "治療", 0, 20, 80);
    assert(cells == 0 && out[0] == '\0');
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        "Power", 23, 27, 80);
    assert(cells == 4 && strcmp(out, "Powe") == 0);
    cells = angdroid_knowledge_clip_label(NULL, 0,
        "治療", 0, 20, 80);
    assert(cells == 0);
    cells = angdroid_knowledge_clip_label(out, sizeof(out),
        NULL, 0, 20, 80);
    assert(cells == 0 && out[0] == '\0');
    assert(strcmp(angdroid_knowledge_tried_suffix(1), " {已試用}") == 0);
    assert(strcmp(angdroid_knowledge_tried_suffix(0), " {tried}") == 0);
    puts("PASS: knowledge item/rune UTF-8 column clipping and locale suffix");
    return 0;
}
