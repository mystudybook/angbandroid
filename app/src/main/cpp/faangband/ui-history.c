/**
 * \file ui-history.c
 * \brief Character auto-history display UI
 *
 * Copyright (c) 2007 J.D. White
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "game-world.h"
#include "player-history.h"
#include "ui-history.h"
#include "ui-input.h"
#include "ui-output.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/menu-text-fit.h"
#include "ui-history-viewport.h"

/* All new output stays on-screen: dump_history() keeps its existing
 * English, byte-for-byte export format. */
static void fa_history_put_clipped(uint8_t color, const char *message,
    int row, int col)
{
    char visible[512];
    if (!Term || !message || row < 0 || row >= Term->hgt ||
        col < 0 || col >= Term->wid) return;
    angdroid_menu_copy_columns(visible, sizeof(visible), message,
        (size_t)(Term->wid - col));
    if (visible[0]) c_put_str(color, visible, row, col);
}

/* Append full UTF-8 scalars only.  Never cut a dynamic event or place
 * half-way through an encoded Chinese character. */
static void fa_history_append(char *dest, size_t cap, size_t *used_columns,
    size_t until_column, const char *text)
{
    size_t bytes;
    if (!text || !dest || !used_columns || *used_columns >= until_column)
        return;
    bytes = strlen(dest);
    if (bytes >= cap) return;
    *used_columns += angdroid_menu_copy_columns(dest + bytes,
        cap - bytes, text, until_column - *used_columns);
}

static void print_history_header(void)
{
    fa_history_put_clipped(COLOUR_WHITE, angdroid_zh_tw_enabled() ?
        "[角色歷史紀錄]" : "[Player history]", 0, 0);
    fa_history_put_clipped(COLOUR_L_BLUE, angdroid_zh_tw_enabled() ?
        "      回合   地點               事件" :
        "      Turn   Place               Note", 1, 0);
}

static void fa_history_screen_row(const struct history_info *event,
    int row, int width)
{
    char visible[512] = { 0 };
    char turn_text[48], padding[21];
    const char *place = level_name(&world->levels[event->place]);
    size_t used = 0;
    size_t limit = (size_t)width;
    size_t place_width = (size_t)angdroid_fa_history_place_columns(width);
    size_t place_cells = place ?
        angdroid_utf8_display_columns(place) : 0;

    if (width < 60)
        strnfmt(turn_text, sizeof(turn_text), "%ld ",
            (long)event->turn);
    else
        strnfmt(turn_text, sizeof(turn_text), "%10ld",
            (long)event->turn);
    fa_history_append(visible, sizeof(visible), &used, limit, turn_text);
    if (width >= 60 && place_cells < 20) {
        size_t spaces = 20 - place_cells;
        memset(padding, ' ', spaces);
        padding[spaces] = '\0';
        fa_history_append(visible, sizeof(visible), &used, limit, padding);
    }

    /* A 20-29-column screen prioritizes the event, omitting location.
     * At 30-39 columns, retain a short place hint.  Show an artifact-loss
     * marker before the event on narrow screens so it cannot be clipped
     * away after an otherwise lengthy history message. */
    if (place_width > 0) {
        fa_history_append(visible, sizeof(visible), &used,
            MIN(limit, used + place_width), place ? place : "");
        fa_history_append(visible, sizeof(visible), &used, limit, "'  ");
    }
    if (hist_has(event->type, HIST_ARTIFACT_LOST) && width < 40)
        fa_history_append(visible, sizeof(visible), &used, limit,
            angdroid_zh_tw_enabled() ? "遺失：" : "LOST: ");
    fa_history_append(visible, sizeof(visible), &used, limit, event->event);
    if (hist_has(event->type, HIST_ARTIFACT_LOST) && width >= 40)
        fa_history_append(visible, sizeof(visible), &used, limit,
            angdroid_zh_tw_enabled() ? "（遺失）" : " (LOST)");
    fa_history_put_clipped(COLOUR_WHITE, visible, row, 0);
}

void history_display(void)
{
    struct history_info *history_list_local = NULL;
    size_t max_item = history_get_list(player, &history_list_local);
    static size_t first_item = 0;
    bool active = true;
    int wid, hgt;

    if (!Term) return;
    Term_get_size(&wid, &hgt);
    if (wid <= 0 || hgt <= 0) return;
    screen_save();

    while (active) {
        struct keypress ch;
        int page_size;
        int row;
        size_t i;

        /* Recalculate after every keypress or screen rotation. */
        if (!Term) break;
        Term_get_size(&wid, &hgt);
        if (wid <= 0 || hgt <= 0) break;
        page_size = angdroid_fa_history_page_rows(hgt);
        first_item = angdroid_fa_history_clamp_top(first_item, max_item);

        Term_clear();
        if (page_size > 0) {
            print_history_header();
            for (row = 0, i = first_item;
                 row < page_size && i < max_item; ++row, ++i) {
                fa_history_screen_row(&history_list_local[i], row + 2, wid);
            }
        } else {
            fa_history_put_clipped(COLOUR_WHITE,
                angdroid_zh_tw_enabled() ?
                "視窗高度不足，請旋轉螢幕。" :
                "Screen too short; rotate the device.", 0, 0);
        }
        /* A one-row terminal must not overwrite the rotate notice. */
        if (hgt > 1)
            fa_history_put_clipped(COLOUR_WHITE,
                angdroid_zh_tw_enabled() ?
                "[方向鍵捲動，p/PgUp 上頁，n/PgDn 下頁，ESC 離開]" :
                "[Arrow keys scroll, p/PgUp for previous page, n/PgDn for next page, ESC to exit.]",
                hgt - 1, 0);

        ch = inkey();
        switch (ch.code) {
            case 'n':
            case ' ':
            case KC_PGDOWN:
                first_item = angdroid_fa_history_step(first_item,
                    max_item, page_size, 1);
                break;
            case 'p':
            case KC_PGUP:
                first_item = angdroid_fa_history_step(first_item,
                    max_item, page_size, 0);
                break;
            case 'j':
            case ARROW_DOWN:
                first_item = angdroid_fa_history_step(first_item,
                    max_item, 1, 1);
                break;
            case 'k':
            case ARROW_UP:
                first_item = angdroid_fa_history_step(first_item,
                    max_item, 1, 0);
                break;
            case ESCAPE:
                active = false;
                break;
        }
    }

    /* A rotation may temporarily invalidate the active terminal. */
    if (Term) screen_load();
}

/**
 * Dump character history to a file, which we assume is already open.
 */
void dump_history(ang_file *file)
{
	struct history_info *history_list_local = NULL;
	size_t max_item = history_get_list(player, &history_list_local);
	size_t i;
	char buf[120];

	file_putf(file, "[Player history]\n");
	file_putf(file, "      Turn   Place               Note\n");

	for (i = 0; i < max_item; i++) {
		strnfmt(buf, sizeof(buf), "%10ld%20s\'  %s",
				(long)history_list_local[i].turn,
				level_name(&world->levels[history_list_local[i].place]),
				history_list_local[i].event);

		if (hist_has(history_list_local[i].type, HIST_ARTIFACT_LOST))
			my_strcat(buf, " (LOST)", sizeof(buf));

		file_putf(file, "%s", buf);
		file_put(file, "\n");
	}

	return;
}
