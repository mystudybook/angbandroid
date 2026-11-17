/* C99 regression for clipping only genuine two-cell CJK stream text. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "../../app/src/main/cpp/curses/term-resize-cjk.h"

static unsigned long assertions;
#define CHECK(x) do { assert(x); ++assertions; } while (0)

static void crop_examples(void)
{
    wchar_t src[8] = {L'A', L'甲', ANGDROID_TEXT_CONTINUATION,
                      L'B', L'乙', ANGDROID_TEXT_CONTINUATION, L'Z', 0};
    wchar_t dst[8];
    int attrs[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    int d_attrs[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    memcpy(dst, src, sizeof(dst));
    angdroid_term_repair_resized_cjk_row(dst, d_attrs, src, attrs, 2, 7);
    CHECK(dst[0] == L'A' && dst[1] == L'?');
    CHECK(src[1] == L'甲' && src[2] == ANGDROID_TEXT_CONTINUATION);

    memcpy(dst, src, sizeof(dst));
    angdroid_term_repair_resized_cjk_row(dst, d_attrs, src, attrs, 3, 7);
    CHECK(dst[1] == L'甲' && dst[2] == ANGDROID_TEXT_CONTINUATION);

    memcpy(dst, src, sizeof(dst));
    angdroid_term_repair_resized_cjk_row(dst, d_attrs, src, attrs, 5, 7);
    CHECK(dst[4] == L'?');
    CHECK(dst[1] == L'甲' && dst[2] == ANGDROID_TEXT_CONTINUATION);

    /* The FA map path can draw one wide code point in one fixed grid. */
    src[2] = L'B';
    memcpy(dst, src, sizeof(dst));
    angdroid_term_repair_resized_cjk_row(dst, d_attrs, src, attrs, 2, 7);
    CHECK(dst[1] == L'甲'); /* No matching source marker: do not replace */

    src[2] = ANGDROID_TEXT_CONTINUATION;
    attrs[1] = 0x80; /* Tile attribute: not CJK prose */
    memcpy(dst, src, sizeof(dst));
    angdroid_term_repair_resized_cjk_row(dst, d_attrs, src, attrs, 2, 7);
    CHECK(dst[1] == L'甲');

    /* An orphan marker is never exposed in either cell layer. */
    src[0] = ANGDROID_TEXT_CONTINUATION;
    src[1] = L'B';
    src[2] = ANGDROID_TEXT_CONTINUATION;
    memcpy(dst, src, sizeof(dst));
    angdroid_term_repair_resized_cjk_row(dst, d_attrs, src, attrs, 4, 7);
    CHECK(dst[0] == L' ' && dst[2] == L' ');
}

static void width_matrix(void)
{
    int width;
    for (width = 1; width <= 32; ++width) {
        wchar_t src[34], dst[34], terrain[34];
        int attrs[34], original_attrs[34];
        int i;
        for (i = 0; i < 34; ++i) {
            src[i] = L'.';
            dst[i] = L'.';
            terrain[i] = L'.';
            attrs[i] = original_attrs[i] = 1;
        }
        src[width - 1] = L'漢';
        src[width] = ANGDROID_TEXT_CONTINUATION;
        for (i = 0; i < width; ++i) {
            dst[i] = src[i];
            terrain[i] = src[i];
        }
        angdroid_term_repair_resized_cjk_row(
            dst, attrs, src, original_attrs, width, width + 1);
        angdroid_term_repair_resized_cjk_row(
            terrain, attrs, src, original_attrs, width, width + 1);
        CHECK(dst[width - 1] == L'?');
        CHECK(terrain[width - 1] == L'?');
        CHECK(src[width - 1] == L'漢');
        CHECK(src[width] == ANGDROID_TEXT_CONTINUATION);
        for (i = 0; i + 1 < width; ++i) {
            CHECK(dst[i] == L'.');
            CHECK(terrain[i] == L'.');
        }

        /* When both cells still fit, the pair must remain untouched. */
        dst[width - 1] = src[width - 1];
        dst[width] = src[width];
        angdroid_term_repair_resized_cjk_row(
            dst, attrs, src, original_attrs, width + 1, width + 1);
        CHECK(dst[width - 1] == L'漢');
        CHECK(dst[width] == ANGDROID_TEXT_CONTINUATION);
    }
}

static void invalid_arguments(void)
{
    wchar_t src[2] = {L'甲', ANGDROID_TEXT_CONTINUATION};
    wchar_t dst[2] = {L'甲', ANGDROID_TEXT_CONTINUATION};
    int attrs[2] = {1, 1};
    angdroid_term_repair_resized_cjk_row(dst, attrs, src, attrs, 0, 2);
    CHECK(dst[0] == L'甲');
    angdroid_term_repair_resized_cjk_row(dst, attrs, src, attrs, 2, 1);
    CHECK(dst[0] == L'甲');
    angdroid_term_repair_resized_cjk_row(NULL, attrs, src, attrs, 1, 2);
    angdroid_term_repair_resized_cjk_row(dst, NULL, src, attrs, 1, 2);
    angdroid_term_repair_resized_cjk_row(dst, attrs, NULL, attrs, 1, 2);
    angdroid_term_repair_resized_cjk_row(dst, attrs, src, NULL, 1, 2);
    CHECK(dst[0] == L'甲');
}

int main(void)
{
    crop_examples();
    width_matrix();
    invalid_arguments();
    printf("PASS: native CJK resize crop (%lu assertions)\n", assertions);
    return 0;
}
