/**
 * \file ui-recall.c
 * \brief Recall point selection menu
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * Copyright (c) 2019 Nick McConnell
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
#include "game-input.h"
#include "game-world.h"
#include "init.h"
#include "ui-menu.h"
#include "ui-output.h"
#include "ui-recall.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/menu-text-fit.h"
#include "ui-recall-viewport.h"
#include <stdint.h>

/**
 * Choice of recall point (or nowhere)
 */
static int selection;

/**
 * Number of actual recall points
 */
static int num_recall_points;

static char recall_tag(struct menu *menu, int oid)
{
	return I2A(oid);
}

/**
 * Display an entry on the recall menu
 */
static void recall_display(struct menu *menu, int oid, bool cursor, int row,
                    int col, int width)
{
    const uint16_t *choice = menu_priv(menu);
    int idx = choice[oid];
    const char *place;
    char visible[256];
    uint8_t attr = cursor ? COLOUR_L_BLUE : COLOUR_WHITE;

    if (!Term || row < 0 || row >= Term->hgt ||
        col < 0 || col >= Term->wid || width <= 0) return;

    if (idx < num_recall_points) {
        int locality = world->levels[player->recall[idx]].locality;
        int level = world->levels[player->recall[idx]].depth;
        if (level)
            place = format("%s %d   ", locality_name(locality), level);
        else if (locality != LOC_ARENA)
            place = format("%s %s   ", locality_name(locality),
                angdroid_zh_tw_enabled() ? "城鎮" : "Town");
        else
            place = angdroid_zh_tw_enabled() ?
                "尚未設定地點" : "No location set     ";
    } else {
        place = angdroid_zh_tw_enabled() ?
            "不取代傳送點" : "Don't replace     ";
    }

    /* Skip the old 96-byte intermediary: CJK locality names must be
     * clipped by visible columns, never halfway through a UTF-8 scalar. */
    angdroid_menu_copy_columns(visible, sizeof(visible), place,
        (size_t)MIN(width, Term->wid - col));
    if (visible[0]) c_put_str(attr, visible, row, col);
}

/**
 * Deal with events on the recall menu
 */
static bool recall_action(struct menu *menu, const ui_event *e, int oid)
{
	uint16_t *choice = menu_priv(menu);
	if (e->type == EVT_SELECT) {
		selection = choice[oid];
	}

	return false;
}


/**
 * Display list of recall points, pick one or decline.
 */
int textui_get_recall_point(bool inward, int num_points, int num_poss)
{
    menu_iter menu_f = { recall_tag, 0, recall_display, recall_action, 0 };
    struct menu *m;
    struct angdroid_fa_recall_layout bounds;
    region area;
    ui_event evt = { 0 };
    int cursor = 0;
    int num_entries, i;
    uint16_t *choice;

    /* This menu has a title that consumes two terminal rows. */
    if (!Term || num_points < 0 ||
        num_points > (int)UINT16_MAX - 2 ||
        Term->wid < 8 || Term->hgt < 4)
        return -1;
    if (inward && num_points < num_poss)
        num_points++;
    num_entries = inward ? num_points + 1 : num_points;
    if (num_entries <= 0) return -1;

    bounds = angdroid_fa_recall_bounds(Term->wid, Term->hgt, num_entries);
    if (bounds.width == 0 || bounds.page_rows == 0) return -1;
    area.col = bounds.col;
    area.row = bounds.row;
    area.width = bounds.width;
    area.page_rows = bounds.page_rows;
    num_recall_points = num_points;

    m = menu_new(MN_SKIN_SCROLL, &menu_f);
    choice = mem_zalloc(num_entries * sizeof(uint16_t));
    for (i = 0; i < num_entries; i++)
        choice[i] = i;

    selection = 0;
    if (inward)
        m->title = angdroid_zh_tw_enabled() ?
            "要取代哪個傳送點？（ESC 取消）" :
            "Which recall point will you replace (or ESC):";
    else
        m->title = angdroid_zh_tw_enabled() ?
            "要前往哪個傳送點？" :
            "Which recall point do you want to go to?";
    menu_setpriv(m, num_entries, choice);
    menu_layout(m, &area);
    evt = menu_select(m, cursor, true);

    /* Re-check both the event and selected index after every menu event.
     * Escape and the no-replace choice must not dereference recall[]. */
    while (angdroid_fa_recall_can_confirm(inward,
            evt.type == EVT_SELECT, selection, num_entries) &&
            player->recall[selection] != player->place) {
        int locality = world->levels[player->recall[selection]].locality;
        int level = world->levels[player->recall[selection]].depth;
        const char *full_place = format("%s %d", locality_name(locality), level);
        char visible_place[256];
        bool compact = Term && Term->wid < 48;
        bool zh = angdroid_zh_tw_enabled();
        int text_budget = Term ? Term->wid - (compact ? 9 : 30) : 0;

        angdroid_menu_copy_columns(visible_place, sizeof(visible_place),
            full_place, (size_t)MAX(0, text_budget));
        if (get_check(compact ?
                (zh ? format("取代 %s？", visible_place) :
                    format("Replace %s?", visible_place)) :
                (zh ? format("確定要取代傳送點 %s？", visible_place) :
                    format("Really replace recall point %s", visible_place))))
            break;
        if (get_check(zh ? "取消傳送？" : "Cancel recall? ")) {
            evt.type = EVT_ESCAPE;
            break;
        }
        evt = menu_select(m, cursor, true);
    }

    mem_free(choice);
    mem_free(m);
    return (evt.type == EVT_SELECT) ? selection : -1;
}
