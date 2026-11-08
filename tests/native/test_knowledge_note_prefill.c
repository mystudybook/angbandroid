/* Host C99: preserve an existing note exactly, or reject the edit. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/knowledge-note-prefill.h"

int main(void)
{
    char note[80], small[7], one[1] = { 'X' };
    char fits[80], overlong[81], invalid[] = { (char)0xE4, 'X', '\0' };
    size_t i;
    assert(angdroid_knowledge_note_prefill(note, sizeof(note), "龍🐱 A"));
    assert(strcmp(note, "龍🐱 A") == 0);
    assert(angdroid_knowledge_note_prefill(small, sizeof(small), "龍"));
    assert(strcmp(small, "龍") == 0);
    assert(!angdroid_knowledge_note_prefill(small, sizeof(small), "龍龍"));
    assert(small[0] == '\0');

    for (i = 0; i < 79; ++i) fits[i] = 'a';
    fits[79] = '\0';
    assert(angdroid_knowledge_note_prefill(note, sizeof(note), fits));
    assert(strcmp(note, fits) == 0);
    for (i = 0; i < 80; ++i) overlong[i] = 'a';
    overlong[80] = '\0';
    assert(!angdroid_knowledge_note_prefill(note, sizeof(note), overlong));
    assert(note[0] == '\0');

    /* An incomplete 3-byte Han glyph must not enter the edit widget. */
    memset(fits, 'a', 77);
    memcpy(fits + 77, "龍", 2);
    fits[79] = '\0';
    assert(!angdroid_knowledge_note_prefill(note, sizeof(note), fits));
    assert(note[0] == '\0');

    assert(!angdroid_knowledge_note_prefill(note, sizeof(note), invalid));
    assert(note[0] == '\0');
    assert(angdroid_knowledge_note_prefill(note, sizeof(note), ""));
    assert(note[0] == '\0');
    assert(angdroid_knowledge_note_prefill(note, sizeof(note), NULL));
    assert(note[0] == '\0');
    assert(!angdroid_knowledge_note_prefill(NULL, sizeof(note), "龍"));
    assert(!angdroid_knowledge_note_prefill(one, 0, "龍"));
    assert(one[0] == 'X');
    puts("PASS: existing inscriptions are exact or editing is refused");
    return 0;
}
