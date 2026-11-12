/**
 * \file ui-player-properties.c 
 * \brief UI for class and race abilities
 *
 * Copyright (c) 1997-2020 Ben Harrison, James E. Wilson, Robert A. Koeneke,
 * Leon Marrick, Bahman Rabii, Nick McConnell
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
#include "player-properties.h"
#include "ui-input.h"
#include "ui-menu.h"
#include "ui-player-properties.h"

#include "droid.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/zh-tw-player-ability.h"
#include "../angband/menu-text-fit.h"
#include "ui-birth-abilities-zh-tw.h"
#include "ui-birth-elements-zh-tw.h"

/**
 * ------------------------------------------------------------------------
 * Code for viewing race and class abilities
 * ------------------------------------------------------------------------ */

static char view_ability_tag(struct menu *menu, int oid)
{
    /* Long race/class lists remain scrollable without reading past tags. */
    return oid >= 0 && (size_t)oid < strlen(all_letters_nohjkl) ?
        all_letters_nohjkl[oid] : 0;
}

/**
 * Display an entry on the gain ability menu
 */
static void view_ability_display(struct menu *menu, int oid, bool cursor,
    int row, int col, int width)
{
    char buf[256], visible[256];
    uint8_t color;
    bool zh = angdroid_zh_tw_enabled();
    const struct player_ability *choices = menu->menu_data;
    const char *name = angdroid_fa_birth_ability_name(
        choices[oid].name, choices[oid].type, choices[oid].index, zh);

    /* Race abilities may be element records, not PF or OF flags.
     * Translate using the native damage percentage, never the English
     * source name; all neutral/unknown values keep the source label. */
    if (zh && choices[oid].type && streq(choices[oid].type, "element")) {
        const char *element = angdroid_fa_birth_element_name(choices[oid].index);
        int tier = angdroid_fa_birth_element_resistance(choices[oid].value);
        if (element) {
            if (tier == 2) name = format("免疫：%s", element);
            else if (tier == 1) name = format("抗性：%s", element);
            else if (tier == -1) name = format("弱點：%s", element);
        }
    }

    switch (choices[oid].group) {
    case PLAYER_FLAG_SPECIAL:
        strnfmt(buf, sizeof(buf), zh ? "特殊能力：%s" :
            "Specialty Ability: %s", name);
        color = COLOUR_GREEN;
        break;
    case PLAYER_FLAG_CLASS:
        strnfmt(buf, sizeof(buf), zh ? "職業能力：%s" : "Class: %s", name);
        color = COLOUR_UMBER;
        break;
    case PLAYER_FLAG_RACE:
        strnfmt(buf, sizeof(buf), zh ? "種族能力：%s" : "Racial: %s", name);
        color = COLOUR_ORANGE;
        break;
    default:
        my_strcpy(buf, zh ? "神祕能力" : "Mysterious", sizeof(buf));
        color = COLOUR_PURPLE;
        break;
    }

    /* Do not clip a multi-byte name in the middle of a Chinese glyph. */
    angdroid_menu_copy_columns(visible, sizeof(visible), buf,
        width > 0 ? (size_t)width : 0);
    c_put_str(cursor ? COLOUR_WHITE : color, visible, row, col);
}


/**
 * Show ability long description when browsing
 */
static void view_ability_menu_browser(int oid, void *data, const region *loc)
{
	struct player_ability *choices = data;

	/* Redirect output to the screen */
	text_out_hook = text_out_to_screen;
	text_out_wrap = 60;
	text_out_indent = loc->col - 1;
	text_out_pad = 1;

	clear_from(loc->row + loc->page_rows);
	Term_gotoxy(loc->col, loc->row + loc->page_rows);
	text_out_c(COLOUR_L_BLUE, "\n%s\n",
        angdroid_ability_description(choices[oid].desc,
            angdroid_zh_tw_enabled()));

	/* XXX */
	text_out_pad = 0;
	text_out_indent = 0;
	text_out_wrap = 0;
}

/**
 * Display list available specialties.
 */
void textui_view_ability_menu(struct player_ability *ability_list,
							  int num_abilities)
{
	struct menu menu;
	menu_iter menu_f = { view_ability_tag, 0, view_ability_display, 0, 0 };
	region loc = { 0, 0, 70, -99 };
	char buf[160];
    int last_shortcut;

    if (!ability_list || num_abilities <= 0 ||
        !all_letters_nohjkl[0]) return;
    last_shortcut = MIN(num_abilities,
        (int)strlen(all_letters_nohjkl)) - 1;

	/* Save the screen and clear it */
	screen_save();

	/* Prompt choices */
    strnfmt(buf, sizeof(buf),
        angdroid_zh_tw_enabled() ?
            "種族與職業能力（%c-%c，Esc 離開）：" :
            "Race and class abilities (%c-%c, ESC=exit): ",
        all_letters_nohjkl[0], all_letters_nohjkl[last_shortcut]);

	/* Set up the menu */
	menu_init(&menu, MN_SKIN_SCROLL, &menu_f);
	menu.header = buf;
	menu_setpriv(&menu, num_abilities, ability_list);
	loc.page_rows = num_abilities + 1;
	menu.flags = MN_DBL_TAP;
	menu.browse_hook = view_ability_menu_browser;
	region_erase_bordered(&loc);
	menu_layout(&menu, &loc);

	menu_select(&menu, 0, false);

	/* Load screen */
	screen_load();

	return;
}

/**
 * ------------------------------------------------------------------------
 * Code for gaining specialties
 * ------------------------------------------------------------------------ */
/**
 * Specialty ability menu data struct
 */
struct spec_menu_data {
	int *specialties;
	int spec_known;
	int selected_spec;
};

/**
 * Check if we can gain a specialty ability -BR-
 */
static bool check_specialty_gain(int specialty)
{
	/* Can't learn it if we already know it */
	if (pf_has(player->specialties, specialty)) {
		return false;
	}

	/* Is it allowed for this class? */
	if (pf_has(player->class->specialties, specialty)) {
		return true;
	}

	return false;
}

/**
 * Gain specialty code 
 */

static char gain_spec_tag(struct menu *menu, int oid)
{
	return I2A(oid);
}

/**
 * Display an entry on the gain specialty menu
 */
static void gain_spec_display(struct menu *menu, int oid, bool cursor, int row,
    int col, int width)
{
    struct spec_menu_data *d = menu_priv(menu);
    int idx = d->specialties[oid];
    struct player_ability *ability = lookup_ability("player", idx, 0);
    uint8_t attr = cursor ? COLOUR_L_GREEN : COLOUR_GREEN;
    char visible[256];
    const char *name;

    if (!ability) return;
    name = angdroid_fa_birth_ability_name(ability->name,
        ability->type, ability->index, angdroid_zh_tw_enabled());
    angdroid_menu_copy_columns(visible, sizeof(visible), name,
        width > 0 ? (size_t)width : 0);
    c_put_str(attr, visible, row, col);
}


/**
 * Deal with events on the gain specialty menu
 */
static bool gain_spec_action(struct menu *menu, const ui_event *e, int oid)
{
	struct spec_menu_data *d = menu_priv(menu);

	if (e->type == EVT_SELECT) {
		d->selected_spec = d->specialties[oid];
		return false;
	}

	return true;
}


/**
 * Show spell long description when browsing
 */
static void gain_spec_menu_browser(int oid, void *data, const region *loc)
{
	struct spec_menu_data *d = data;
	int idx = d->specialties[oid];
	struct player_ability *ability = lookup_ability("player", idx, 0);

	if (!ability) return;

	/* Redirect output to the screen */
	text_out_hook = text_out_to_screen;
	text_out_wrap = 60;
	text_out_indent = loc->col - 1;
	text_out_pad = 1;

	clear_from(loc->row + loc->page_rows);
	Term_gotoxy(loc->col, loc->row + loc->page_rows + 1);
	text_out_to_screen(COLOUR_DEEP_L_BLUE,
        (char *)angdroid_ability_description(ability->desc,
            angdroid_zh_tw_enabled()));

	/* Reset */
	text_out_pad = 0;
	text_out_indent = 0;
	text_out_wrap = 0;
}

/**
 * Display list available specialties.
 */
bool textui_gain_spec_menu(int *pick)
{
	struct menu menu;
	menu_iter menu_f = { gain_spec_tag, 0, gain_spec_display,
		gain_spec_action, 0
	};
	ui_event evt = { 0 };
	struct spec_menu_data *d = mem_alloc(sizeof *d);
	region loc = { 0, 0, 70, -99 };

	size_t i;

	char buf[192];

	/* Count the learnable specialties */
	d->spec_known = 0;
	for (i = 0; i < PF_MAX; i++) {
		if (check_specialty_gain(i)) {
			d->spec_known++;
		}
	}

	/* We are out of specialties - should never happen */
	if (!d->spec_known) {
		msg("%s", angdroid_zh_tw_enabled() ?
            "沒有可學習的專精能力。" : "No specialties available.");
        mem_free(d);
		return false;
	}

	/* Find the learnable specialties */
	d->specialties = mem_zalloc(d->spec_known * sizeof(int));
	d->spec_known = 0;
	d->selected_spec = 0;
	for (i = 0; i < PF_MAX; i++) {
		if (check_specialty_gain(i)) {
			d->specialties[d->spec_known] = i;
			d->spec_known++;
		}
	}

	/* Save the screen and clear it */
	screen_save();
	clear_from(0);

	/* Prompt choices */
    strnfmt(buf, sizeof(buf), angdroid_zh_tw_enabled() ?
            "學習哪項專精（%c-%c，Esc 離開，剩餘 %d 次）？" :
            "(Specialties: %c-%c, ESC=exit) Gain which specialty (%d available)? ",
            I2A(0), I2A(d->spec_known - 1), player->upkeep->new_specialties);

	/* Set up the menu */
	menu_init(&menu, MN_SKIN_SCROLL, &menu_f);
	menu.title = buf;
	menu_setpriv(&menu, d->spec_known, d);
	loc.page_rows = d->spec_known + 2;
	menu.browse_hook = gain_spec_menu_browser;
	menu.flags = MN_DBL_TAP;
	region_erase_bordered(&loc);
	menu_layout(&menu, &loc);

	while (1) {
		evt = menu_select(&menu, EVT_SELECT, true);
		if (evt.type == EVT_ESCAPE) {
			break;
		}
		if (d->selected_spec) {
			int idx = d->selected_spec;
			struct player_ability *ability = lookup_ability("player", idx, 0);
			region_erase_bordered(&loc);
			menu_layout(&menu, &loc);
            if (ability && get_check(format(
                    angdroid_zh_tw_enabled() ? "確定選擇%s？" :
                        "Definitely choose %s? ",
                    angdroid_fa_birth_ability_name(ability->name,
                        ability->type, ability->index,
                        angdroid_zh_tw_enabled())))) {
				break;
			}
			d->selected_spec = 0;
		}
	}

	if (evt.type != EVT_ESCAPE)
		*pick = d->selected_spec;

	/* Load screen */
	screen_load();
    mem_free(d->specialties);
    mem_free(d);

	return (evt.type != EVT_ESCAPE);
}

/**
 * ------------------------------------------------------------------------
 * Main ability interaction function
 * ------------------------------------------------------------------------ */
/**
 * Gain a new specialty ability
 * Adapted from birth.c get_player_choice -BR-
 */
void textui_interact_with_specialties(void)
{
	ui_event answer;

	soft_kbd_flash("lv");

	/* Interact and choose. */
	while (get_com_ex
		   (angdroid_zh_tw_enabled() ?
            "學習專精或檢視能力（l/v/Esc）？" :
            "View abilities or Learn specialty (l/v/ESC)?", &answer)) {
		/* New ability */
		if ((answer.key.code == 'L') || (answer.key.code == 'l')) {
			gain_specialty();
			break;
		}

		/* View Current */
		if ((answer.key.code == 'V') || (answer.key.code == 'v')) {
			view_abilities();
			break;
		}

		/* Exit */
		if (answer.key.code == ESCAPE)
			break;

		soft_kbd_flash("lv");
	}
}
