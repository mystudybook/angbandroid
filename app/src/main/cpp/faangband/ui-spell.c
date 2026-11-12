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
#include "../angband/zh-tw-ui.h"
#include "../angband/zh-tw-magic.h"
#include "../angband/zh-tw-spell-names.h"
#include "../angband/zh-tw-spell-descriptions.h"
#include "../angband/zh-tw-projection-terms.h"
#include "../angband/menu-text-fit.h"


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
    char help[128], out[384], name[192], visible[384];
    const char *comment = "", *illegible = NULL;
    int attr, available, name_budget;
    size_t name_columns;

    if (!spell || !Term || col < 0 || col >= Term->wid || wid <= 0)
        return;

    if (spell->slevel >= 99) {
        illegible = angdroid_game_ui_text("(illegible)",
            angdroid_zh_tw_enabled());
        attr = COLOUR_L_DARK;
    } else if (player->spell_flags[spell_index] & PY_SPELL_FORGOTTEN) {
        comment = angdroid_game_ui_text(" forgotten", angdroid_zh_tw_enabled());
        attr = COLOUR_YELLOW;
    } else if (player->spell_flags[spell_index] & PY_SPELL_LEARNED) {
        if (player->spell_flags[spell_index] & PY_SPELL_WORKED) {
            get_spell_info(spell_index, help, sizeof(help));
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

    available = MIN(wid, Term->wid - col);
    /* Reserve 15 terminal columns for Lv/Mana/Fail when there is room.
     * A full-width view keeps the original 30-column name field. */
    name_budget = MIN(30, MAX(0, available - 15));
    if (name_budget <= 0) name_budget = available;
    name_columns = angdroid_menu_copy_columns(name, sizeof(name),
        angdroid_zh_tw_spell_name(spell->name, angdroid_zh_tw_enabled()),
        (size_t)name_budget);
    if (available > name_budget) {
        strnfmt(out, sizeof(out), "%s%*s%2d %4d %3d%%%s", name,
            (int)(name_budget - name_columns), "", spell->slevel,
            spell->smana, spell_chance(spell_index), comment);
    } else {
        my_strcpy(out, name, sizeof(out));
    }

    angdroid_menu_copy_columns(visible, sizeof(visible),
        illegible ? illegible : out, (size_t)available);
    if (visible[0]) c_prt(attr, visible, row, col);
}

/**
 * Handle an event on a menu row.
 */
static bool spell_menu_handler(struct menu *m, const ui_event *e, int oid)
{
	struct spell_menu_data *d = menu_priv(m);

	if (e->type == EVT_SELECT) {
		d->selected_spell = d->spells[oid];
		return d->browse ? true : false;
	}
	else if (e->type == EVT_KBRD) {
		if (e->key.code == '?') {
			d->show_description = !d->show_description;
		}
	}

	return false;
}

/**
 * Show spell long description when browsing
 */
static void spell_menu_browser(int oid, void *data, const region *loc)
{
	struct spell_menu_data *d = data;
	int spell_index = d->spells[oid];
	const struct class_spell *spell = spell_by_index(player, spell_index);

	if (d->show_description) {
		/* Redirect output to the screen */
		text_out_hook = text_out_to_screen;
		text_out_wrap = 0;
		text_out_indent = loc->col - 1;
		text_out_pad = 1;

		Term_gotoxy(loc->col, loc->row + loc->page_rows);
		/* Spell description */
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
	NULL	/* no resize hook */
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
	size_t width = MAX(0, MIN(Term->wid - 15, 80));

	region loc = { 0 - width, 1, width, -99 };

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
	d->browse = false;
	d->show_description = show_description;

	menu_setpriv(m, d->n_spells, d);

	/* Set flags */
	m->header = angdroid_zh_tw_enabled() ?
        "名稱                            等級 魔力 失敗率 資訊" :
        "Name                             Lv Mana Fail Info";
	m->flags = MN_CASELESS_TAGS;
	m->selections = all_letters_nohjkl;
	m->browse_hook = spell_menu_browser;
	m->cmd_keys = "?";

	/* Set size */
	loc.page_rows = d->n_spells + 1;
	menu_layout(m, &loc);

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
    char buf[192], visible[192];

    screen_save();
    region_erase_bordered(&m->active);
    if (angdroid_zh_tw_enabled())
        strnfmt(buf, sizeof(buf), "選擇要%s的%s（? 切換說明）：",
            angdroid_zh_tw_magic_term(verb, true),
            angdroid_zh_tw_magic_term(noun, true));
    else {
        strnfmt(buf, sizeof(buf),
            "%s which %s? ('?' to toggle description)", verb, noun);
        my_strcap(buf);
    }
    angdroid_menu_copy_columns(visible, sizeof(visible),
        buf, Term ? (size_t)MAX(0, Term->wid) : 0);
    prt(visible, 0, 0);

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
    {
        char prompt[192], visible[192];
        if (angdroid_zh_tw_enabled())
            strnfmt(prompt, sizeof(prompt), "瀏覽%s（? 切換說明）",
                angdroid_zh_tw_magic_term(noun, true));
        else
            strnfmt(prompt, sizeof(prompt),
                "Browsing %ss. ('?' to toggle description)", noun);
        angdroid_menu_copy_columns(visible, sizeof(visible), prompt,
            Term ? (size_t)MAX(0, Term->wid) : 0);
        prt(visible, 0, 0);
    }

	d->browse = true;
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
		msg("%s", angdroid_game_ui_text(error, angdroid_zh_tw_enabled()));
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
        strnfmt(prompt, sizeof(prompt), "選擇要%s的書本：",
            angdroid_zh_tw_magic_term(verb, true));
    else {
        strnfmt(prompt, sizeof(prompt), "%s which book?", verb);
        my_strcap(prompt);
    }

	if (!get_item(&book, prompt, angdroid_game_ui_text(book_error, angdroid_zh_tw_enabled()),
				  cmd, book_filter, (USE_INVEN | USE_FLOOR)))
		return -1;

	if (rtn_book) {
		*rtn_book = book;
	}

	return textui_get_spell_from_book(p, verb, book, spell_error,
		spell_filter);
}
