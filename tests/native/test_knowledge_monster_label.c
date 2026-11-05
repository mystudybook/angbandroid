/* Knowledge browser must keep unique names and clip whole CJK glyphs. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/knowledge-monster-label.h"
int main(void)
{
    char label[128];
    size_t cells;
    assert(strcmp(angdroid_knowledge_monster_label("kobold", 0, 1),
                  "狗頭人") == 0);
    assert(strcmp(angdroid_knowledge_monster_label("kobold", 0, 0),
                  "kobold") == 0);
    assert(strcmp(angdroid_knowledge_monster_label("kobold", 1, 1),
                  "kobold") == 0);
    /* Unique names are localized only for UI and retain English fallback. */
    assert(strcmp(angdroid_knowledge_monster_label(
        "Morgoth, Lord of Darkness", 1, 1),
        "魔苟斯，黑暗之王") == 0);
    assert(strcmp(angdroid_knowledge_monster_label(
        "Sauron, the Sorcerer", 1, 1),
        "索倫，黑暗巫師") == 0);
    assert(strcmp(angdroid_knowledge_monster_label(
        "Sauron, the Sorcerer", 1, 0),
        "Sauron, the Sorcerer") == 0);
    assert(strcmp(angdroid_knowledge_monster_label(
        "Unknown named unique", 1, 1),
        "Unknown named unique") == 0);
    cells = angdroid_knowledge_monster_copy(label, sizeof(label),
        "Morgoth, Lord of Darkness", 1, 1, 5);
    assert(cells == 4 && strcmp(label, "魔苟") == 0);
    assert(strcmp(angdroid_knowledge_monster_label("ancient red dragon", 0, 1),
                  "遠古紅龍") == 0);
    assert(strcmp(angdroid_knowledge_monster_label("unknown creature", 0, 1),
                  "unknown creature") == 0);
    assert(angdroid_knowledge_monster_label(NULL, 0, 1) == NULL);
    cells = angdroid_knowledge_monster_copy(label, sizeof(label),
        "ancient red dragon", 0, 1, 6);
    assert(cells == 6 && strcmp(label, "遠古紅") == 0);
    cells = angdroid_knowledge_monster_copy(label, sizeof(label),
        "ancient red dragon", 0, 1, 5);
    assert(cells == 4 && strcmp(label, "遠古") == 0);
    cells = angdroid_knowledge_monster_copy(label, sizeof(label),
        "ancient red dragon", 0, 1, 1);
    assert(cells == 0 && label[0] == '\0');
    cells = angdroid_knowledge_monster_copy(label, sizeof(label),
        "ancient red dragon", 0, 1, -1);
    assert(cells == 0 && label[0] == '\0');
    cells = angdroid_knowledge_monster_copy(label, sizeof(label),
        "kobold", 1, 1, 4);
    assert(cells == 4 && strcmp(label, "kobo") == 0);
    cells = angdroid_knowledge_monster_copy(label, 3,
        "ancient red dragon", 0, 1, 20);
    assert(cells == 0 && label[0] == '\0'); /* no partial UTF-8 */
    cells = angdroid_knowledge_monster_copy(NULL, 0,
        "kobold", 0, 1, 20);
    assert(cells == 0);
    puts("PASS: knowledge monster labels, uniques and UTF-8 column clipping");
    return 0;
}
