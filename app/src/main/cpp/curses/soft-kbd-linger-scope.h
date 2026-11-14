/* Save a lingering parent prompt across an object picker's temporary keys.
 * Copy control sequences (including ${...} tokens) exactly, without de-dup.
 */
#ifndef ANGDROID_SOFT_KBD_LINGER_SCOPE_H
#define ANGDROID_SOFT_KBD_LINGER_SCOPE_H
#include <stddef.h>
#include <string.h>
typedef struct {
    char keys[256];
    int lingering;
} soft_kbd_linger_snapshot;
static inline void angdroid_soft_kbd_capture_linger(
    soft_kbd_linger_snapshot *saved, const char *keys, int flashing)
{
    size_t i = 0;
    if (!saved) return;
    saved->keys[0] = '\0';
    saved->lingering = 0;
    if (flashing || !keys || !keys[0]) return;
    while (i + 1 < sizeof(saved->keys) && keys[i]) {
        saved->keys[i] = keys[i];
        ++i;
    }
    if (keys[i]) { saved->keys[0] = '\0'; return; }
    saved->keys[i] = '\0';
    saved->lingering = 1;
}
/* Restore only on cancellation. Failure must not alter active keys. */
static inline int angdroid_soft_kbd_recover_linger(
    char *current, size_t capacity,
    const soft_kbd_linger_snapshot *saved, int cancelled)
{
    size_t length;
    if (!cancelled || !current || !saved || !saved->lingering) return 0;
    length = strlen(saved->keys);
    if (!length || length >= capacity) return 0;
    memcpy(current, saved->keys, length + 1);
    return 1;
}
#endif /* ANGDROID_SOFT_KBD_LINGER_SCOPE_H */
