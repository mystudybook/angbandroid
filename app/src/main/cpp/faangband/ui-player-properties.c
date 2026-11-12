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
#include "ui-birth-history-utf8.h"
#include "ui-ability-compact-layout.h"

/**
 * ------------------------------------------------------------------------
 * Code for viewing race and class abilities
 * ------------------------------------------------------------------------ */

/* The two ability menus share a bounded list and a separately paged
 * description. Resize happens inside menu_select(), not only on entry. */
static int fa_ability_detail_page;
static int fa_ability_detail_pages;
static int fa_ability_detail_oid = -1;

static void fa_ability_layout_menu(struct menu *m)
{
    struct angdroid_fa_ability_layout plan;
    region bounds;
    int width = Term ? Term->wid : 0;
    int height = Term ? Term->hgt : 0;

    if (angdroid_fa_ability_plan(width, height, m->title != NULL,
            &plan)) {
        bounds = (region){ 0, 0, plan.width, plan.menu_rows };
    } else {
        /* A region outside the terminal makes menu_select() cancel
         * safely instead of treating a zero size as 'fill remainder'. */
        bounds = (region){ width, height, 1, 1 };
    }
    menu_layout(m, &bounds);
}

static void fa_ability_resize_menu(struct menu *m)
{
    if (!m) return;
    fa_ability_detail_page = 0;
    fa_ability_detail_oid = -1;
    fa_ability_layout_menu(m);
}

static void fa_ability_draw_description(const char *description,
    const region *loc, int oid, uint8_t attr)
{
    size_t starts[256], lengths[256], count, index;
    char raw[512], visible[512], footer[128];
    int row, first, content_rows, width, page, total_pages;
    const char *text = description ? description : "";
    int i;

    if (!Term || !loc) return;
    first = loc->row + loc->page_rows + 1;
    content_rows = Term->hgt - first - 1;
    width = MIN(Term->wid, 80);
    if (width < 16 || content_rows < 1 || first < 0) return;

    if (fa_ability_detail_oid != oid) {
        fa_ability_detail_oid = oid;
        fa_ability_detail_page = 0;
    }
    count = angdroid_fa_history_wrap(text, (size_t)width,
        starts, lengths, N_ELEMENTS(starts));
    total_pages = MAX(1, (int)((count + (size_t)content_rows - 1) /
        (size_t)content_rows));
    page = angdroid_fa_ability_page_clamp(
        fa_ability_detail_page, count, content_rows);
    fa_ability_detail_page = page;
    fa_ability_detail_pages = total_pages;

    /* Repaint only the detail region, never the selectable list. */
    for (row = first; row < Term->hgt; ++row)
        Term_erase(0, row, Term->wid);
    for (i = 0; i < content_rows; ++i) {
        index = (size_t)page * (size_t)content_rows + (size_t)i;
        if (index >= count) break;
        angdroid_fa_history_copy_line(raw, sizeof(raw), text,
            starts[index], lengths[index]);
        angdroid_menu_copy_columns(visible, sizeof(visible), raw,
            (size_t)width);
        if (visible[0])
            c_put_str(attr, visible, first + i, 0);
    }

    if (total_pages > 1) {
        strnfmt(footer, sizeof(footer), angdroid_zh_tw_enabled() ?
            "說明 %d/%d  [ 上一頁  ] 下一頁" :
            "Description %d/%d  [ prev  ] next",
            page + 1, total_pages);
    } else {
        my_strcpy(footer, angdroid_zh_tw_enabled() ?
            "Esc 返回／方向鍵選擇能力" : "Esc back / arrows select",
            sizeof(footer));
    }
    angdroid_menu_copy_columns(visible, sizeof(visible), footer,
        (size_t)width);
    c_put_str(COLOUR_WHITE, visible, Term->hgt - 1, 0);
}

static bool fa_ability_handle_page(struct menu *m,
    const ui_event *event, int oid)
{
    int direction;
    (void)m;
    (void)oid;
    if (!event || event->type != EVT_KBRD) return false;
    if (event->key.code == '[') direction = -1;
    else if (event->key.code == ']') direction = 1;
    else return false;
    fa_ability_detail_page += direction;
    if (fa_ability_detail_page < 0) fa_ability_detail_page = 0;
    if (fa_ability_detail_page >= fa_ability_detail_pages)
        fa_ability_detail_page = MAX(0, fa_ability_detail_pages - 1);
    return true;
}

/* The status row is outside the menu list. Its two halves also act
 * as previous/next buttons on a touchscreen. */
static bool fa_ability_page_touch(struct menu *m,
    const ui_event *event, ui_event *out)
{
    ui_event key = EVENT_EMPTY;
    (void)out;
    if (!m || !Term || !event || event->type != EVT_MOUSE ||
        event->mouse.y != Term->hgt - 1 ||
        event->mouse.x < 0 || event->mouse.x >= Term->wid ||
        fa_ability_detail_pages < 2) return false;
    key.type = EVT_KBRD;
    key.key.code = event->mouse.x < Term->wid / 2 ? '[' : ']';
    return fa_ability_handle_page(m, &key, m->cursor);
}


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

    fa_ability_draw_description(
        angdroid_ability_description(choices[oid].desc,
            angdroid_zh_tw_enabled()),
        loc, oid, COLOUR_L_BLUE);
}

/**
 * Display list available specialties.
 */
void textui_view_ability_menu(struct player_ability *ability_list,
							  int num_abilities)
{
	struct menu menu;
	menu_iter menu_f = { view_ability_tag, 0, view_ability_display,
        fa_ability_handle_page, fa_ability_resize_menu };
	char buf[160];
    int last_shortcut;

    if (!ability_list || num_abilities <= 0 || !Term ||
        !all_letters_nohjkl[0] || Term->wid < 16 || Term->hgt < 12) return;
    fa_ability_detail_page = 0;
    fa_ability_detail_pages = 0;
    fa_ability_detail_oid = -1;
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
    menu.cmd_keys = "[]";
    menu.context_hook = fa_ability_page_touch;
    fa_ability_layout_menu(&menu);
    clear_from(0);

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
    return oid >= 0 && (size_t)oid < strlen(lower_case) ?
        I2A(oid) : 0;
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

    if (fa_ability_handle_page(menu, e, oid)) return true;
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

    fa_ability_draw_description(
        angdroid_ability_description(ability->desc,
            angdroid_zh_tw_enabled()),
        loc, oid, COLOUR_DEEP_L_BLUE);
}

/**
 * Display list available specialties.
 */
bool textui_gain_spec_menu(int *pick)
{
	struct menu menu;
	menu_iter menu_f = { gain_spec_tag, 0, gain_spec_display,
        gain_spec_action, fa_ability_resize_menu };
	ui_event evt = { 0 };
	struct spec_menu_data *d = mem_alloc(sizeof *d);
	size_t i;

	char buf[192];

    if (!Term || Term->wid < 16 || Term->hgt < 12 || !pick) {
        mem_free(d);
        return false;
    }
    fa_ability_detail_page = 0;
    fa_ability_detail_pages = 0;
    fa_ability_detail_oid = -1;

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
            I2A(0), I2A(MIN(d->spec_known,
                (int)strlen(lower_case)) - 1),
            player->upkeep->new_specialties);

	/* Set up the menu */
	menu_init(&menu, MN_SKIN_SCROLL, &menu_f);
	menu.title = buf;
	menu_setpriv(&menu, d->spec_known, d);
	loc.page_rows = d->spec_known + 2;
	menu.browse_hook = gain_spec_menu_browser;
	menu.flags = MN_DBL_TAP;
    menu.cmd_keys = "[]";
    menu.context_hook = fa_ability_page_touch;
    fa_ability_layout_menu(&menu);

	while (1) {
		evt = menu_select(&menu, EVT_SELECT, true);
		if (evt.type == EVT_ESCAPE) {
			break;
		}
		if (d->selected_spec) {
			int idx = d->selected_spec;
			struct player_ability *ability = lookup_ability("player", idx, 0);
            fa_ability_layout_menu(&menu);
            menu_refresh(&menu, false);
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
