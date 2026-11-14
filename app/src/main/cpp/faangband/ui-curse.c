/**
 * \file ui-curse.c
 * \brief Curse selection menu
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * Copyright (c) 2016 Nick McConnell
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
#include "init.h"
#include "obj-curse.h"
#include "obj-knowledge.h"
#include "ui-curse.h"
#include "ui-menu.h"
#include "ui-output.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/menu-text-fit.h"

static int selection;

/* The same localized label is used to measure and display a curse entry. */
static void fa_curse_label(char *dest, size_t size, const char *name, int power)
{
	if (angdroid_zh_tw_enabled())
		strnfmt(dest, size, "  %s（詛咒強度 %d）", name, power);
	else
		strnfmt(dest, size, "  %s (curse strength %d)", name, power);
}

struct curse_menu_data {
	int index;
	int power;
};

/**
 * Display an entry on the item menu
 */
static void get_curse_display(struct menu *menu, int oid, bool cursor, int row,
					  int col, int width)
{
	struct curse_menu_data *choice = menu_priv(menu);
	int attr = cursor ? COLOUR_L_BLUE : COLOUR_WHITE;
	char buf[256], visible[256];
	int power = choice[oid].power;
	const char *name = curses[choice[oid].index].name;

	fa_curse_label(buf, sizeof(buf), name, power);
	/* Clip to both the menu cell and the remaining terminal columns. */
	if (!Term || row < 0 || row >= Term->hgt ||
		col < 0 || col >= Term->wid || width <= 0) return;
	angdroid_menu_copy_columns(visible, sizeof(visible), buf,
		(size_t)MIN(width, Term->wid - col));
	if (visible[0])
		c_put_str(attr, visible, row, col);
}

/**
 * Deal with events on the get_item menu
 */
static bool get_curse_action(struct menu *menu, const ui_event *event, int oid)
{
	struct curse_menu_data *choice = menu_priv(menu);
	if (event->type == EVT_SELECT) {
		selection = choice[oid].index;
	}

	return false;
}

/**
 * Show spell long description when browsing
 */
static void curse_menu_browser(int oid, void *data, const region *loc)
{
	struct curse_menu_data *choice = data;
	char buf[80];

	/* Redirect output to the screen */
	text_out_hook = text_out_to_screen;
	text_out_wrap = 0;
	text_out_indent = loc->col - 1;
	text_out_pad = 1;

	Term_gotoxy(loc->col, loc->row + loc->page_rows);
	my_strcpy(buf, curses[choice[oid].index].desc, sizeof(buf));
	my_strcap(buf);
	text_out(" %s.\n", buf);

	/* XXX */
	text_out_pad = 0;
	text_out_indent = 0;
}

/**
 * Display list of curses to choose from
 */
static int curse_menu(struct object *obj, char *dice_string)
{
	menu_iter menu_f = { 0, 0, get_curse_display, get_curse_action, 0 };
	struct menu *m = menu_new(MN_SKIN_SCROLL, &menu_f);
	char header[160], visible_header[160];
	int row;
	size_t length = 0;
	size_t header_columns;
	size_t requested_width;
	int max_width;
	int i, count = 0;
	size_t array_size = z_info->curse_max * sizeof(struct curse_menu_data);
	struct curse_menu_data *available = mem_zalloc(array_size);
	static region area = { 20, 1, -1, -2 };

	/* Count and then list the curses */
	for (i = 1; i < z_info->curse_max; i++) {
		if ((obj->known->curses[i].power > 0) &&
			(obj->known->curses[i].power < 100) &&
			player_knows_curse(player, i)) {
			char label[256];
			available[count].index = i;
			available[count].power = obj->curses[i].power;
			fa_curse_label(label, sizeof(label), curses[i].name,
				available[count].power);
			length = MAX(length, angdroid_utf8_display_columns(label));
			count++;
		}
	}
	if (!count) {
		mem_free(available);
		return 0;
	}

	/* Set up the menu */
	menu_setpriv(m, count, available);
	if (angdroid_zh_tw_enabled())
		strnfmt(header, sizeof(header),
			"要移除哪種詛咒（法術強度 %s）？", dice_string);
	else
		strnfmt(header, sizeof(header),
			" Remove which curse (spell strength %s)?", dice_string);
	m->header = header;
	m->selections = all_letters_nohjkl;
	m->flags = (MN_PVT_TAGS);
	m->browse_hook = curse_menu_browser;

	/* Set up the item list variables */
	selection = 0;

	/* Set up the menu region */
	area.page_rows = m->count + 2;
	area.row = 1;
	header_columns = angdroid_utf8_display_columns(header);
	requested_width = MAX(length + 1, header_columns);
	/* Clamp before centering: never mix unsigned length and screen width. */
	max_width = MAX(1, Term->wid - 1);
	if (requested_width > (size_t)max_width)
		requested_width = (size_t)max_width;
	area.width = (int)requested_width;
	area.col = (Term->wid - area.width) / 2;
	if (area.col <= 3)
		area.col = 0;
	angdroid_menu_copy_columns(visible_header, sizeof(visible_header),
		header, (size_t)area.width);
	m->header = visible_header;

	for (row = area.row; row < area.row + area.page_rows &&
			row < Term->hgt; row++)
		prt("", row, MAX(0, area.col - 1));

	menu_layout(m, &area);

	/* Choose */
	menu_select(m, 0, true);

	/* Clean up */
	mem_free(available);
	mem_free(m);

	/* Result */
	return selection;
}

bool textui_get_curse(int *choice, struct object *obj, char *dice_string)
{
	int curse = curse_menu(obj, dice_string);
	if (curse) {
		*choice = curse;
		return true;
	}
	return false;
}
