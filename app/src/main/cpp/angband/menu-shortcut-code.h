/* Native keyboard codes can be Unicode scalars or special-key values.
 * Menu tags and shortcut lists contain single-byte labels only. Never
 * truncate a keycode before matching against those byte strings.
 */
#ifndef ANGDROID_MENU_SHORTCUT_CODE_H
#define ANGDROID_MENU_SHORTCUT_CODE_H

static inline int angdroid_menu_shortcut_is_byte(unsigned int code)
{
    return code > 0 && code <= 0xff;
}

#endif /* ANGDROID_MENU_SHORTCUT_CODE_H */
