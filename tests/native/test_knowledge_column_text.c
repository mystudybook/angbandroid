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
    /* An unusually long Chinese artifact name still belongs entirely
     * to the display layer; the list edge, not the raw UTF-8 byte count,
     * determines how many complete glyphs may be drawn. */
    {
        const char *long_artifact =
            "失落王國傳說中的古老不朽巨龍之王所佩戴的終極魔法神器";
        char clipped[256];
        char short_buffer[9];
        size_t columns;

        columns = angdroid_knowledge_clip_label(clipped, sizeof clipped,
            long_artifact, 18, 26, 80);
        assert(columns == 8);
        assert(strcmp(clipped, "失落王國") == 0);

        columns = angdroid_knowledge_clip_label(clipped, sizeof clipped,
            long_artifact, 18, 80, 80);
        assert(columns == angdroid_utf8_display_columns(long_artifact));
        assert(strcmp(clipped, long_artifact) == 0);

        columns = angdroid_knowledge_clip_label(short_buffer,
            sizeof short_buffer, long_artifact, 18, 80, 80);
        assert(columns == 4);
        assert(strcmp(short_buffer, "失落") == 0);

        columns = angdroid_knowledge_clip_label(clipped, sizeof clipped,
            long_artifact, 18, 18, 80);
        assert(columns == 0 && clipped[0] == '\0');
        columns = angdroid_knowledge_clip_label(clipped, sizeof clipped,
            long_artifact, 18, 80, 18);
        assert(columns == 0 && clipped[0] == '\0');
        columns = angdroid_knowledge_clip_label(clipped, sizeof clipped,
            long_artifact, 18, 80, 19);
        assert(columns == 0 && clipped[0] == '\0');

        /* The full translated title may remain long while the output
         * is clipped to an even-width CJK terminal cell budget. */
        assert(angdroid_utf8_display_columns(long_artifact) > 20);
    }
    puts("PASS: knowledge item/rune UTF-8 column clipping and locale suffix");
    return 0;
}
