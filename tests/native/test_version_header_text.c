/* Host C99 verification of the real version header formatter.
 * No Android runtime; the game version and copyright are display data only.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-version-header.h"

int main(void)
{
    char output[256];
    char tiny[5] = "old";
    char exact[48];
    char fits[49];
    char long_name[300];
    int checks = 0;

    assert(angdroid_version_header_text(output, sizeof(output),
                                       "4.2.5", false));
    assert(strcmp(output,
                  "You are playing 4.2.5.  Type '?' for more info.") == 0);
    ++checks;
    assert(angdroid_version_header_text(output, sizeof(output),
                                       "4.2.5", true));
    assert(strcmp(output, "正在遊玩 4.2.5。按 ? 查看詳細資訊。") == 0);
    ++checks;
    assert(angdroid_version_header_text(output, sizeof(output), "", true));
    assert(strcmp(output, "正在遊玩 。按 ? 查看詳細資訊。") == 0);
    ++checks;
    assert(angdroid_version_header_text(output, sizeof(output), NULL, false));
    assert(strcmp(output,
                  "You are playing .  Type '?' for more info.") == 0);
    ++checks;
    assert(angdroid_version_header_text(output, sizeof(output), "100% done", true));
    assert(strstr(output, "100% done") != NULL);
    ++checks;

    /* Entire UTF-8 suffix must fit or the result is empty. */
    assert(!angdroid_version_header_text(tiny, sizeof(tiny), "4.2.5", true));
    assert(tiny[0] == '\0');
    ++checks;
    assert(!angdroid_version_header_text(exact, sizeof(exact),
                                         "4.2.5", true));
    assert(exact[0] == '\0');
    ++checks;
    assert(angdroid_version_header_text(fits, sizeof(fits), "4.2.5", true));
    assert(strcmp(fits, "正在遊玩 4.2.5。按 ? 查看詳細資訊。") == 0);
    ++checks;

    memset(long_name, 'X', sizeof(long_name));
    long_name[sizeof(long_name) - 1] = '\0';
    assert(!angdroid_version_header_text(output, sizeof(output), long_name, false));
    assert(output[0] == '\0');
    ++checks;
    assert(!angdroid_version_header_text(NULL, sizeof(output), "4.2.5", false));
    ++checks;
    assert(!angdroid_version_header_text(output, 0, "4.2.5", true));
    ++checks;
    puts("PASS: bilingual bounded version header (11 cases)");
    return 0;
}
