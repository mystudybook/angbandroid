/**
 * \file ui-output.c
 * \brief Putting text on the screen, screen saving and loading, panel handling
 *
 * Copyright (c) 2007 Pete Mack and others.
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
#include "player-calcs.h"
#include "ui-input.h"
#include "ui-output.h"
#include "z-textblock.h"
#include "../angband/menu-text-fit.h"
#include "../angband/zh-tw-ui.h"
#include "../curses/unicode_layout.h"

/**
 * ------------------------------------------------------------------------
 * Regions
 * ------------------------------------------------------------------------ */

/**
 * These functions are used for manipulating regions on the screen, used 
 * mostly (but not exclusively) by the menu functions.
 */

region region_calculate(region loc)
{
	int w, h;
	Term_get_size(&w, &h);

	if (loc.col < 0)
		loc.col += w;
	if (loc.row < 0)
		loc.row += h;
	if (loc.width <= 0)
		loc.width += w - loc.col;
	if (loc.page_rows <= 0)
		loc.page_rows += h - loc.row;

	return loc;
}

void region_erase_bordered(const region *loc)
{
	region calc = region_calculate(*loc);
	int i = 0;

	calc.col = MAX(calc.col - 1, 0);
	calc.row = MAX(calc.row - 1, 0);
	calc.width += 2;
	calc.page_rows += 2;

	for (i = 0; i < calc.page_rows; i++)
		Term_erase(calc.col, calc.row + i, calc.width);
}

void region_erase(const region *loc)
{
	region calc = region_calculate(*loc);
	int i = 0;

	for (i = 0; i < calc.page_rows; i++)
		Term_erase(calc.col, calc.row + i, calc.width);
}

bool region_inside(const region *loc, const ui_event *key)
{
	if ((loc->col > key->mouse.x) || (loc->col + loc->width <= key->mouse.x))
		return false;

	if ((loc->row > key->mouse.y) ||
		(loc->row + loc->page_rows <= key->mouse.y))
		return false;

	return true;
}


/**
 * ------------------------------------------------------------------------
 * Text display
 * ------------------------------------------------------------------------ */

/**
 * These functions are designed to display large blocks of text on the screen
 * all at once.  They are the ui-term specific layer on top of the z-textblock.c
 * functions.
 */

/**
 * Utility function
 */
static void display_area(const wchar_t *text, const uint8_t *attrs,
        size_t *line_starts, size_t *line_lengths,
        size_t n_lines, region area, size_t line_from)
{
    size_t i, j, available;

    if (!Term || area.width <= 0 || area.page_rows <= 0 ||
            area.col < 0 || area.row < 0 ||
            area.col >= Term->wid || area.row >= Term->hgt)
        return;

    /* Never address lines before the first page, after the last page,
     * or beyond the terminal after rotation. Clear unused rows too,
     * so a short last page does not leave stale description text. */
    available = line_from < n_lines ? n_lines - line_from : 0;
    if (!text || !attrs || !line_starts || !line_lengths) available = 0;
    for (i = 0; i < (size_t)area.page_rows &&
            area.row + (int)i < Term->hgt; ++i) {
        size_t cells = 0;
        Term_erase(area.col, area.row + (int)i, area.width);
        if (i >= available) continue;
        for (j = 0; j < line_lengths[line_from + i]; ++j) {
            uint32_t cp = (uint32_t)text[line_starts[line_from + i] + j];
            size_t width = (size_t)angdroid_terminal_columns(cp);
            if (width > (size_t)area.width - cells) {
                /* A lone wide glyph is not drawable in a 1-cell area.
                 * Show an explicit placeholder rather than losing a
                 * complete line with no indication of its content. */
                if (cells == 0 && width > (size_t)area.width)
                    Term_putch(area.col, area.row + (int)i,
                        attrs[line_starts[line_from + i] + j], L'?');
                break;
            }
            Term_putch(area.col + (int)cells, area.row + (int)i,
                attrs[line_starts[line_from + i] + j],
                text[line_starts[line_from + i] + j]);
            cells += width;
        }
    }
}

/**
 * Plonk a textblock on the screen in a certain bounding box.
 */
void textui_textblock_place(textblock *tb, region orig_area, const char *header)
{
    size_t *line_starts = NULL, *line_lengths = NULL;
    size_t n_lines = 0;
    region area;
    int wid, hgt;

    if (!Term) return;
    Term_get_size(&wid, &hgt);
    if (wid <= 1 || hgt <= 0) return;
    area = region_calculate(orig_area);
    if (area.col < 0) area.col = 0;
    if (area.row < 0) area.row = 0;
    if (area.col >= wid - 1 || area.row >= hgt) return;
    area.width = MAX(0, MIN(area.width, wid - area.col - 1));
    area.page_rows = MAX(0, MIN(area.page_rows, hgt - area.row));
    if (!area.width || !area.page_rows) return;

    if (header && area.page_rows > 1) {
        char visible[256];
        angdroid_menu_copy_columns(visible, sizeof(visible), header,
            (size_t)area.width);
        Term_erase(area.col, area.row, area.width);
        c_put_str(COLOUR_L_BLUE, visible, area.row, area.col);
        ++area.row;
        --area.page_rows;
    }

    n_lines = textblock_calculate_display_lines(tb,
        &line_starts, &line_lengths, (size_t)area.width);
    display_area(textblock_text(tb), textblock_attrs(tb), line_starts,
        line_lengths, n_lines, area, 0);
    mem_free(line_starts);
    mem_free(line_lengths);
}

/**
 * Show a textblock interactively
 */
/* Use the same key normalization as inkey(), but allow the modal
 * textblock pager to receive EVT_RESIZE instead of swallowing it. */
static struct keypress fa_textblock_key_from_event(ui_event event)
{
    struct keypress key = KEYPRESS_NULL;

    if (event.type == EVT_KBRD || event.type == EVT_BUTTON) {
        key = event.key;
        key.type = EVT_KBRD;
    } else if (event.type == EVT_ESCAPE || event.type == EVT_MOUSE) {
        key.type = EVT_KBRD;
        key.code = (event.type == EVT_MOUSE && event.mouse.button == 1) ?
            '\n' : ESCAPE;
        key.mods = 0;
    }
    return key;
}

static void fa_textblock_print_bounded(
    const char *label, int row, int col, int columns)
{
    char visible[256];

    if (columns <= 0 || !Term || row < 0 || col < 0 ||
            row >= Term->hgt || col >= Term->wid)
        return;
    angdroid_menu_copy_columns(visible, sizeof(visible), label,
        (size_t)columns);
    c_put_str(COLOUR_L_BLUE, visible, row, col);
}

/* Show a long textblock with layout recalculated after window rotations.
 * At small heights, reserve body content before optional header/footer. */
struct keypress textui_textblock_show(textblock *tb, region orig_area, const char *header)
{
    size_t *line_starts = NULL, *line_lengths = NULL;
    size_t n_lines = 0, start_line = 0;
    struct keypress ch = KEYPRESS_NULL;
    int layout_width = -1;

    screen_save();
    for (;;) {
        int term_width, term_height, body_rows;
        region full, area;
        bool show_header, show_footer, pager;
        ui_event input;
        const char *footer;

        Term_get_size(&term_width, &term_height);
        /* A phone can briefly report an empty terminal during rotation.
         * Do not calculate zero-width lines or negative page sizes. */
        if (term_width <= 1 || term_height <= 0) {
            input = inkey_ex();
            if (input.type == EVT_RESIZE || input.type == EVT_NONE) continue;
            ch = fa_textblock_key_from_event(input);
            if (ch.type == EVT_KBRD) break;
            continue;
        }

        full = region_calculate(orig_area);
        if (full.col < 0) full.col = 0;
        if (full.row < 0) full.row = 0;
        if (full.col >= term_width - 1 || full.row >= term_height) {
            input = inkey_ex();
            if (input.type == EVT_RESIZE || input.type == EVT_NONE) continue;
            ch = fa_textblock_key_from_event(input);
            if (ch.type == EVT_KBRD) break;
            continue;
        }
        full.width = MAX(0, MIN(full.width, term_width - full.col - 1));
        full.page_rows = MAX(0, MIN(full.page_rows, term_height - full.row));
        if (full.width <= 0 || full.page_rows <= 0) {
            input = inkey_ex();
            if (input.type == EVT_RESIZE || input.type == EVT_NONE) continue;
            ch = fa_textblock_key_from_event(input);
            if (ch.type == EVT_KBRD) break;
            continue;
        }

        show_header = header && full.page_rows >= 3;
        show_footer = full.page_rows >= (show_header ? 3 : 2);
        body_rows = full.page_rows - (show_header ? 1 : 0) -
            (show_footer ? 1 : 0);
        area = full;
        if (show_header) {
            Term_erase(full.col, full.row, full.width);
            fa_textblock_print_bounded(header, full.row, full.col, full.width);
            ++area.row;
        }
        area.page_rows = body_rows;

        if (layout_width != area.width) {
            mem_free(line_starts);
            mem_free(line_lengths);
            line_starts = NULL;
            line_lengths = NULL;
            n_lines = textblock_calculate_display_lines(
                tb, &line_starts, &line_lengths, (size_t)area.width);
            layout_width = area.width;
        }
        /* Subtract only when the result is known to be nonnegative.
         * Old code mixed signed and size_t and could underflow after
         * a resize or with an empty textblock. */
        if (n_lines <= (size_t)body_rows) {
            start_line = 0;
        } else if (start_line > n_lines - (size_t)body_rows) {
            start_line = n_lines - (size_t)body_rows;
        }
        pager = n_lines > (size_t)body_rows;
        display_area(textblock_text(tb), textblock_attrs(tb),
            line_starts, line_lengths, n_lines, area, start_line);

        if (show_footer) {
            int row = area.row + body_rows;
            Term_erase(full.col, row, full.width);
            footer = pager ?
                (angdroid_zh_tw_enabled() ?
                    "上下鍵捲動，Esc 離開" : "Up/down or Esc to exit") :
                (angdroid_zh_tw_enabled() ?
                    "按任意鍵繼續" : "Press any key to continue");
            fa_textblock_print_bounded(footer, row, full.col, full.width);
        }

        input = inkey_ex();
        if (input.type == EVT_RESIZE || input.type == EVT_NONE) continue;
        ch = fa_textblock_key_from_event(input);
        if (ch.type != EVT_KBRD) continue;
        if (!pager || ch.code == ESCAPE || ch.code == 'q' ||
                ch.code == 'x' || ch.code == ']' || ch.code == '[')
            break;
        if (ch.code == ARROW_UP) {
            if (start_line) --start_line;
        } else if (ch.code == ARROW_DOWN) {
            if (start_line < n_lines - (size_t)body_rows) ++start_line;
        } else if (ch.code == ' ') {
            size_t remaining = n_lines - (size_t)body_rows - start_line;
            size_t step = (size_t)body_rows;
            start_line += remaining < step ? remaining : step;
        }
    }

    mem_free(line_starts);
    mem_free(line_lengths);
    screen_load();
    return ch;
}

/**
 * ------------------------------------------------------------------------
 * text_out hook for screen display
 * ------------------------------------------------------------------------ */

/**
 * Print some (colored) text to the screen at the current cursor position,
 * automatically "wrapping" existing text (at spaces) when necessary to
 * avoid placing any text into the last column, and clearing every line
 * before placing any text in that line.  Also, allow "newline" to force
 * a "wrap" to the next line.  Advance the cursor as needed so sequential
 * calls to this function will work correctly.
 *
 * Once this function has been called, the cursor should not be moved
 * until all the related "text_out()" calls to the window are complete.
 *
 * This function will correctly handle any width up to the maximum legal
 * value of 256, though it works best for a standard 80 character width.
 */
void text_out_to_screen(uint8_t a, const char *str)
{
	int x, y;

	int wid, h;

	int wrap;

	const wchar_t *s;
	wchar_t buf[1024];

    /* Android rotation may briefly invalidate the terminal. */
    if (!Term || !str) return;
    (void)Term_get_size(&wid, &h);
    if (wid < 2 || h < 1) return;
    if (Term_locate(&x, &y) != 0) return;

	/* Copy to a rewriteable string */
	text_mbstowcs(buf, str, 1023);
    buf[1023] = L'\0';
	
	/* Use special wrapping boundary? */
	if ((text_out_wrap > 0) && (text_out_wrap < wid))
		wrap = text_out_wrap;
	else
		wrap = wid;

    /* The legacy wrap scratch arrays have only 256 entries. */
    if (wrap > 256) wrap = 256;
    if (wrap < 2) return;

	/* Process the string */
	for (s = buf; *s; s++) {
		wchar_t ch;
        int span;

		/* Force wrap */
		if (*s == L'\n') {
			/* Wrap */
            x = MAX(0, MIN(text_out_indent, wrap - 1));
            if (++y >= h) return;

			/* Clear line, move cursor */
			Term_erase(x, y, 255);

            x = MAX(0, MIN(x + text_out_pad, wrap - 1));
			Term_gotoxy(x, y);

			continue;
		}

		/* Clean up the char */
		ch = (text_iswprint(*s) ? *s : L' ');
        span = angdroid_is_wide_text(ch, a) ? 2 : 1;
        if (span > wrap - 1) {
            /* An extremely narrow region cannot contain this glyph. */
            ch = L'?';
            span = 1;
        }

        /* Width is measured in physical screen cells. Keep one cell
         * available at the edge as the original English wrapper did. */
		if (angdroid_screen_text_needs_wrap(x, wrap, ch, a)) {
            if (ch == L' ') {
                /* A separator at the margin should not trigger an
                 * extra blank or consume an invalid terminal cell. */
                continue;
            }
			int i, n = 0;

			int av[256];
			wchar_t cv[256];

			/* Wrap word */
			if (span == 1 && x < wrap) {
				/* ASCII words may move intact. An already-painted CJK
                 * glyph and its continuation must remain together. */
				for (i = wrap - 2; i >= 0; i--) {
					/* Grab existing attr/char */
					Term_what(i, y, &av[i], &cv[i]);
                    if (cv[i] == ANGDROID_TEXT_CONTINUATION ||
                            angdroid_is_wide_text(cv[i], av[i])) {
                        n = wrap;
                        break;
                    }
                    if (i > 0) {
                        int previous_attr = 0;
                        wchar_t previous_char = L' ';
                        Term_what(i - 1, y, &previous_attr, &previous_char);
                        if (angdroid_is_wide_text(previous_char, previous_attr)) {
                            n = wrap;
                            break;
                        }
                    }

					/* Break on space */
					if (cv[i] == L' ') break;

					/* Track current word */
					n = i;
				}
			}

			/* Special case */
			if (n == 0) n = wrap;

            /* A whole word can reach the wrap boundary. In that case
             * n == wrap and there is no on-screen tail to erase. */
            if (n >= 0 && n < wrap)
                Term_erase(n, y, 255);

			/* Wrap */
            x = MAX(0, MIN(text_out_indent, wrap - 1));
            if (++y >= h) return;

			/* Clear line, move cursor */
			Term_erase(x, y, 255);

            x = MAX(0, MIN(x + text_out_pad, wrap - 1));
			Term_gotoxy(x, y);

			/* Wrap the word (if any) */
			for (i = n; i < wrap - 1; i++) {
				/* Dump */
				Term_addch(av[i], cv[i]);

				/* Advance (no wrap) */
				if (++x > wrap) x = wrap;
			}
		}

		/* Term_addch() now owns both physical cells of a wide
         * character and advances its cursor by that display width.
         * Keep this local x aligned with the terminal cursor, without
         * inserting a second continuation marker. */
        if (Term_addch(a, ch) < 0) return;
        x += span;
        if (x > wrap) x = wrap;
	}
}


/**
 * ------------------------------------------------------------------------
 * Simple text display
 * ------------------------------------------------------------------------ */

/**
 * Display a string on the screen using an attribute.
 *
 * At the given location, using the given attribute, if allowed,
 * add the given string.  Do not clear the line.
 */
void c_put_str(uint8_t attr, const char *str, int row, int col) {
	/* Position cursor, Dump the attr/text */
	Term_putstr(col, row, -1, attr, str);
}


/**
 * As above, but in white
 */
void put_str(const char *str, int row, int col) {
	c_put_str(COLOUR_WHITE, str, row, col);
}

/**
 * Display a string on the screen using an attribute, and clear to the
 * end of the line.
 */
void c_prt(uint8_t attr, const char *str, int row, int col) {
	/* Clear line, position cursor */
	Term_erase(col, row, 255);

	/* Dump the attr/text */
	Term_addstr(-1, attr, str);
}

/**
 * As above, but in white
 */
void prt(const char *str, int row, int col) {
	c_prt(COLOUR_WHITE, str, row, col);
}



/**
 * ------------------------------------------------------------------------
 * Screen loading/saving
 * ------------------------------------------------------------------------ */

/**
 * Screen loading and saving can be done to an arbitrary depth but it's
 * important that every call to screen_save() is balanced by a call to
 * screen_load() or screen_load_all() later on.  'screen_save_depth' is used
 * by the game to keep track of whether it should try to update the map and
 * sidebar or not, so if you miss out a screen_load or screen_load_all you will
 * not get proper game updates.
 *
 * Term_save() / Term_load() / Term_load_all() do all the heavy lifting here.
 */

/**
 * Depth of the screen_save() stack
 */
int16_t screen_save_depth;

/**
 * Save the screen, and increase the "icky" depth.
 */
void screen_save(void)
{
	player->upkeep->redraw |= PR_MAP;
	redraw_stuff(player);
	event_signal(EVENT_MESSAGE_FLUSH);
	Term_save();
	screen_save_depth++;
}

/**
 * Load the screen, and decrease the "icky" depth.
 */
void screen_load(void)
{
	event_signal(EVENT_MESSAGE_FLUSH);
	Term_load();
	screen_save_depth--;
}

/**
 * Load the screen by replaying all the saves in reverse order with a redraw
 * for each and decrease the "icky" depth.
 */
void screen_load_all(void)
{
	event_signal(EVENT_MESSAGE_FLUSH);
	Term_load_all();
	screen_save_depth--;
}

bool textui_map_is_visible(void)
{
	return (screen_save_depth == 0);
}

/**
 * ------------------------------------------------------------------------
 * Miscellaneous things
 * ------------------------------------------------------------------------ */

/**
 * A Hengband-like 'window' function, that draws a surround box in ASCII art.
 */
void window_make(int origin_x, int origin_y, int end_x, int end_y)
{
	int n;
	region to_clear;

	to_clear.col = origin_x;
	to_clear.row = origin_y;
	to_clear.width = end_x - origin_x;
	to_clear.page_rows = end_y - origin_y;

	region_erase(&to_clear);

	Term_putch(origin_x, origin_y, COLOUR_WHITE, L'+');
	Term_putch(end_x, origin_y, COLOUR_WHITE, L'+');
	Term_putch(origin_x, end_y, COLOUR_WHITE, L'+');
	Term_putch(end_x, end_y, COLOUR_WHITE, L'+');

	for (n = 1; n < (end_x - origin_x); n++) {
		Term_putch(origin_x + n, origin_y, COLOUR_WHITE, L'-');
		Term_putch(origin_x + n, end_y, COLOUR_WHITE, L'-');
	}

	for (n = 1; n < (end_y - origin_y); n++) {
		Term_putch(origin_x, origin_y + n, COLOUR_WHITE, L'|');
		Term_putch(end_x, origin_y + n, COLOUR_WHITE, L'|');
	}
}

bool panel_should_modify(term *t, int wy, int wx)
{
	int dungeon_hgt = cave->height;
	int dungeon_wid = cave->width;
	int screen_hgt = (t == angband_term[0]) ?
		SCREEN_HGT : t->hgt / tile_height;
	int screen_wid = (t == angband_term[0]) ?
		SCREEN_WID : t->wid / tile_width;

	/* Verify wy, adjust if needed */
	if (wy > dungeon_hgt - screen_hgt) wy = dungeon_hgt - screen_hgt;
	if (wy < 0) wy = 0;

	/* Verify wx, adjust if needed */
	if (wx > dungeon_wid - screen_wid) wx = dungeon_wid - screen_wid;
	if (wx < 0) wx = 0;

	/* Needs changes? */
	return ((t->offset_y != wy) || (t->offset_x != wx));
}

/**
 * Modify the current panel to the given coordinates, adjusting only to
 * ensure the coordinates are legal, and return true if anything done.
 *
 * The town should never be scrolled around.
 *
 * Note that monsters are no longer affected in any way by panel changes.
 *
 * As a total hack, whenever the current panel changes, we assume that
 * the "overhead view" window should be updated.
 */
bool modify_panel(term *t, int wy, int wx)
{
	int dungeon_hgt = cave->height;
	int dungeon_wid = cave->width;
	int screen_hgt = (t == angband_term[0]) ?
		SCREEN_HGT : t->hgt / tile_height;
	int screen_wid = (t == angband_term[0]) ?
		SCREEN_WID : t->wid / tile_width;

	/* Verify wy, adjust if needed */
	if (wy > dungeon_hgt - screen_hgt) wy = dungeon_hgt - screen_hgt;
	if (wy < 0) wy = 0;

	/* Verify wx, adjust if needed */
	if (wx > dungeon_wid - screen_wid) wx = dungeon_wid - screen_wid;
	if (wx < 0) wx = 0;

	/* React to changes */
	if (panel_should_modify(t, wy, wx)) {
		/* Save wy, wx */
		t->offset_y = wy;
		t->offset_x = wx;

		/* Redraw map */
		player->upkeep->redraw |= (PR_MAP);

		/* Changed */
		return (true);
	}

	/* No change */
	return (false);
}

static void verify_panel_int(bool centered)
{
	int wy, wx;
	int screen_hgt, screen_wid;

	int panel_wid, panel_hgt;

	int py = player->grid.y;
	int px = player->grid.x;

	int j;

	/* Scan windows */
	for (j = 0; j < ANGBAND_TERM_MAX; j++) {
		term *t = angband_term[j];

		/* No window */
		if (!t) continue;

		/* No relevant flags */
		if ((j > 0) && !(window_flag[j] & (PW_OVERHEAD))) continue;

		wy = t->offset_y;
		wx = t->offset_x;

		screen_hgt = (j == 0) ? SCREEN_HGT : t->hgt / tile_height;
		screen_wid = (j == 0) ? SCREEN_WID : t->wid / tile_width;

		panel_wid = screen_wid / 2;
		panel_hgt = screen_hgt / 2;


		/* Scroll screen vertically when off-center */
		if (centered && !player->upkeep->running && (py != wy + panel_hgt))
			wy = py - panel_hgt;

		/* Scroll screen vertically when 3 grids from top/bottom edge */
		else if ((py < wy + 3) || (py >= wy + screen_hgt - 3))
			wy = py - panel_hgt;


		/* Scroll screen horizontally when off-center */
		if (centered && !player->upkeep->running && (px != wx + panel_wid))
			wx = px - panel_wid;

		/* Scroll screen horizontally when 3 grids from left/right edge */
		else if ((px < wx + 3) || (px >= wx + screen_wid - 3))
			wx = px - panel_wid;


		/* Scroll if needed */
		modify_panel(t, wy, wx);
	}
}

/**
 * Change the current panel to the panel lying in the given direction.
 *
 * Return true if the panel was changed.
 */
bool change_panel(int dir)
{
	bool changed = false;
	int j;

	/* Scan windows */
	for (j = 0; j < ANGBAND_TERM_MAX; j++) {
		int screen_hgt, screen_wid;
		int wx, wy;

		term *t = angband_term[j];

		/* No window */
		if (!t) continue;

		/* No relevant flags */
		if ((j > 0) && !(window_flag[j] & PW_OVERHEAD)) continue;

		screen_hgt = (j == 0) ? SCREEN_HGT : t->hgt / tile_height;
		screen_wid = (j == 0) ? SCREEN_WID : t->wid / tile_width;

		/* Shift by half a panel */
		wy = t->offset_y + ddy[dir] * screen_hgt / 2;
		wx = t->offset_x + ddx[dir] * screen_wid / 2;

		/* Use "modify_panel" */
		if (modify_panel(t, wy, wx)) changed = true;
	}

	return (changed);
}


/**
 * Verify the current panel (relative to the player location).
 *
 * By default, when the player gets "too close" to the edge of the current
 * panel, the map scrolls one panel in that direction so that the player
 * is no longer so close to the edge.
 *
 * The "OPT(player, center_player)" option allows the current panel to always be
 * centered around the player, which is very expensive, and also has some
 * interesting gameplay ramifications.
 */
void verify_panel(void)
{
	verify_panel_int(OPT(player, center_player));
}

void center_panel(void)
{
	verify_panel_int(true);
}

void textui_get_panel(int *min_y, int *min_x, int *max_y, int *max_x)
{
	term *t = term_screen;

	if (!t) return;

	*min_y = t->offset_y;
	*min_x = t->offset_x;
	*max_y = t->offset_y + SCREEN_HGT;
	*max_x = t->offset_x + SCREEN_WID;
}

bool textui_panel_contains(unsigned int y, unsigned int x)
{
	unsigned int hgt;
	unsigned int wid;
	if (!Term)
		return true;
	if (Term == term_screen) {
		hgt = SCREEN_HGT;
		wid = SCREEN_WID;
	} else {
		hgt = Term->hgt / tile_height;
		wid = Term->wid / tile_width;
	}
	return (y - Term->offset_y) < hgt && (x - Term->offset_x) < wid;
}
