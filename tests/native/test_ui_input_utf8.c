/* Host C99: UTF-8 deletion and CJK cursor rendering boundaries. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/ui-input-utf8.h"

int main(void)
{
    char text[] = "龍A貓B";
    char short_name[] = "中文";
    char invalid[] = { (char)0xE4, 'Z', '\0' };
    char not_terminated[] = { 'A', 'B', 'C' };
    char *at;
    size_t len;

    assert(angdroid_input_cursor_columns(text, 0) == 0);
    assert(angdroid_input_cursor_columns(text, 1) == 2);
    assert(angdroid_input_cursor_columns(text, 2) == 3);
    assert(angdroid_input_cursor_columns(text, 3) == 5);
    assert(angdroid_input_cursor_columns(text, 4) == 6);
    assert(angdroid_input_cursor_columns(text, 999) == 6);
    assert(angdroid_input_cursor_columns("A🐱B", 2) == 3);
    assert(angdroid_input_cursor_columns(NULL, 3) == 0);

    at = text + strlen("龍A");
    assert(strcmp(at, "貓B") == 0);
    assert(strcmp(angdroid_input_next_codepoint(at), "B") == 0);
    len = strlen(text);
    memmove(at, angdroid_input_next_codepoint(at),
        len - (size_t)(angdroid_input_next_codepoint(at) - text));
    text[len - strlen("貓")] = '\0';
    assert(strcmp(text, "龍AB") == 0);
    assert(angdroid_input_next_codepoint(NULL) == NULL);
    assert(angdroid_input_next_codepoint(text + strlen(text)) == NULL);
    assert(angdroid_input_next_codepoint(invalid) == invalid + 1);

    assert(angdroid_input_prefill_fits(short_name, sizeof(short_name)));
    assert(angdroid_input_prefill_fits(short_name, 7));
    assert(!angdroid_input_prefill_fits(short_name, 6));
    assert(!angdroid_input_prefill_fits(not_terminated, sizeof(not_terminated)));
    assert(!angdroid_input_prefill_fits(NULL, 8));
    assert(!angdroid_input_prefill_fits(short_name, 0));
    puts("PASS: UTF-8 delete span, CJK cursor cells and lossless default text");
    return 0;
}
