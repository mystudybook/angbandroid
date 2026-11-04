#!/bin/sh
# Check wide-character UTF-8 transport using the host C compiler.
# No Android SDK/NDK is required for this standalone validation.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM
CC="${CC:-cc}"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_utf8.c" \
    -o "$BUILD_DIR/test_unicode_utf8"

"$BUILD_DIR/test_unicode_utf8"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_width.c" \
    -o "$BUILD_DIR/test_unicode_width"

"$BUILD_DIR/test_unicode_width"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_layout.c" \
    -o "$BUILD_DIR/test_unicode_layout"

"$BUILD_DIR/test_unicode_layout"

# CJK display columns differ from Unicode scalar counts.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_unicode_display.c" \
    -o "$BUILD_DIR/test_unicode_display"

"$BUILD_DIR/test_unicode_display"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_death_locale.c" \
    -o "$BUILD_DIR/test_death_locale"

"$BUILD_DIR/test_death_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_locale.c" \
    -o "$BUILD_DIR/test_monster_locale"

"$BUILD_DIR/test_monster_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_lore_locale.c" \
    -o "$BUILD_DIR/test_monster_lore_locale"
"$BUILD_DIR/test_monster_lore_locale"

# Named original Angband monster status/damage messages.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_messages_locale.c" \
    -o "$BUILD_DIR/test_monster_messages_locale"
"$BUILD_DIR/test_monster_messages_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_monster_pain_locale.c" \
    -o "$BUILD_DIR/test_monster_pain_locale"
"$BUILD_DIR/test_monster_pain_locale"

"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_item_locale.c" \
    -o "$BUILD_DIR/test_item_locale"

"$BUILD_DIR/test_item_locale"

# Native UI translations: compile without Android SDK or the game engine.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_game_ui_locale.c" \
    -o "$BUILD_DIR/test_game_ui_locale"
"$BUILD_DIR/test_game_ui_locale"

# Object information property and brand/slay labels.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic "$ROOT/tests/native/test_object_info_locale.c" -o "$BUILD_DIR/test_object_info_locale"
"$BUILD_DIR/test_object_info_locale"

# All source class.txt spell names, display-only C99 lookup and English fallback.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic "$ROOT/tests/native/test_spell_names_locale.c" -o "$BUILD_DIR/test_spell_names_locale"
"$BUILD_DIR/test_spell_names_locale"

# Original Angband class progression titles; game records unchanged.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic "$ROOT/tests/native/test_class_titles_locale.c" -o "$BUILD_DIR/test_class_titles_locale"
"$BUILD_DIR/test_class_titles_locale"

# Filtered menu soft-keyboard hints must use visible-row indexes and remain bounded.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_key_hints.c" \
    -o "$BUILD_DIR/test_menu_key_hints"
"$BUILD_DIR/test_menu_key_hints"

# Scrolling relative-tag input and Android keyboard hints must use the visible page.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_visible_tags.c" \
    -o "$BUILD_DIR/test_menu_visible_tags"
"$BUILD_DIR/test_menu_visible_tags"

# Horizontal movement across invalid menu rows must terminate.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_valid_cursor.c" \
    -o "$BUILD_DIR/test_menu_valid_cursor"
"$BUILD_DIR/test_menu_valid_cursor"

# Multi-column menu layout on tiny terminal widths must not divide by zero.
"$CC" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_menu_column_width.c" \
    -o "$BUILD_DIR/test_menu_column_width"
"$BUILD_DIR/test_menu_column_width"
