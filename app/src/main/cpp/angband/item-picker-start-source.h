/* Choose a useful starting tab for a native item picker.
 * An allowed source can still be empty (for example while browsing with
 * allow_all); prefer an eligible nonempty tab without changing command rules.
 * The four source IDs are supplied by the caller: no game headers needed.
 */
#ifndef ANGDROID_ITEM_PICKER_START_SOURCE_H
#define ANGDROID_ITEM_PICKER_START_SOURCE_H

#define ANGDROID_PICKER_INVEN  (1u << 0)
#define ANGDROID_PICKER_EQUIP  (1u << 1)
#define ANGDROID_PICKER_QUIVER (1u << 2)
#define ANGDROID_PICKER_FLOOR  (1u << 3)

typedef struct {
    int current;
    int source[4];
    unsigned int allowed;
    unsigned int nonempty;
    unsigned int requested;
    int prefer_quiver;
} angdroid_item_picker_start_options;

static inline int angdroid_item_picker_initial_source(
    const angdroid_item_picker_start_options *options)
{
    unsigned int selectable, mask;
    int i;

    if (!options) return 0;
    selectable = options->allowed & options->nonempty & 0x0fu;

    /* A previously selected tab is stable only if it has actual items.
     * This avoids showing an empty pack before a nonempty equipment tab. */
    if (selectable) {
        for (i = 0; i < 4; ++i) {
            mask = 1u << i;
            if ((selectable & mask) &&
                options->current == options->source[i])
                return options->current;
        }

        /* Honor the existing quiver-tag priority for relevant commands. */
        if (options->prefer_quiver &&
            (selectable & ANGDROID_PICKER_QUIVER))
            return options->source[2];

        /* Prefer an explicitly requested item source before another tab. */
        for (i = 0; i < 4; ++i)
            if ((selectable & options->requested) & (1u << i))
                return options->source[i];
        for (i = 0; i < 4; ++i)
            if (selectable & (1u << i))
                return options->source[i];
    }

    /* No valid items anywhere: retain the old empty-page browsing
     * behavior, so the user can still use available tab switch keys. */
    for (i = 0; i < 4; ++i)
        if ((options->allowed & (1u << i)) &&
            options->current == options->source[i])
            return options->current;

    if (options->prefer_quiver &&
        (options->allowed & ANGDROID_PICKER_QUIVER))
        return options->source[2];

    for (i = 0; i < 4; ++i)
        if ((options->allowed & options->requested) & (1u << i))
            return options->source[i];

    for (i = 0; i < 4; ++i)
        if (options->allowed & (1u << i))
            return options->source[i];

    return options->source[0];
}

#endif /* ANGDROID_ITEM_PICKER_START_SOURCE_H */
