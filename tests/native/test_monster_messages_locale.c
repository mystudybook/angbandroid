/* C99 host test of original Angband MON_MSG enum mappings; no SDK. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

enum local_mon_messages {
#define MON_MSG(name, type, omit, english) MON_MSG_##name,
#include "../../app/src/main/cpp/angband/list-mon-message.h"
#undef MON_MSG
};

static const char *message_zh(int code)
{
    switch (code) {
#define ZH_MON_MSG(c, text) case MON_MSG_##c: return text;
#include "../../app/src/main/cpp/angband/zh-tw-mon-messages.h"
#undef ZH_MON_MSG
        default: return NULL;
    }
}

int main(void)
{
    int count = 0;
#define ZH_MON_MSG(c, text) do { \
    assert(message_zh(MON_MSG_##c) != NULL); \
    assert(strcmp(message_zh(MON_MSG_##c), text) == 0); \
    ++count; \
} while (0);
#include "../../app/src/main/cpp/angband/zh-tw-mon-messages.h"
#undef ZH_MON_MSG

    assert(count == 55);
    assert(strcmp(message_zh(MON_MSG_DIE), "死亡了。") == 0);
    assert(strcmp(message_zh(MON_MSG_FLEE_IN_TERROR),
                  "驚恐地逃跑了！") == 0);
    assert(strcmp(message_zh(MON_MSG_MORIA_DEATH),
                  "你聽到了痛苦的尖叫聲！") == 0);
    /* Pain messages come from race-specific text and remain English. */
    assert(message_zh(MON_MSG_95) == NULL);
    assert(message_zh(MON_MSG_0) == NULL);
    assert(message_zh(MON_MSG_MAX) == NULL);
    puts("PASS: 55 named monster event messages with pain fallback");
    return 0;
}
