/**
 * \file ui-menu.c
 * \brief Generic menu interaction functions
 *
 * Copyright (c) 2007 Pete Mack
 * Copyright (c) 2010 Andi Sidwell
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
#include "cave.h"
#include "ui-target.h"
#include "ui-event.h"
#include "ui-input.h"
#include "ui-menu.h"
#include "ui-menu-resize-guard.h"
#include "ui-menu-cell-text.h"
#include "ui-menu-key-guard.h"
#include "ui-menu-viewport.h"
#include "../angband/menu-hit-test.h"
#include "../angband/menu-column-width.h"
#include "../angband/menu-compact-layout.h"
#include "../angband/menu-valid-cursor.h"
#include "../angband/menu-visible-tags.h"
#include "../angband/menu-popup-layout.h"
#include "../curses/unicode_display.h"

#include "droid.h"

/* Draw by physical terminal cells. Term_putstr(n) counts characters,
 * which is unsafe for CJK/fullwidth names and long menu labels. */
static void fa_menu_put_clipped(uint8_t color, const char *source,
                                int row, int col, int limit)
{
    char clipped[512];

    if (!Term || !source) return;
    (void)angdroid_fa_menu_copy_visible(
        clipped, sizeof(clipped), source,
        Term->wid, Term->hgt, row, col, limit);
    if (clipped[0])
        c_put_str(color, clipped, row, col);
}

static void fa_menu_erase_clipped(int row, int col, int limit)
{
    int cells;
    if (!Term) return;
    cells = angdroid_fa_menu_visible_columns(
        Term->wid, Term->hgt, row, col, limit);
    if (cells > 0)
        Term_erase(col, row, cells);
}

/**
 * Cursor colours
 */
const uint8_t curs_attrs[2][2] =
{
	{ COLOUR_SLATE, COLOUR_BLUE },      /* Greyed row */
	{ COLOUR_WHITE, COLOUR_L_BLUE }     /* Valid row */
};

/**
 * Some useful constants
 */
const char lower_case[] = "abcdefghijklmnopqrstuvwxyz";
const char upper_case[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char all_letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char all_letters_nohjkl[] = "abcdefgimnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * Forward declarations
 */
static void display_menu_row(struct menu *menu, int pos, int top,
			     bool cursor, int row, int col, int width);
static bool menu_calc_size(struct menu *menu);
static bool is_valid_row(struct menu *menu, int cursor);
static bool no_valid_row(struct menu *menu, int count);


/**
 * Display an event, with possible preference overrides
 */
static void display_action_aux(menu_action *act, uint8_t color,
		int row, int col, int wid)
{
	/* TODO: add preference support */
	/* TODO: wizard mode should show more data */
	fa_menu_erase_clipped(row, col, wid);

	if (act->name)
		fa_menu_put_clipped(color, act->name, row, col, wid);
}

/* ------------------------------------------------------------------------
 * MN_ACTIONS HELPER FUNCTIONS
 *
 * MN_ACTIONS is the type of menu iterator that displays a simple list of
 * menu_actions.
 * ------------------------------------------------------------------------ */

static char menu_action_tag(struct menu *m, int oid)
{
	menu_action *acts = menu_priv(m);
	return acts[oid].tag;
}

static int menu_action_valid(struct menu *m, int oid)
{
	menu_action *acts = menu_priv(m);

	if (acts[oid].flags & MN_ACT_HIDDEN)
		return 2;

	return acts[oid].name ? true : false;
}

static void menu_action_display(struct menu *m, int oid, bool cursor, int row, int col, int width)
{
	menu_action *acts = menu_priv(m);
	uint8_t color = curs_attrs[!(acts[oid].flags & (MN_ACT_GRAYED))][0 != cursor];

	display_action_aux(&acts[oid], color, row, col, width);
}

static bool menu_action_handle(struct menu *m, const ui_event *event, int oid)
{
	menu_action *acts = menu_priv(m);

	if (event->type == EVT_SELECT) {
		if (!(acts->flags & MN_ACT_GRAYED) && acts[oid].action) {
			acts[oid].action(acts[oid].name, m->cursor);
			return true;
		}
	} else if (m->keys_hook && event->type == EVT_KBRD) {
		return m->keys_hook(m, event, oid);
	}

	return false;
}


/**
 * Virtual function table for action_events
 */
static const menu_iter menu_iter_actions =
{
	menu_action_tag,
	menu_action_valid,
	menu_action_display,
	menu_action_handle,
	NULL
};


/* ------------------------------------------------------------------------
 * MN_STRINGS HELPER FUNCTIONS
 *
 * MN_STRINGS is the type of menu iterator that displays a simple list of 
 * strings - an action is associated, but only for cmd_keys and switch_keys
 * handling via keys_hook as selection will just return the index.
 * ------------------------------------------------------------------------ */
static void display_string(struct menu *m, int oid, bool cursor,
		int row, int col, int width)
{
	const char **items = menu_priv(m);
	uint8_t color = curs_attrs[CURS_KNOWN][0 != cursor];
	fa_menu_put_clipped(color, items[oid], row, col, width);
}

static bool handle_string(struct menu *m, const ui_event *event, int oid)
{
	if (m->keys_hook && event->type == EVT_KBRD) {
		return m->keys_hook(m, event, oid);
	}
	return false;
}

/* Virtual function table for displaying arrays of strings */
static const menu_iter menu_iter_strings =
{ 
	NULL,              /* get_tag() */
	NULL,              /* valid_row() */
	display_string,    /* display_row() */
	handle_string, 	   /* row_handler() */
	NULL
};

/* ================== SKINS ============== */


/*** Scrolling menu ***/

/**
 * Find the position of a cursor given a screen address
 */
static int scrolling_get_cursor(int row, int col, int n, int top, region *loc)
{
	return angdroid_menu_scrolling_hit(
		row, loc->row, loc->page_rows, top, n);
}


/**
 * Display current view of a skin
 */
static void display_scrolling(struct menu *menu, int cursor, int *top, region *loc)
{
	int col = loc->col;
	int row = loc->row;
	int rows_per_page = loc->page_rows;
	int n = menu->filter_list ? menu->filter_count : menu->count;
	int i;

	/* Clamp both the cursor and scroll origin after a filter update or
	 * rotation. Do not let a stale top position render an empty final page. */
	*top = angdroid_fa_menu_clamp_top(*top, cursor, n, rows_per_page);

	for (i = 0; i < rows_per_page; i++) {
		/* Blank all lines */
		Term_erase(col, row + i, loc->width);
		if (*top < n && i < n - *top) {
			/* Only paint actual rows after scrolling; blank rows stay blank. */
			bool is_curs = (i == cursor - *top);
			display_menu_row(menu, i + *top, *top, is_curs, row + i, col,
							loc->width);
		}
	}

	if (cursor >= *top && cursor < n &&
		cursor - *top < rows_per_page &&
		menu->cursor_x_offset >= 0 &&
		menu->cursor_x_offset < loc->width)
		Term_gotoxy(col + menu->cursor_x_offset, row + cursor - *top);
}

static char scroll_get_tag(struct menu *menu, int pos)
{
	return angdroid_fa_menu_selection_key(menu->selections, pos);
}

static ui_event scroll_process_direction(struct menu *m, int dir)
{
	ui_event out = EVENT_EMPTY;

	/* Reject diagonals */
	if (ddx[dir] && ddy[dir])
		;

	/* Forward/back */
	else if (ddx[dir])
		out.type = ddx[dir] < 0 ? EVT_ESCAPE : EVT_SELECT;

	/* Move up or down to the next valid & visible row */
	else if (ddy[dir]) {
		m->cursor += ddy[dir];
		out.type = EVT_MOVE;
	}

	return out;
}

/**
 * Virtual function table for scrollable menu skin
 */
static const menu_skin menu_skin_scroll =
{
	scrolling_get_cursor,
	display_scrolling,
	scroll_get_tag,
	scroll_process_direction
};


/*** Object menu skin ***/

/**
 * Find the position of a cursor given a screen address
 */
static int object_skin_get_cursor(int row, int col, int n, int top, region *loc)
{
	return angdroid_menu_scrolling_hit(
		row, loc->row, loc->page_rows, top, n);
}


/**
 * Display current view of a skin
 */
static void object_skin_display(struct menu *menu, int cursor, int *top, region *loc)
{
	int col = loc->col;
	int row = loc->row;
	int rows_per_page = loc->page_rows;
	int n = menu->filter_list ? menu->filter_count : menu->count;
	int i;

	/* Clamp both the cursor and scroll origin after a filter update or
	 * rotation. Do not let a stale top position render an empty final page. */
	*top = angdroid_fa_menu_clamp_top(*top, cursor, n, rows_per_page);

	for (i = 0; i < rows_per_page; i++) {
		/* Blank all lines */
		Term_erase(col, row + i, loc->width);
		if (*top < n && i < n - *top) {
			/* Only paint actual rows after scrolling; blank rows stay blank. */
			bool is_curs = (i == cursor - *top);
			display_menu_row(menu, i + *top, *top, is_curs, row + i, col,
							loc->width);
		}
	}

	if (cursor >= *top && cursor < n &&
		cursor - *top < rows_per_page &&
		menu->cursor_x_offset >= 0 &&
		menu->cursor_x_offset < loc->width)
		Term_gotoxy(col + menu->cursor_x_offset, row + cursor - *top);
}

static char object_skin_get_tag(struct menu *menu, int pos)
{
	return angdroid_fa_menu_selection_key(menu->selections, pos);
}

static ui_event object_skin_process_direction(struct menu *m, int dir)
{
	ui_event out = EVENT_EMPTY;

	/* Reject diagonals */
	if (ddx[dir] && ddy[dir])
		;

	/* Prepare to switch menus */
	else if (ddx[dir]) {
		out.type = EVT_SWITCH;
		out.key.code = ddx[dir] < 0 ? ARROW_LEFT : ARROW_RIGHT;
	}

	/* Move up or down to the next valid & visible row */
	else if (ddy[dir]) {
		m->cursor += ddy[dir];
		out.type = EVT_MOVE;
	}

	return out;
}

/**
 * Virtual function table for object menu skin
 */
static const menu_skin menu_skin_object =
{
	object_skin_get_cursor,
	object_skin_display,
	object_skin_get_tag,
	object_skin_process_direction
};


/*** Multi-column menus ***/

static int columns_get_cursor(int row, int col, int n, int top, region *loc)
{
	int rows = loc->page_rows;
	int cols, colw;

	if (rows <= 0 || n <= 0 || loc->width <= 0) return -1;
	/* On a phone there is no room to interact with unreadable one-cell
	 * columns. Match the single scrolling layout used by drawing. */
	if (angdroid_menu_use_compact_columns(n, rows, loc->width, 8))
		return angdroid_menu_scrolling_hit(
			row, loc->row, rows, top, n);
	cols = 1 + (n - 1) / rows;
	colw = angdroid_menu_fit_column_width(23, loc->width, 0, cols);
	return angdroid_menu_columns_hit(
		row, col, loc->row, loc->col, rows, loc->width, colw, n);
}

static void display_columns(struct menu *menu, int cursor, int *top, region *loc)
{
	int c, r;
	int n = menu->filter_list ? menu->filter_count : menu->count;
	int col = loc->col, row = loc->row;
	int rows = loc->page_rows, cols, colw;

	if (rows <= 0 || loc->width <= 0) return;
	if (n > 0 &&
		angdroid_menu_use_compact_columns(n, rows, loc->width, 8)) {
		display_scrolling(menu, cursor, top, loc);
		return;
	}
	/* A changed filter or rotated viewport may leave previously painted
	 * columns behind. Wipe the full cell region before drawing the new set,
	 * even when the filtered view now contains zero rows. */
	for (r = 0; r < rows; ++r)
		fa_menu_erase_clipped(row + r, col, loc->width);
	if (n <= 0) return;
	cols = 1 + (n - 1) / rows;
	colw = angdroid_menu_fit_column_width(23, loc->width, 0, cols);
	for (c = 0; c < cols && c < loc->width / colw; c++) {
		for (r = 0; r < rows; r++) {
			int pos = c * rows + r;
			if (pos < n)
				display_menu_row(menu, pos, 0, pos == cursor,
					row + r, col + c * colw, colw);
		}
	}
	if (cursor >= 0 && cursor < n) {
		int dx = (cursor / rows) * colw + menu->cursor_x_offset;
		int dy = cursor % rows;
		if (dx >= 0 && dx < loc->width)
			Term_gotoxy(col + dx, row + dy);
	}
}

static char column_get_tag(struct menu *menu, int pos)
{
	return angdroid_fa_menu_selection_key(menu->selections, pos);
}

static ui_event column_process_direction(struct menu *m, int dir)
{
	ui_event out = EVENT_EMPTY;

	int n = m->filter_list ? m->filter_count : m->count;

	region *loc = &m->active;
	int rows_per_page = loc->page_rows;
	if (rows_per_page <= 0 || n <= 0) return out;
	if (angdroid_menu_use_compact_columns(
			n, rows_per_page, loc->width, 8))
		return scroll_process_direction(m, dir);

	if (ddx[dir])
		m->cursor += ddx[dir] * rows_per_page;
	if (ddy[dir])
		m->cursor += ddy[dir];

	/* Adjust to the correct locations (roughly) */
	if (m->cursor >= n)
		m->cursor = m->cursor % rows_per_page;
	else if (m->cursor < 0)
		m->cursor = n - 1;
	/* No column navigation may leave the selected position outside the
	 * available rows (including a partially filled final column). */
	m->cursor = angdroid_fa_menu_clamp_cursor(m->cursor, n);

	out.type = EVT_MOVE;
	return out;
}

/* Virtual function table for multi-column menu skin */
static const menu_skin menu_skin_column =
{
	columns_get_cursor,
	display_columns,
	column_get_tag,
	column_process_direction
};


/* Column skins show all shortcuts when columns fit. In compact portrait
 * mode the same skin falls back to a scrolling page, whose tags must be
 * relative to that page rather than to all inventory items. */
static bool fa_menu_relative_tags_paged(const struct menu *menu)
{
	int n = menu->filter_list ? menu->filter_count : menu->count;
	return menu->skin != &menu_skin_column ||
		angdroid_menu_use_compact_columns(
			n, menu->active.page_rows, menu->active.width, 8);
}

/* ================== GENERIC HELPER FUNCTIONS ============== */

static bool is_valid_row(struct menu *menu, int cursor)
{
	int count = menu->filter_list ? menu->filter_count : menu->count;
	int oid = angdroid_fa_menu_source_index(
		menu->filter_list, cursor, count, menu->count);

	if (oid < 0)
		return false;

	if (menu->row_funcs->valid_row)
		return menu->row_funcs->valid_row(menu, oid);

	return true;
}

/* Bridge the FA menu view to the bounded shared navigation helper.
 * A horizontal move has dy == 0, so looping by dy alone can hang. */
static int fa_menu_navigation_is_valid(void *context, int row)
{
	struct menu *menu = (struct menu *)context;
	int count = menu->filter_list ? menu->filter_count : menu->count;
	int oid = angdroid_fa_menu_source_index(
		menu->filter_list, row, count, menu->count);

	if (oid < 0) return false;
	/* Hidden choices may be invoked by an explicit tag, but they have
	 * no rendered row and must not become the arrow/page/Enter cursor. */
	return !menu->row_funcs->valid_row ||
		menu->row_funcs->valid_row(menu, oid) == MN_ROW_VALID;
}

static bool no_valid_row(struct menu *menu, int count)
{
	int i;
	for (i = 0; i < count; i++)
		if (fa_menu_navigation_is_valid(menu, i))
			return false;
	return true;
}

/* 
 * Return a new position in the menu based on the key
 * pressed and the flags and various handler functions.
 */
static int get_cursor_key(struct menu *menu, int top, struct keypress key)
{
	int i;
	int n = menu->filter_list ? menu->filter_count : menu->count;

	if (menu->flags & MN_CASELESS_TAGS)
		key.code = toupper((unsigned char) key.code);

	if ((menu->flags & MN_INSCRIP_TAGS) && menu->inscriptions &&
		isdigit((unsigned char)key.code) &&
		menu->inscriptions[D2I(key.code)])
		key.code = menu->inscriptions[D2I(key.code)];

	if (menu->flags & MN_NO_TAGS) {
		return -1;
	} else if (menu->flags & MN_REL_TAGS) {
		bool paged = fa_menu_relative_tags_paged(menu);
		int first = angdroid_menu_visible_tag_first(n, top, paged);
		int end = angdroid_menu_visible_tag_end(
			n, top, menu->active.page_rows, paged);
		for (i = first; i < end; i++) {
			char c;

			/* A relative shortcut is generated from a rendered row. Hidden
			 * rows have no visible tag, so accepting one here would conflict
			 * with both the screen and Android's soft-key hint. Explicit
			 * absolute/inscription tags still follow their original path. */
			if (!fa_menu_navigation_is_valid(menu, i)) continue;
			c = menu->skin->get_tag(menu, paged ? i - first : i);

			if ((menu->flags & MN_CASELESS_TAGS) && c)
				c = toupper((unsigned char) c);

			if (c && c == (char)key.code)
				return i;
		}
	} else if (!(menu->flags & MN_PVT_TAGS) && menu->selections) {
		for (i = 0; i < n && menu->selections[i]; i++) {
			char c = menu->selections[i];

			if (menu->flags & MN_CASELESS_TAGS)
				c = toupper((unsigned char) c);

			if (c == (char)key.code)
				return i;
		}
	} else if (menu->row_funcs->get_tag) {
		for (i = 0; i < n; i++) {
			int oid = angdroid_fa_menu_source_index(
				menu->filter_list, i, n, menu->count);
			char c;
			if (oid < 0) continue;
			c = menu->row_funcs->get_tag(menu, oid);

			if ((menu->flags & MN_CASELESS_TAGS) && c)
				c = toupper((unsigned char) c);

			if (c && c == (char)key.code)
				return i;
		}
	}

	return -1;
}

static menu_row_style_t menu_row_style_for_validity(menu_row_validity_t row_valid)
{
	menu_row_style_t style;

	switch (row_valid) {
		case MN_ROW_INVALID:
		case MN_ROW_HIDDEN:
			style = MN_ROW_STYLE_DISABLED;
			break;
		case MN_ROW_VALID:
		default:
			style = MN_ROW_STYLE_ENABLED;
			break;
	}

	return style;
}

/**
 * Modal display of menu
 */
static void display_menu_row(struct menu *menu, int pos, int top,
                             bool cursor, int row, int col, int width)
{
	int flags = menu->flags;
	char sel = 0;
	int count = menu->filter_list ? menu->filter_count : menu->count;
	int oid = angdroid_fa_menu_source_index(
		menu->filter_list, pos, count, menu->count);
	menu_row_validity_t row_valid = MN_ROW_VALID;

	/* Each row is a physical cell region. Never forward a negative
	 * or overlong width to the row skin after a terminal resize. */
	if (!Term) return;
	width = angdroid_fa_menu_visible_columns(
		Term->wid, Term->hgt, row, col, width);
	if (width <= 0 || oid < 0) return;

	if (menu->row_funcs->valid_row)
		row_valid = menu->row_funcs->valid_row(menu, oid);

	if (row_valid == MN_ROW_HIDDEN)
		return;

	if (!(flags & MN_NO_TAGS)) {
		if (flags & MN_REL_TAGS)
			sel = menu->skin->get_tag(menu,
				fa_menu_relative_tags_paged(menu) ? pos - top : pos);
		else if (menu->selections && !(flags & MN_PVT_TAGS))
			sel = angdroid_fa_menu_selection_key(menu->selections, pos);
		else if (menu->row_funcs->get_tag)
			sel = menu->row_funcs->get_tag(menu, oid);
	}

	if (sel) {
		menu_row_style_t style = menu_row_style_for_validity(row_valid);
		uint8_t color = curs_attrs[style][0 != (cursor)];
		/* With fewer than three cells, show only the selection letter.
		 * Do not send zero/negative widths to the underlying row skin. */
		if (width <= 3) {
			fa_menu_put_clipped(color, format("%c) ", sel),
				row, col, width);
			return;
		}
		fa_menu_put_clipped(color, format("%c) ", sel),
			row, col, 3);
		col += 3;
		width -= 3;
	}

	menu->row_funcs->display_row(menu, oid, cursor, row, col, width);
}

void menu_refresh(struct menu *menu, bool reset_screen)
{
	int oid = menu->cursor;
	region *loc = &menu->active;

	/* A resize can leave a previously valid region partly off-screen.
	 * Never invoke the item skin with an invalid rectangle. */
	if (!Term || !angdroid_fa_menu_region_visible(
		Term->wid, Term->hgt, loc->col, loc->row,
		loc->width, loc->page_rows))
		return;

	if (reset_screen) {
		screen_load();
		screen_save();
	}

	if (menu->browse_hook)
		oid = angdroid_fa_menu_source_index(menu->filter_list,
			menu->cursor,
			menu->filter_list ? menu->filter_count : menu->count,
			menu->count);

	if (menu->title)
		fa_menu_put_clipped(COLOUR_WHITE, menu->title,
			menu->boundary.row, menu->boundary.col, loc->width);

	if (menu->header)
		fa_menu_put_clipped(COLOUR_WHITE, menu->header,
			loc->row - 1, loc->col, loc->width);

	if (menu->prompt)
		fa_menu_put_clipped(COLOUR_WHITE, menu->prompt,
			loc->row + loc->page_rows, menu->boundary.col, loc->width);

	if (menu->browse_hook && oid >= 0)
		menu->browse_hook(oid, menu->menu_data, loc);

	menu->skin->display_list(menu, menu->cursor, &menu->top, loc);
}


/*** MENU RUNNING AND INPUT HANDLING CODE ***/

/**
 * Handle mouse input in a menu.
 * 
 * Mouse output is either moving, selecting, escaping, or nothing.  Returns
 * true if something changes as a result of the click.
 */
bool menu_handle_mouse(struct menu *menu, const ui_event *in,
		ui_event *out)
{
	int new_cursor;

	if (in->mouse.button == 2) {
		out->type = EVT_ESCAPE;
	} else if (!region_inside(&menu->active, in)) {
		/* A click to the left of the active region is 'back' */
		if (!region_inside(&menu->active, in)
				&& in->mouse.x < menu->active.col) {
			out->type = EVT_ESCAPE;
		} else if (menu->context_hook) {
			return (*menu->context_hook)(menu, in, out);
		}
	} else {
		int count = menu->filter_list ? menu->filter_count : menu->count;

		new_cursor = menu->skin->get_cursor(in->mouse.y, in->mouse.x,
				count, menu->top, &menu->active);
	
		/* Hidden rows can be selected by an explicit tag, but a tap on
		 * blank/hidden screen cells must not select them. */
		if (fa_menu_navigation_is_valid(menu, new_cursor)) {
			if (new_cursor == menu->cursor || !(menu->flags & MN_DBL_TAP))
				out->type = EVT_SELECT;
			else
				out->type = EVT_MOVE;

			menu->cursor = new_cursor;
		} else if (menu->context_hook) {
			return (*menu->context_hook)(menu, in, out);
		}
	}

	return out->type != EVT_NONE;
}


/**
 * Handle any menu command keys / SELECT events.
 *
 * Returns true if the key was handled at all (including if it's not handled
 * and just ignored).
 */
static bool menu_handle_action(struct menu *m, const ui_event *in)
{
	if (m->row_funcs->row_handler) {
		int oid = angdroid_fa_menu_source_index(
			m->filter_list, m->cursor,
			m->filter_list ? m->filter_count : m->count, m->count);
		if (oid < 0) return false;
		return m->row_funcs->row_handler(m, in, oid);
	}

	return false;
}


/**
 * Handle navigation keypresses.
 *
 * Returns true if they key was intelligible as navigation, regardless of
 * whether any action was taken.
 */
bool menu_handle_keypress(struct menu *menu, const ui_event *in,
		ui_event *out)
{
	bool eat = false;
	int count = menu->filter_list ? menu->filter_count : menu->count;
	int new_cursor = get_cursor_key(menu, menu->top, in->key);

	/* Explicit tag selection may target an offscreen row, but only if the
	 * filtered/source index still maps to a valid item. */
	if (new_cursor >= 0 && is_valid_row(menu, new_cursor)) {
		if (!(menu->flags & MN_DBL_TAP) || new_cursor == menu->cursor)
			out->type = EVT_SELECT;
		else
			out->type = EVT_MOVE;
		menu->cursor = new_cursor;
	} else if (in->key.code == ESCAPE) {
		out->type = EVT_ESCAPE;
	} else if (count <= 0) {
		eat = true;
	} else if (in->key.code == ' ') {
		/* Reaching count - 1 is legal. The former >= count - 1 check
		 * jumped over the last item and even allowed disabled rows. */
		int target = angdroid_menu_page_next_cursor(
			menu->cursor, count, menu->active.page_rows);
		int next = target < 0 ? -1 : angdroid_menu_find_valid_cursor(
			target, count, 1, menu, fa_menu_navigation_is_valid);
		if (next >= 0) {
			menu->cursor = next;
			menu->top = angdroid_fa_menu_clamp_top(
				next, next, count, menu->active.page_rows);
			out->type = EVT_MOVE;
		} else {
			eat = true;
		}
	} else if (in->key.code == KC_ENTER) {
		/* A filtered list may become empty after a stock change. Enter
		 * must not act on a stale or invalid selected item. */
		if (fa_menu_navigation_is_valid(menu, menu->cursor))
			out->type = EVT_SELECT;
		else
			eat = true;
	} else {
		int dir = target_dir(in->key);
		if (dir && !no_valid_row(menu, count)) {
			int previous = menu->cursor;
			*out = menu->skin->process_dir(menu, dir);
			if (out->type == EVT_MOVE) {
				int direction = ddy[dir] ? ddy[dir] : ddx[dir];
				int next = angdroid_menu_find_valid_cursor(
					menu->cursor, count, direction,
					menu, fa_menu_navigation_is_valid);
				if (next >= 0) {
					menu->cursor = next;
				} else {
					menu->cursor = previous;
					out->type = EVT_NONE;
				}
			}
		}
	}
	return eat;
}

static void keys_to_ui(struct menu *menu)
{
	char buf[256] = "";
	size_t used = 0;
	size_t tag_capacity;
	int cursor;
	int n = menu->filter_list ? menu->filter_count : menu->count;

	/* Numeric inscription aliases are separate from the visible tags. */
	if (menu->inscriptions) {
		for (cursor = 0; cursor < 10; cursor++) {
			if (menu->inscriptions[cursor])
				(void)angdroid_fa_menu_add_key(
					buf, sizeof(buf), &used, (char)('0' + cursor));
		}
	}

	/* Keep room at the end of the hint for fixed commands and
	 * screen switching even if thousands of rows have selectable tags. */
	{
		size_t reserved = 0, remaining = sizeof(buf) - used - 1;
		if (menu->cmd_keys) {
			size_t nkeys = strlen(menu->cmd_keys);
			reserved = nkeys < remaining ? nkeys : remaining;
		}
		remaining -= reserved;
		if (menu->switch_keys) {
			size_t nkeys = strlen(menu->switch_keys);
			reserved += nkeys < remaining ? nkeys : remaining;
		}
		/* buf has one NUL slot. The tag loop must leave fixed keys room. */
		tag_capacity = sizeof(buf) - reserved;
	}

	/* A filtered view changes the actual row index on *each* iteration.
	 * The old code always read filter_list[0] and wrote temp[256]
	 * without a capacity check. A crowded inventory could corrupt memory
	 * while updating the Android soft keyboard. */
	for (cursor = 0; cursor < n; cursor++) {
		int oid = angdroid_fa_menu_source_index(
			menu->filter_list, cursor, n, menu->count);
		char sel = 0;
		/* One visibility rule for the Android hint, tap target, keyboard
		 * arrows and auto-generated relative shortcuts. A hidden entry
		 * remains available only through a deliberate explicit tag. */
		if (oid < 0 || !fa_menu_navigation_is_valid(menu, cursor))
			continue;
		if (!(menu->flags & MN_NO_TAGS)) {
			if (menu->flags & MN_REL_TAGS) {
				bool paged = fa_menu_relative_tags_paged(menu);
				int first = angdroid_menu_visible_tag_first(
					n, menu->top, paged);
				int end = angdroid_menu_visible_tag_end(
					n, menu->top, menu->active.page_rows, paged);
				if (cursor < first || cursor >= end) continue;
				sel = menu->skin->get_tag(menu,
					paged ? cursor - first : cursor);
			} else if (menu->selections &&
				!(menu->flags & MN_PVT_TAGS)) {
				sel = angdroid_fa_menu_selection_key(
					menu->selections, cursor);
			} else if (menu->row_funcs->get_tag) {
				sel = menu->row_funcs->get_tag(menu, oid);
			}
		}
		if (sel && !angdroid_fa_menu_add_key(
			buf, tag_capacity, &used, sel))
			break;
	}

	/* Preserve existing command/switch keys. my_strcat() bounds the
	 * remaining space; never use a separate unbounded temporary array. */
	if (menu->cmd_keys)
		my_strcat(buf, menu->cmd_keys, sizeof(buf));
	if (menu->switch_keys)
		my_strcat(buf, menu->switch_keys, sizeof(buf));

	strdeldup(buf);
	/* Hints for the current filtered page replace old modal/lingering keys.
	 * An empty page must actively clear the previous Android shortcut bar. */
	soft_kbd_replace(buf);
}

/**
 * Run a menu.
 *
 * If popup is true, the screen is saved before the menu is drawn, and
 * restored afterwards. Each time a popup menu is redrawn, it resets the
 * screen before redrawing.
 */
ui_event menu_select(struct menu *menu, int notify, bool popup)
{
	ui_event in = EVENT_EMPTY;
	bool no_act = (menu->flags & MN_NO_ACTION) ? true : false;

	/* The initial layout can already be empty when an Android phone is
	 * extremely narrow. Do not assert, render, or enter a dead input loop. */
	if (!Term || !angdroid_fa_menu_region_visible(
		Term->wid, Term->hgt,
		menu->active.col, menu->active.row,
		menu->active.width, menu->active.page_rows)) {
		in.type = EVT_ESCAPE;
		return in;
	}

	notify |= (EVT_SELECT | EVT_ESCAPE | EVT_SWITCH);
	if (popup)
		screen_save();

	menu_ensure_cursor_valid(menu);
	/* Stop on first unhandled event */
	while (!(in.type & notify)) {
		ui_event out = EVENT_EMPTY;
		int cursor = menu->cursor;

		menu_refresh(menu, popup);
        
        keys_to_ui(menu);
        
		in = inkey_ex();

		/* Handle mouse & keyboard commands */
		if (in.type == EVT_MOUSE) {
			if (!no_act && menu_handle_action(menu, &in)) {
				continue;
			}
			menu_handle_mouse(menu, &in, &out);
		} else if (in.type == EVT_KBRD) {
			/* Command key */
			if (!no_act && menu->cmd_keys &&
				strchr(menu->cmd_keys, (char)in.key.code) &&
				menu_handle_action(menu, &in))
				continue;

			/* Switch key */
			if (!no_act && menu->switch_keys &&
				strchr(menu->switch_keys, (char)in.key.code)) {
				menu_handle_action(menu, &in);
				if (popup)
					screen_load();
				return in;
			}

			menu_handle_keypress(menu, &in, &out);
		} else if (in.type == EVT_RESIZE) {
			menu_calc_size(menu);
			if (menu->row_funcs->resize)
				menu->row_funcs->resize(menu);
			menu_ensure_cursor_valid(menu);

			/* Repaint even when the cursor has not moved. The store,
			 * equipment, inventory and generic menus share this loop.
			 * If the new viewport cannot contain one valid menu row,
			 * return cancellation safely instead of using stale bounds. */
			if (!Term || !angdroid_fa_menu_region_visible(
				Term->wid, Term->hgt,
				menu->active.col, menu->active.row,
				menu->active.width, menu->active.page_rows)) {
				if (popup) screen_load();
				in.type = EVT_ESCAPE;
				return in;
			}
			menu_refresh(menu, popup);
		}

		/* Redraw menu here if cursor has moved */
		if (cursor != menu->cursor) {
			menu_refresh(menu, popup);
		}

		/* If we've selected an item, then send that event out */
		if (out.type == EVT_SELECT && !no_act && menu_handle_action(menu, &out))
			continue;

		/* Notify about the outgoing type */
		if (notify & out.type) {
			if (popup)
				screen_load();
			return out;
		}
	}

	if (popup)
		screen_load();
	return in;
}


/* ================== MENU ACCESSORS ================ */

/**
 * Return the menu iter struct for a given iter ID.
 */
const menu_iter *menu_find_iter(menu_iter_id id)
{
	switch (id)
	{
		case MN_ITER_ACTIONS:
			return &menu_iter_actions;

		case MN_ITER_STRINGS:
			return &menu_iter_strings;
	}

	return NULL;
}

/*
 * Return the skin behaviour struct for a given skin ID.
 */
static const menu_skin *menu_find_skin(skin_id id)
{
	switch (id)
	{
		case MN_SKIN_SCROLL:
			return &menu_skin_scroll;

		case MN_SKIN_OBJECT:
			return &menu_skin_object;

		case MN_SKIN_COLUMNS:
			return &menu_skin_column;
	}

	return NULL;
}


void menu_set_filter(struct menu *menu, const int filter_list[], int n)
{
	menu->filter_list = filter_list;
	menu->filter_count = n;

	menu_ensure_cursor_valid(menu);
}

void menu_release_filter(struct menu *menu)
{
	menu->filter_list = NULL;
	menu->filter_count = 0;

	menu_ensure_cursor_valid(menu);

}

void menu_ensure_cursor_valid(struct menu *m)
{
	int count = m->filter_list ? m->filter_count : m->count;
	int row, start, found = -1;

	if (count <= 0) {
		m->cursor = -1;
		m->top = 0;
		return;
	}
	start = angdroid_fa_menu_clamp_cursor(m->cursor, count);
	/* Preserve the selection if valid; otherwise try the following
	 * visible rows before wrapping. No negative cursor enters a callback. */
	for (row = start; row < count; row++)
		if (fa_menu_navigation_is_valid(m, row)) { found = row; break; }
	if (found < 0)
		for (row = 0; row < start; row++)
			if (fa_menu_navigation_is_valid(m, row)) { found = row; break; }
	m->cursor = found;
	m->top = found < 0 ? 0 : angdroid_fa_menu_clamp_top(
		m->top, found, count, m->active.page_rows);
}

/* ======================== MENU INITIALIZATION ==================== */

static bool menu_calc_size(struct menu *menu)
{
	/* Calculate term-relative positions */
	menu->active = region_calculate(menu->boundary);

	if (menu->title) {
		menu->active.row += 2;
		menu->active.page_rows -= 2;
		/* Four title-indent cells must also be removed from the active
		 * width, otherwise the item skin draws past the screen's right edge. */
		menu->active.col += 4;
		menu->active.width = menu->active.width > 4 ?
			menu->active.width - 4 : 0;
	}

	if (menu->header) {
		menu->active.row++;
		menu->active.page_rows--;
	}

	if (menu->prompt) {
		if (menu->active.page_rows > 1) {
			menu->active.page_rows--;
		} else {
			/* The prompt is rendered in terminal cells, not UTF-8 bytes.
			 * A long Chinese prompt must never create negative widths. */
			size_t cells = angdroid_utf8_display_columns(menu->prompt);
			if (menu->active.width <= 2 ||
				cells >= (size_t)(menu->active.width - 2)) {
				menu->active.width = 0;
			} else {
				int offset = (int)cells + 2;
				menu->active.col += offset;
				menu->active.width -= offset;
			}
		}
	}

	return (menu->active.width > 0 && menu->active.page_rows > 0);
}

bool menu_layout(struct menu *m, const region *loc)
{
	m->boundary = *loc;
	return menu_calc_size(m);
}

void menu_setpriv(struct menu *menu, int count, void *data)
{
	menu->count = count;
	menu->menu_data = data;

	menu_ensure_cursor_valid(menu);
}

void *menu_priv(struct menu *menu)
{
	return menu->menu_data;
}

void menu_init(struct menu *menu, skin_id id, const menu_iter *iter)
{
	const menu_skin *skin = menu_find_skin(id);
	assert(skin && "menu skin not found!");
	assert(iter && "menu iter not found!");

	/* Wipe the struct */
	memset(menu, 0, sizeof *menu);

	/* Menu-specific initialisation */
	menu->row_funcs = iter;
	menu->skin = skin;
	menu->cursor = 0;
	menu->cursor_x_offset = 0;
}

struct menu *menu_new(skin_id id, const menu_iter *iter)
{
	struct menu *m = mem_alloc(sizeof *m);
	menu_init(m, id, iter);
	return m;
}

struct menu *menu_new_action(menu_action *acts, size_t n)
{
	struct menu *m = menu_new(MN_SKIN_SCROLL, menu_find_iter(MN_ITER_ACTIONS));
	menu_setpriv(m, n, acts);
	return m;
}

void menu_free(struct menu *m)
{
	mem_free(m);
}

void menu_set_cursor_x_offset(struct menu *m, int offset)
{
	/* This value is used in the menu skin's display_list() function. */
	m->cursor_x_offset = offset;
}

/*** Dynamic menu handling ***/

struct menu_entry {
	char *text;
	int value;
	menu_row_validity_t valid;

	struct menu_entry *next;
};

static int dynamic_valid(struct menu *m, int oid)
{
	struct menu_entry *entry;

	for (entry = menu_priv(m); oid; oid--) {
		entry = entry->next;
		assert(entry);
	}

	return entry->valid;
}

static void dynamic_display(struct menu *m, int oid, bool cursor,
		int row, int col, int width)
{
	struct menu_entry *entry;
	uint8_t color = curs_attrs[MN_ROW_STYLE_ENABLED][0 != cursor];

	/* Hack? While row_funcs is private, we need to be consistent with what the menu will do. */
	if (m->row_funcs->valid_row) {
		menu_row_validity_t row_valid = m->row_funcs->valid_row(m, oid);
		menu_row_style_t style = menu_row_style_for_validity(row_valid);
		color = curs_attrs[style][0 != cursor];
	}

	for (entry = menu_priv(m); oid; oid--) {
		entry = entry->next;
		assert(entry);
	}

	fa_menu_put_clipped(color, entry->text, row, col, width);
}

static const menu_iter dynamic_iter = {
	NULL,	/* tag */
	dynamic_valid,
	dynamic_display,
	NULL,	/* handler */
	NULL	/* resize */
};

struct menu *menu_dynamic_new(void)
{
	struct menu *m = menu_new(MN_SKIN_SCROLL, &dynamic_iter);
	menu_setpriv(m, 0, NULL);
	return m;
}

void menu_dynamic_add_valid(struct menu *m, const char *text, int value, menu_row_validity_t valid)
{
	struct menu_entry *head = menu_priv(m);
	struct menu_entry *new = mem_zalloc(sizeof *new);

	assert(m->row_funcs == &dynamic_iter);

	new->text = string_make(text);
	new->value = value;
	new->valid = valid;

	if (head) {
		struct menu_entry *tail = head;
		while (1) {
			if (tail->next)
				tail = tail->next;
			else
				break;
		}

		tail->next = new;
		menu_setpriv(m, m->count + 1, head);
	} else {
		menu_setpriv(m, m->count + 1, new);
	}
}

void menu_dynamic_add(struct menu *m, const char *text, int value)
{
	menu_dynamic_add_valid(m, text, value, MN_ROW_VALID);
}

void menu_dynamic_add_label_valid(struct menu *m, const char *text, const char label, int value, char *label_list, menu_row_validity_t valid)
{
	if (label && m->selections && (m->selections == label_list)) {
		label_list[m->count] = label;
	}
	menu_dynamic_add_valid(m,text,value, valid);
}

void menu_dynamic_add_label(struct menu *m, const char *text, const char label, int value, char *label_list)
{
	menu_dynamic_add_label_valid(m, text, label, value, label_list, MN_ROW_VALID);
}

size_t menu_dynamic_longest_entry(struct menu *m)
{
	size_t biggest = 0;
	size_t current;

	struct menu_entry *entry;

	for (entry = menu_priv(m); entry; entry = entry->next) {
		current = entry->text ?
			angdroid_utf8_display_columns(entry->text) : 0;
		if (current > biggest)
			biggest = current;
	}

	return biggest;
}

void menu_dynamic_calc_location(struct menu *m, int mx, int my)
{
	region r = {0, 0, 0, 0};
	angdroid_menu_popup_box popup;

	/* Reuse the bounded popup layout from Angband: FAangband also needs
	 * CJK cell widths, long-list paging and safe post-rotation anchors. */
	if (!Term || !angdroid_menu_popup_bounds(
		Term->wid, Term->hgt, mx, my, menu_dynamic_longest_entry(m),
		m->count, &popup)) {
		/* Never pass a zero-size region to region_calculate(), where zero
		 * means fill the remaining screen. menu_select() cancels safely. */
		m->boundary = r;
		m->boundary.width = Term && Term->wid > 0 ? 1 : 0;
		m->boundary.page_rows = Term && Term->hgt > 0 ? 1 : 0;
		m->active = r;
		return;
	}
	r.col = popup.col;
	r.row = popup.row;
	r.width = popup.width;
	r.page_rows = popup.page_rows;
	menu_layout(m, &r);
}

int menu_dynamic_select(struct menu *m)
{
	ui_event e = menu_select(m, 0, true);
	struct menu_entry *entry;
	int cursor;

	/* A filtered cursor is a view position, not a linked-list index.
	 * Never return a stale row after filtering or an empty popup. */
	if (e.type != EVT_SELECT || !is_valid_row(m, m->cursor))
		return -1;
	cursor = angdroid_fa_menu_source_index(
		m->filter_list, m->cursor,
		m->filter_list ? m->filter_count : m->count, m->count);
	if (cursor < 0) return -1;

	entry = menu_priv(m);
	while (cursor-- > 0) {
		if (!entry) return -1;
		entry = entry->next;
	}
	return entry ? entry->value : -1;
}

void menu_dynamic_free(struct menu *m)
{
	struct menu_entry *entry = menu_priv(m);
	while (entry) {
		struct menu_entry *next = entry->next;
		string_free(entry->text);
		mem_free(entry);
		entry = next;
	}
	mem_free(m);
}

