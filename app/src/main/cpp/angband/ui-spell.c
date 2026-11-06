/**
 * \file ui-spell.c
 * \brief Spell UI handing
 *
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
#include "cmds.h"
#include "cmd-core.h"
#include "effects.h"
#include "effects-info.h"
#include "game-input.h"
#include "obj-tval.h"
#include "obj-util.h"
#include "object.h"
#include "player-calcs.h"
#include "player-spell.h"
#include "ui-menu.h"
#include "ui-output.h"
#include "ui-spell.h"
#include "droid.h"
#include "zh-tw-ui.h"
#include "zh-tw-magic.h"
#include "zh-tw-spell-names.h"
#include "zh-tw-spell-descriptions.h"
#include "zh-tw-projection-terms.h"
#include "../curses/unicode_display.h"
#include "zh-tw-spell-reader-pages.h"


/**
 * Spell menu data struct
 */
struct spell_menu_data {
	int *spells;
	int n_spells;

	bool browse;
	bool (*is_valid)(const struct player *p, int spell_index);
	bool show_description;

	int selected_spell;
	/* -1 = no pending tap; a touch must explicitly arm its row. */
	int touch_pending_oid;
};


/**
 * Is item oid valid?
 */
static int spell_menu_valid(struct menu *m, int oid)
{
	struct spell_menu_data *d = menu_priv(m);
	int *spells = d->spells;

	return d->is_valid(player, spells[oid]);
}

/**
 * Display a row of the spell menu
 */
static void spell_menu_display(struct menu *m, int oid, bool cursor,
		int row, int col, int wid)
{
	struct spell_menu_data *d = menu_priv(m);
	int spell_index = d->spells[oid];
	const struct class_spell *spell = spell_by_index(player, spell_index);

	char help[192];
	char out[256];

	int attr;
	const char *illegible = NULL;
	const char *comment = "";
	size_t spell_columns;
    char spell_name_buf[96];
    const char *display_spell_name;

	if (!spell) return;

	if (spell->slevel >= 99) {
		illegible = angdroid_game_ui_text("(illegible)", angdroid_zh_tw_enabled());
		attr = COLOUR_L_DARK;
	} else if (player->spell_flags[spell_index] & PY_SPELL_FORGOTTEN) {
		comment = angdroid_game_ui_text(" forgotten", angdroid_zh_tw_enabled());
		attr = COLOUR_YELLOW;
	} else if (player->spell_flags[spell_index] & PY_SPELL_LEARNED) {
		if (player->spell_flags[spell_index] & PY_SPELL_WORKED) {
			/* Get extra info */
			get_spell_info(spell_index, help, sizeof(help));
			/* Clip by terminal columns, not UTF-8 bytes. */
			angdroid_utf8_clip_columns(help, 22);
			comment = help;
			attr = COLOUR_WHITE;
		} else {
			comment = angdroid_game_ui_text(" untried", angdroid_zh_tw_enabled());
			attr = COLOUR_L_GREEN;
		}
	} else if (spell->slevel <= player->lev) {
		comment = angdroid_game_ui_text(" unknown", angdroid_zh_tw_enabled());
		attr = COLOUR_L_BLUE;
	} else {
		comment = angdroid_game_ui_text(" difficult", angdroid_zh_tw_enabled());
		attr = COLOUR_RED;
	}

	/* Localize only the presented name, not the class spell entry.
     * Calculate terminal columns, never UTF-8 bytes or codepoint count:
     * a Han glyph occupies two cells on Android's curses terminal. */
    display_spell_name = angdroid_zh_tw_spell_name(spell->name,
        angdroid_zh_tw_enabled());
    my_strcpy(spell_name_buf, display_spell_name, sizeof(spell_name_buf));
    angdroid_utf8_clip_columns(spell_name_buf, 30);
    spell_columns = angdroid_utf8_display_columns(spell_name_buf);
    strnfmt(out, sizeof(out), "%s%*s", spell_name_buf,
        (int)(30 - spell_columns), "");
	my_strcat(out, format("%2d %4d %3d%%%s", spell->slevel, spell->smana,
		spell_chance(spell_index), comment), sizeof(out));
	c_prt(attr, illegible ? illegible : out, row, col);
}

/* Open full description in a separate paged reader.
 * Does not replace the spell menu's arrow or letter-key behaviors. */
static void spell_menu_show_full_description(const struct class_spell *spell)
{
    bool zh;
    const char *description, *name;
    size_t first = 0, saved_offset = 0;
    bool reflow = false;
    bool finished = false;
    if (!spell) return;
    zh = angdroid_zh_tw_enabled();
    description = angdroid_zh_tw_spell_description(spell->name,
        spell->text, zh);
    name = angdroid_zh_tw_spell_name(spell->name, zh);
    if (!description) return;

    screen_save();
    while (!finished) {
        int width, height;
        size_t columns, visible, total, row = 0, start = 0, next;
        size_t top_offset = 0;
        char line[1024], title[256], status[256];
        ui_event in;
        Term_get_size(&width, &height);
        if (width < 4 || height < 4) break;
        columns = (size_t)(width - 1);
        visible = (size_t)(height - 2);
        total = angdroid_spell_reader_line_count(description, columns);
        if (reflow) {
            first = angdroid_spell_reader_line_for_offset(description,
                columns, saved_offset);
            reflow = false;
        }
        first = angdroid_spell_reader_clamp_scroll(first, total, visible);

        Term_clear();
        /* Avoid allowing the title itself to wrap into the body. */
        my_strcpy(title, name, sizeof(title));
        angdroid_utf8_clip_columns(title, columns);
        prt(title, 0, 0);
        while (row < first + visible &&
            angdroid_spell_reader_next_line(description, start, columns,
                line, sizeof(line), &next)) {
            if (row == first) top_offset = start;
            if (row >= first)
                c_prt(COLOUR_WHITE, line, (int)(row - first) + 1, 0);
            start = next;
            ++row;
        }
        /* Keep the exit hint visible even on narrow portrait terminals. */
        if (zh)
            strnfmt(status, sizeof(status),
                "Esc/點頂底返回  %d/%d 行 ↑↓捲動",
                (int)(first + (total > 0)), (int)total);
        else
            strnfmt(status, sizeof(status),
                "Esc/tap top or bottom back  %d/%d",
                (int)(first + (total > 0)), (int)total);
        angdroid_utf8_clip_columns(status, columns);
        prt(status, height - 1, 0);

        in = inkey_ex();
        if (in.type == EVT_RESIZE) {
            /* The old row number is invalid after changing column width. */
            saved_offset = top_offset;
            reflow = true;
            continue;
        }
        if (in.type == EVT_ESCAPE) break;
        if (in.type == EVT_MOUSE) {
            int action = angdroid_spell_reader_tap_action(
                (int)in.mouse.x, (int)in.mouse.y, width, height);
            if (action == ANGDROID_SPELL_READER_TAP_BACK) break;
            if (action == ANGDROID_SPELL_READER_TAP_UP)
                first = angdroid_spell_reader_page_up(first, visible);
            else if (action == ANGDROID_SPELL_READER_TAP_DOWN)
                first = angdroid_spell_reader_page_down(first, total, visible);
            continue;
        }
        if (in.type != EVT_KBRD) continue;
        switch (in.key.code) {
            case ESCAPE:
            case 'q':
            case 'Q':
                finished = true;
                break;
            case KC_HOME:
                first = 0;
                break;
            case KC_END:
                first = angdroid_spell_reader_clamp_scroll(total, total, visible);
                break;
            case ARROW_UP:
            case 'k':
                if (first > 0) --first;
                break;
            case ARROW_DOWN:
            case 'j':
                first = angdroid_spell_reader_clamp_scroll(first + 1, total, visible);
                break;
            case KC_PGUP:
            case '[':
                first = angdroid_spell_reader_page_up(first, visible);
                break;
            case KC_PGDOWN:
            case ' ':
            case KC_ENTER:
            case ']':
                first = angdroid_spell_reader_page_down(first, total, visible);
                break;
        }
    }
    screen_load();
}

/* Menu callbacks can consume EVT_RESIZE while a nested reader is open.
 * The outer menu_select() then never sees that event and keeps the old
 * active region. Rebuild from the CURRENT terminal dimensions whenever
 * the reader closes; keep m->cursor and selected_spell unchanged. */
static void spell_menu_relayout(struct menu *m)
{
    int width, height, menu_width;
    region bounds;
    struct spell_menu_data *d = menu_priv(m);

    /* Rotation or returning from the reader cancels an old confirmation. */
    d->touch_pending_oid = -1;
    Term_get_size(&width, &height);
    menu_width = angdroid_spell_reader_menu_width(width);
    bounds.col = -menu_width;
    bounds.row = 1;
    bounds.width = menu_width;
    bounds.page_rows = m->count + 1;
    (void)menu_layout(m, &bounds);
}

/**
 * Handle an event on a menu row.
 */
static bool spell_menu_handler(struct menu *m, const ui_event *e, int oid)
{
    struct spell_menu_data *d = menu_priv(m);

    if (e->type == EVT_MOUSE) {
        /* menu_select() has already moved m->cursor for this tap.
         * Never arm a header, outside or back-button touch. */
        if (!angdroid_spell_menu_touch_selectable(
                region_inside(&m->active, e), e->mouse.button, oid) ||
                oid >= d->n_spells) {
            d->touch_pending_oid = -1;
            return false;
        }
        /* Casting/studying needs two intentional touches of the SAME
         * spell. Browsing is harmless and remains a single touch. */
        if (!angdroid_spell_menu_confirm_touch(
                &d->touch_pending_oid, oid, d->browse))
            return true;
    } else if (e->type == EVT_SELECT) {
        /* Enter and letter tags still perform the original single-step
         * selection: only touch interactions require confirmation. */
        d->touch_pending_oid = -1;
    } else if (e->type == EVT_KBRD) {
        d->touch_pending_oid = -1;
        if (e->key.code == '?') {
            d->show_description = !d->show_description;
            return true;
        }
        if (e->key.code == ']') {
            const struct class_spell *spell;
            if (oid < 0 || oid >= d->n_spells) return true;
            spell = spell_by_index(player, d->spells[oid]);
            spell_menu_show_full_description(spell);
            /* The reader may have consumed a resize event. */
            spell_menu_relayout(m);
            return true;
        }
        return false;
    } else {
        d->touch_pending_oid = -1;
        return false;
    }

    if (oid < 0 || oid >= d->n_spells) return false;
    d->selected_spell = d->spells[oid];
    /* Browse stays in the menu; casting returns the one select event
     * from menu_select() without a second row-handler invocation. */
    return d->browse ? true : false;
}

/**
 * Show spell long description when browsing
 */
static void spell_menu_browser(int oid, void *data, const region *loc)
{
	struct spell_menu_data *d = data;
	/* Arrow/key navigation away from an armed touch cancels that tap. */
	if (d->touch_pending_oid >= 0 && d->touch_pending_oid != oid)
		d->touch_pending_oid = -1;
	int spell_index = d->spells[oid];
	const struct class_spell *spell = spell_by_index(player, spell_index);

	if (d->show_description) {
		/* Redirect output to the screen */
		text_out_hook = text_out_to_screen;
		text_out_wrap = 0;
		/* Guard against an invalid erase origin when loc->col is zero. */
		text_out_indent = loc->col > 0 ? loc->col - 1 : 0;
		text_out_pad = loc->col > 0 ? 1 : 0;

		Term_gotoxy(loc->col, loc->row + loc->page_rows);
		/* Spell description */
		/* Localize the view only, preserving canonical spell data. */
		text_out("\n%s", angdroid_zh_tw_spell_description(
			spell->name, spell->text, angdroid_zh_tw_enabled()));

		/* To summarize average damage, count the damaging effects */
		int num_damaging = 0;
		for (struct effect *e = spell->effect; e != NULL; e = effect_next(e)) {
			if (effect_damages(e)) {
				num_damaging++;
			}
		}
		/* Now enumerate the effects' damage and type if not forgotten */
		if (num_damaging > 0
			&& (player->spell_flags[spell_index] & PY_SPELL_WORKED)
			&& !(player->spell_flags[spell_index] & PY_SPELL_FORGOTTEN)) {
			dice_t *shared_dice = NULL;
			int i = 0;

			text_out("%s", angdroid_zh_tw_enabled() ? "  平均造成" : "  Inflicts an average of");
			for (struct effect *e = spell->effect; e != NULL; e = effect_next(e)) {
				if (e->index == EF_SET_VALUE) {
					shared_dice = e->dice;
				} else if (e->index == EF_CLEAR_VALUE) {
					shared_dice = NULL;
				}
				if (effect_damages(e)) {
					if (num_damaging > 2 && i > 0) {
						text_out(",");
					}
					if (num_damaging > 1 && i == num_damaging - 1) {
						text_out("%s", angdroid_zh_tw_enabled() ? " 與" : " and");
					}
					text_out_c(COLOUR_L_GREEN, " %d", effect_avg_damage(e, shared_dice));
					const char *projection = angdroid_projection_description(
						effect_projection(e), angdroid_zh_tw_enabled());
					if (strlen(projection) > 0) {
						text_out(" %s", projection);
					}
					i++;
				}
			}
			text_out("%s", angdroid_zh_tw_enabled() ? " 點傷害。" : " damage.");
		}
		text_out("\n\n");

		/* XXX */
		text_out_pad = 0;
		text_out_indent = 0;
	}
}

static const menu_iter spell_menu_iter = {
	NULL,	/* get_tag = NULL, just use lowercase selections */
	spell_menu_valid,
	spell_menu_display,
	spell_menu_handler,
	spell_menu_relayout	/* reflow menu and clear pending touch on resize */
};

/**
 * Create and initialise a spell menu, given an object and a validity hook
 */
static struct menu *spell_menu_new(const struct object *obj,
		bool (*is_valid)(const struct player *p, int spell_index),
		bool show_description)
{
	struct menu *m = menu_new(MN_SKIN_SCROLL, &spell_menu_iter);
	struct spell_menu_data *d = mem_alloc(sizeof *d);


	/* collect spells from object */
	d->n_spells = spell_collect_from_book(player, obj, &d->spells);
	if (d->n_spells == 0 || !spell_okay_list(player, is_valid, d->spells, d->n_spells)) {
		mem_free(m);
		mem_free(d->spells);
		mem_free(d);
		return NULL;
	}

	/* Copy across private data */
	d->is_valid = is_valid;
	d->selected_spell = -1;
	d->touch_pending_oid = -1;
	d->browse = false;
	d->show_description = show_description;

	menu_setpriv(m, d->n_spells, d);

	/* Set flags */
	m->header = "Name                             Lv Mana Fail Info";
	m->flags = MN_CASELESS_TAGS;
	m->selections = all_letters_nohjkl;
	m->browse_hook = spell_menu_browser;
	/* Separate reader command without modifying spell tag selection. */
	m->cmd_keys = "?]";

	/* Match the same layout calculation on creation and on return. */
	spell_menu_relayout(m);

	return m;
}

/**
 * Clean up a spell menu instance
 */
static void spell_menu_destroy(struct menu *m)
{
	struct spell_menu_data *d = menu_priv(m);
	mem_free(d->spells);
	mem_free(d);
	mem_free(m);
}

/**
 * Run the spell menu to select a spell.
 */
static int spell_menu_select(struct menu *m, const char *noun, const char *verb)
{
	struct spell_menu_data *d = menu_priv(m);
	char buf[192];

	screen_save();
	region_erase_bordered(&m->active);

	/* Format, capitalise and display */
	if (angdroid_zh_tw_enabled()) {
        strnfmt(buf, sizeof buf, "選擇%s（%s）？觸控同列兩次確認；? 說明 ] 全文",
            angdroid_zh_tw_magic_term(noun, true),
            angdroid_zh_tw_magic_term(verb, true));
    } else {
        strnfmt(buf, sizeof buf, "%s which %s? Tap row twice; ? help, ] full",
                verb, noun);
        my_strcap(buf);
    }
	/* The extra two-tap help must not wrap across the spell menu. */
	if (Term->wid > 1)
		angdroid_utf8_clip_columns(buf, (size_t)(Term->wid - 1));
	prt(buf, 0, 0);

	menu_select(m, 0, true);
	screen_load();

	return d->selected_spell;
}

/**
 * Run the spell menu, without selections.
 */
static void spell_menu_browse(struct menu *m, const char *noun)
{
	struct spell_menu_data *d = menu_priv(m);

	screen_save();

	region_erase_bordered(&m->active);
	if (angdroid_zh_tw_enabled())
        prt(format("瀏覽%s（? 切換說明、] 閱讀全文）",
            angdroid_zh_tw_magic_term(noun, true)), 0, 0);
    else
        prt(format("Browsing %ss. ('?' toggle, ']' read all)", noun), 0, 0);

	d->browse = true;
	d->touch_pending_oid = -1;
	menu_select(m, 0, true);

	screen_load();
}

/**
 * Browse a given book.
 */
void textui_book_browse(const struct object *obj)
{
	struct menu *m;
	const char *noun = player_object_to_book(player, obj)->realm->spell_noun;

	m = spell_menu_new(obj, spell_okay_to_browse, true);
	if (m) {
		spell_menu_browse(m, noun);
		spell_menu_destroy(m);
	} else {
		msg("%s", angdroid_game_ui_text("You cannot browse that.", angdroid_zh_tw_enabled()));
	}
}

/**
 * Browse the given book.
 */
void textui_spell_browse(void)
{
	struct object *obj;

	if (!get_item(&obj, angdroid_game_ui_text("Browse which book? ", angdroid_zh_tw_enabled()),
				  angdroid_game_ui_text("You have no books that you can read.", angdroid_zh_tw_enabled()),
				  CMD_BROWSE_SPELL, obj_can_browse,
				  (USE_INVEN | USE_FLOOR | IS_HARMLESS)))
		return;

	/* Track the object kind */
	track_object(player->upkeep, obj);
	handle_stuff(player);

	textui_book_browse(obj);
}

/**
 * Get a spell from specified book.
 */
int textui_get_spell_from_book(struct player *p, const char *verb,
	struct object *book, const char *error,
	bool (*spell_filter)(const struct player *p, int spell_index))
{
	const char *noun = player_object_to_book(p, book)->realm->spell_noun;
	struct menu *m;

	track_object(p->upkeep, book);
	handle_stuff(p);

	m = spell_menu_new(book, spell_filter, false);
	if (m) {
		int spell_index = spell_menu_select(m, noun, verb);
		spell_menu_destroy(m);
		return spell_index;
	} else if (error) {
		msg("%s", error);
	}

	return -1;
}

/**
 * Get a spell from the player.
 */
int textui_get_spell(struct player *p, const char *verb,
		item_tester book_filter, cmd_code cmd, const char *book_error,
		bool (*spell_filter)(const struct player *p, int spell_index),
		const char *spell_error, struct object **rtn_book)
{
	char prompt[1024];
	struct object *book;

	/* Create prompt */
	if (angdroid_zh_tw_enabled())
        strnfmt(prompt, sizeof prompt, "選擇要%s的書本：",
            angdroid_zh_tw_magic_term(verb, true));
    else {
        strnfmt(prompt, sizeof prompt, "%s which book?", verb);
        my_strcap(prompt);
    }

	if (!get_item(&book, prompt, book_error,
				  cmd, book_filter, (USE_INVEN | USE_FLOOR)))
		return -1;

	if (rtn_book) {
		*rtn_book = book;
	}

	return textui_get_spell_from_book(p, verb, book, spell_error,
		spell_filter);
}
