/**
 * \file ui-birth.c
 * \brief Text-based user interface for character creation
 *
 * Copyright (c) 1987 - 2015 Angband contributors
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
#include "cmds.h"
#include "cmd-core.h"
#include "game-event.h"
#include "game-input.h"
#include "game-world.h"
#include "obj-tval.h"
#include "player.h"
#include "player-birth.h"
#include "player-spell.h"
#include "ui-birth.h"
#include "ui-display.h"
#include "ui-game.h"
#include "ui-help.h"
#include "ui-input.h"
#include "../angband/ui-input-utf8.h"
#include "ui-birth-history-utf8.h"
#include "ui-menu.h"
#include "ui-options.h"
#include "ui-player.h"
#include "ui-prefs.h"
#include "ui-target.h"

#include "droid.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/menu-text-fit.h"

/* Only visible birth hints change: original character-generation
 * commands and their letter shortcuts remain canonical. */
static void fa_birth_prompt_centered(const char *message, int row)
{
    int columns, x;
    char visible[256];
    if (!Term || !message || Term->wid <= 0 ||
        row < 0 || row >= Term->hgt) return;
    columns = (int)angdroid_utf8_display_columns(message);
    x = columns >= Term->wid ? 0 : (Term->wid - columns) / 2;
    angdroid_menu_copy_columns(visible, sizeof(visible), message,
        (size_t)(Term->wid - x));
    prt("", row, 0);
    if (visible[0]) put_str(visible, row, x);
}

/* A one-line prompt should not flow past the end of a narrow terminal.
 * This helper owns only display output, never key selections or game data. */
static void fa_birth_prompt_left(const char *message, int row)
{
    char visible[256];
    if (!Term || !message || Term->wid <= 0 ||
        row < 0 || row >= Term->hgt) return;
    angdroid_menu_copy_columns(visible, sizeof(visible),
        message, (size_t)Term->wid);
    prt("", row, 0);
    if (visible[0]) prt(visible, row, 0);
}

/**
 * Overview
 * ========
 * This file implements the user interface side of the birth process
 * for the classic terminal-based UI of Angband.
 *
 * It models birth as a series of steps which must be carried out in 
 * a specified order, with the option of stepping backwards to revisit
 * past choices.
 *
 * It starts when we receive the EVENT_ENTER_BIRTH event from the game,
 * and ends when we receive the EVENT_LEAVE_BIRTH event.  In between,
 * we will repeatedly be asked to supply a game command, which change
 * the state of the character being rolled.  Once the player is happy
 * with their character, we send the CMD_ACCEPT_CHARACTER command.
 */

/**
 * A local-to-this-file global to hold the most important bit of state
 * between calls to the game proper.  Probably not strictly necessary,
 * but reduces complexity a bit. */
enum birth_stage
{
	BIRTH_BACK = -1,
	BIRTH_RESET = 0,
	BIRTH_QUICKSTART,
	BIRTH_MAP_CHOICE,
	BIRTH_RACE_CHOICE,
	BIRTH_CLASS_CHOICE,
	BIRTH_ROLLER_CHOICE,
	BIRTH_POINTBASED,
	BIRTH_ROLLER,
	BIRTH_NAME_CHOICE,
	BIRTH_HISTORY_CHOICE,
	BIRTH_FINAL_CONFIRM,
	BIRTH_COMPLETE
};


enum birth_questions
{
	BQ_METHOD = 0,
	BQ_RACE,
	BQ_CLASS,
	BQ_ROLLER,
	MAX_BIRTH_QUESTIONS
};

enum birth_rollers
{
	BR_POINTBASED = 0,
	BR_NORMAL,
	MAX_BIRTH_ROLLERS
};


static void finish_with_random_choices(enum birth_stage current);
static void point_based_start(void);
static bool quickstart_allowed = false;
bool arg_force_name;

/**
 * ------------------------------------------------------------------------
 * Quickstart? screen.
 * ------------------------------------------------------------------------ */
static enum birth_stage textui_birth_quickstart(void)
//phantom name change changes
{
	const char *prompt = angdroid_game_ui_text(
        "['Y': use as is; 'N': redo; 'C': change name/history; '=': set birth options]",
        angdroid_zh_tw_enabled());

	enum birth_stage next = BIRTH_QUICKSTART;

	/* Prompt for it */
	fa_birth_prompt_left(angdroid_game_ui_text(
        "New character based on previous one:",
        angdroid_zh_tw_enabled()), 0);
	fa_birth_prompt_centered(prompt, Term->hgt - 1);

	do {
		soft_kbd_flash("YNC=\x18");

		/* Get a key */
		struct keypress ke = inkey();
		
		if (ke.code == 'N' || ke.code == 'n') {
			cmdq_push(CMD_BIRTH_RESET);
			next = BIRTH_MAP_CHOICE;
		} else if (ke.code == KTRL('X')) {
			quit(NULL);
		} else if ( !arg_force_name && (ke.code == 'C' || ke.code == 'c')) {
			next = BIRTH_NAME_CHOICE;
		} else if (ke.code == '=') {
			do_cmd_options_birth();
		} else if (ke.code == 'Y' || ke.code == 'y') {
			cmdq_push(CMD_ACCEPT_CHARACTER);
			next = BIRTH_COMPLETE;
		}
	} while (next == BIRTH_QUICKSTART);

	/* Clear prompt */
	clear_from(23);

	return next;
}


/**
 * ------------------------------------------------------------------------
 * Set the map
 * ------------------------------------------------------------------------ */
#define MAP_TEXT \
    "    *    \n" \
    "  #{light yellow}#{/} {light yellow}#{/}#  \n" \
    " #{light green}#{/}{light yellow}#{/} {light yellow}#{/}{light green}#{/}# \n" \
    "/{light yellow}##{/}{deep light blue}/{/}{yellow}@{/}{deep light blue}\\{/}{light yellow}##{/}\\\n" \
    "*  {yellow}@{/}{orange}@{/}{yellow}@{/}  *  Welcome to First Age Angband!\n" \
    "\\{light yellow}##{/}{deep light blue}\\{/}{yellow}@{/}{deep light blue}/{/}{light yellow}##{/}/\n" \
    " #{light green}#{/}{light yellow}#{/} {light yellow}#{/}{light green}#{/}# \n" \
    "  #{light yellow}#{/} {light yellow}#{/}#  \n" \
    "    *    \n\n"				     \
    "There are different ways of structuring the world\n" \
    "of FAangband; '{light green}?{/}' gives more details."

#define MAP_TEXT_ZH \
    "    *    \n" \
    "  #{light yellow}#{/} {light yellow}#{/}#  \n" \
    " #{light green}#{/}{light yellow}#{/} {light yellow}#{/}{light green}#{/}# \n" \
    "/{light yellow}##{/}{deep light blue}/{/}{yellow}@{/}{deep light blue}\\{/}{light yellow}##{/}\\\n" \
    "*  {yellow}@{/}{orange}@{/}{yellow}@{/}  *  歡迎來到第一紀元 Angband！\n" \
    "\\{light yellow}##{/}{deep light blue}\\{/}{yellow}@{/}{deep light blue}/{/}{light yellow}##{/}/\n" \
    " #{light green}#{/}{light yellow}#{/} {light yellow}#{/}{light green}#{/}# \n" \
    "  #{light yellow}#{/} {light yellow}#{/}#  \n" \
    "    *    \n\n"				     \
    "FAangband 有多種世界地圖配置\n" \
    "可選擇；按 '{light green}?{/}' 查看說明。"

/**
 * Show the map instructions
 */
static void print_map_instructions(void)
{
    int saved_wrap = text_out_wrap;
    int saved_indent = text_out_indent;
    int indent;
    if (!Term || Term->wid <= 0 || Term->hgt <= 1) return;

    Term_clear();
    text_out_hook = text_out_to_screen;
    indent = MIN(5, MAX(0, Term->wid - 2));
    text_out_indent = indent;
    text_out_wrap = Term->wid > 1 ? Term->wid - 1 : 0;
    Term_gotoxy(indent, 1);

    text_out_e(angdroid_zh_tw_enabled() ? MAP_TEXT_ZH : MAP_TEXT);

    text_out_wrap = saved_wrap;
    text_out_indent = saved_indent;
}

/**
 * Current map and index
 */
int num_maps;
int current_map_index;


/**
 * Map menu data struct
 */
struct map_menu_data {
	const char *name;
	const char *description;
};


/**
 * Display a row of the map menu
 */
static void map_menu_display(struct menu *m, int oid, bool cursor,
							 int row, int col, int wid)
{
	struct map_menu_data *d = menu_priv(m);
	int attr_name = cursor ? COLOUR_L_BLUE : COLOUR_WHITE;

    /* Display-only map title; preserve the map->name value and oid. */
    if (Term && col >= 0 && col < Term->wid && wid > 0) {
        char visible[256];
        const char *name = angdroid_game_ui_text(d[oid].name,
            angdroid_zh_tw_enabled());
        angdroid_menu_copy_columns(visible, sizeof(visible), name,
            (size_t)MIN(wid, Term->wid - col));
        if (visible[0]) c_put_str(attr_name, visible, row, col);
    }
}

/**
 * Handle an event on a menu row.
 */
static bool map_menu_handler(struct menu *m, const ui_event *e, int oid)
{
	/* Select */
	if (e->type == EVT_SELECT) {
		current_map_index = oid;
		return false;
	} else if (e->type == EVT_ESCAPE) {
		current_map_index = -1;
		return false;
	} else if (e->type == EVT_KBRD) {
		/* Get help */
        if (e->key.code == '?') {
            screen_save();
            show_file("mapmode.txt", NULL, 0, 0);
            screen_load();
            /* Help is informational: keep the map picker active, and
             * don't accidentally accept an unconfirmed map. */
            return true;
        }
        return false;
	}
	return true;
}

/**
 * Show map description when browsing
 */
static void map_menu_browser(int oid, void *data, const region *loc)
{
    struct map_menu_data *d = data;
    size_t starts[128], lengths[128], lines, show, max_lines;
    const char *description;
    int col, row, width;
    size_t i;

    if (!Term || Term->wid <= 0 || Term->hgt <= 0 || !loc ||
            oid < 0 || oid >= num_maps) return;
    col = MAX(0, MIN(loc->col, Term->wid - 1));
    /* Leave a spacer between choices and the first description line. */
    row = loc->row + loc->page_rows + 1;
    if (row >= Term->hgt) return;
    width = MIN(MAX(1, loc->width), Term->wid - col);
    if (width <= 0) return;

    description = angdroid_game_ui_text(d[oid].description,
        angdroid_zh_tw_enabled());
    if (!description) description = "";
    /* text_out_c() could wrap indefinitely into rows below the screen.
     * Instead wrap each complete UTF-8 scalar by actual terminal columns
     * and paint at most the available two or three preview lines. */
    lines = angdroid_fa_history_wrap(description, (size_t)width,
        starts, lengths, N_ELEMENTS(starts));
    max_lines = MIN((size_t)3, (size_t)(Term->hgt - row));
    show = MIN(lines, max_lines);
    for (i = 0; i < max_lines; ++i)
        Term_erase(col, row + (int)i, width);
    for (i = 0; i < show; ++i) {
        char uncut[512], visible[512];
        angdroid_fa_history_copy_line(uncut, sizeof(uncut),
            description, starts[i], lengths[i]);
        angdroid_menu_copy_columns(visible, sizeof(visible),
            uncut, (size_t)width);
        if (visible[0])
            c_put_str(COLOUR_DEEP_L_BLUE, visible,
                row + (int)i, col);
    }
    /* The full original map description remains available in the
     * game data; presentation never mutates it or its selection index. */
}

/* Recompute the map chooser geometry after a phone rotation. Native
 * region math clamps the viewport, but does not choose a new width/row
 * for this map-specific explanatory picker. */
static void fa_birth_map_menu_resize(struct menu *m)
{
    region loc = { 5, 14, 70, -99 };
    if (!m || !Term || Term->wid <= 0 || Term->hgt <= 0) return;
    loc.col = MIN(5, MAX(0, Term->wid / 8));
    loc.row = MIN(14, MAX(0, Term->hgt / 3));
    loc.width = MAX(1, MIN(70, Term->wid - loc.col - 1));
    loc.page_rows = MIN(m->count + 1,
        MAX(1, Term->hgt - loc.row - 3));
    menu_layout(m, &loc);
}

static const menu_iter map_menu_iter = {
	NULL,						/* get_tag = NULL, use lowercase selections */
	NULL,						/* no validity hook */
	map_menu_display,
	map_menu_handler,
    fa_birth_map_menu_resize /* rotate or resize */
};

/**
 * Create and initialise the map menu
 */
static struct menu *map_menu_new(void)
{
	struct menu *m = menu_new(MN_SKIN_SCROLL, &map_menu_iter);
	struct level_map *map;
	struct map_menu_data *d;
	int i;

    region loc = { 5, 14, 70, -99 };

	current_map_index = -1;
	num_maps = 0;

	/* Count the maps */
	for (map = maps; map; map = map->next) {
		if (world && map == world) {
			current_map_index = num_maps;
		}
		num_maps++;
	}
	num_maps++;

	/* Allocate menu array */
	d = mem_zalloc(num_maps * sizeof(*d));
	for (i = 0, map = maps; i < num_maps; i++) {
		if (map) {
			d[i].name = map->name;
			d[i].description = map->help;
			map = map->next;
		} else {
			d[i].name = "Quit";
			d[i].description = angdroid_zh_tw_enabled() ?
                "離開 FAangband。" : "Quit FAangband.";
		}
	}
	menu_setpriv(m, num_maps, d);

	/* Set flags */
	m->cmd_keys = "?";
	m->flags = MN_CASELESS_TAGS;
	m->selections = lower_case;
	m->browse_hook = map_menu_browser;

    /* Leave room for a selectable map list and the description on
     * narrow Android terminals; keep the selected map index unchanged. */
    if (Term && Term->wid > 0 && Term->hgt > 0) {
        loc.col = MIN(5, MAX(0, Term->wid / 8));
        loc.row = MIN(14, MAX(0, Term->hgt / 3));
        loc.width = MAX(1, MIN(70, Term->wid - loc.col - 1));
        loc.page_rows = MIN(num_maps + 1,
            MAX(1, Term->hgt - loc.row - 3));
    } else {
        loc.page_rows = num_maps + 1;
    }
    menu_layout(m, &loc);

	return m;
}


/**
 * Clean up a map menu instance
 */
static void map_menu_destroy(struct menu *m)
{
	struct map_menu_data *d = menu_priv(m);
	mem_free(d);
	mem_free(m);
}

/**
 * Run the map menu to select a map.
 */
static int map_menu_select(struct menu *m)
{
    ui_event result;
    int confirmed = -1;

    screen_save();
    region_erase_bordered(&m->active);
    result = menu_select(m, 0, true);
    /* The initial cursor may highlight a previous world; ESC or an
     * aborted/resized menu must never silently select that world. */
    if (result.type == EVT_SELECT && current_map_index >= 0 &&
            current_map_index < num_maps)
        confirmed = current_map_index;
    screen_load();
    current_map_index = confirmed;
    return confirmed;
}

/**
 * Choose which map to play on
 */
static enum birth_stage get_map_command(void)
{
	enum birth_stage next = BIRTH_BACK;
	struct menu *m;

	m = map_menu_new();
	if (m) {
		int map = map_menu_select(m);
		if (map != -1) {
			if (map == num_maps - 1) {
				quit(NULL);
			} else {
				cmdq_push(CMD_CHOOSE_MAP);
				cmd_set_arg_choice(cmdq_peek(), "choice", map);
				next = BIRTH_RACE_CHOICE;
			}
		}
	}
	map_menu_destroy(m);

	return next;
}

/**
 * ------------------------------------------------------------------------
 * The various "menu" bits of the birth process - namely choice of race,
 * class, and roller type.
 * ------------------------------------------------------------------------ */

/**
 * The various menus
 */
static struct menu race_menu, class_menu, roller_menu;

/**
 * Locations of the menus, etc. on the screen
 */
#define HEADER_ROW       1
#define QUESTION_ROW     7
#define TABLE_ROW        9

#define QUESTION_COL     2
#define RACE_COL         2
#define RACE_AUX_COL    19
#define CLASS_COL       19
#define CLASS_AUX_COL   36
#define ROLLER_COL      36
#define HIST_INSTRUCT_ROW 18

#define MENU_ROWS TABLE_ROW + 14

/**
 * upper left column and row, width, and lower column
 */
static region race_region = {RACE_COL, TABLE_ROW, 17, MENU_ROWS};
static region class_region = {CLASS_COL, TABLE_ROW, 17, MENU_ROWS};
static region roller_region = {ROLLER_COL, TABLE_ROW, 34, MENU_ROWS};

/* Landscape terminals keep the traditional three-column birth sheet.
 * On portrait terminals each step gets a focused, selectable menu with
 * its detail pane to the right; prior steps are available via ESC. */
static bool fa_birth_compact_layout(void)
{
    return Term && Term->wid > 0 && Term->wid < 76;
}

static int fa_birth_detail_col(int normal_col)
{
    if (!fa_birth_compact_layout()) return normal_col;
    return MIN(MAX(0, Term->wid - 1),
        race_region.col + race_region.width + 1);
}

static void fa_birth_configure_layout(void)
{
    int menu_width, rows;
    if (!fa_birth_compact_layout()) {
        race_region = (region){RACE_COL, TABLE_ROW, 17, MENU_ROWS};
        class_region = (region){CLASS_COL, TABLE_ROW, 17, MENU_ROWS};
        roller_region = (region){ROLLER_COL, TABLE_ROW, 34, MENU_ROWS};
        return;
    }
    if (Term->wid < 50) {
        /* Stack detail below the selection list on very narrow phones. */
        menu_width = MAX(1, MIN(36, Term->wid - RACE_COL - 1));
        rows = MAX(1, MIN(6, Term->hgt - TABLE_ROW - 7));
    } else {
        menu_width = MIN(19, MAX(6, Term->wid / 2 - 3));
        rows = MAX(1, MIN(14, Term->hgt - TABLE_ROW - 1));
    }
    race_region = (region){RACE_COL, TABLE_ROW, menu_width, rows};
    class_region = race_region;
    roller_region = race_region;
}

/* Display only: never changes enum birth_stage or the race/class IDs. */


/**
 * We use different menu "browse functions" to display the help text
 * sometimes supplied with the menu items - currently just the list
 * of bonuses, etc, corresponding to each race and class.
 */
typedef void (*browse_f) (int oid, void *db, const region *l);

/**
 * We have one of these structures for each menu we display - it holds
 * the useful information for the menu - text of the menu items, "help"
 * text, current (or default) selection, whether random selection is allowed,
 * and the current stage of the process for setting up a context menu and
 * relaying the reuslt of a selection in that menu.
 */
struct birthmenu_data 
{
	const char **items;
	const char *hint;
	bool allow_random;
	enum birth_stage stage_inout;
};

/**
 * A custom "display" function for our menus that simply displays the
 * text from our stored data in a different colour if it's currently
 * selected.
 */
static void birthmenu_display(struct menu *menu, int oid, bool cursor,
			      int row, int col, int width)
{
	struct birthmenu_data *data = menu->menu_data;

	uint8_t attr = curs_attrs[CURS_KNOWN][0 != cursor];
    /* Only visible words are localized; selection ID is unchanged. */
    if (Term && col >= 0 && col < Term->wid && width > 0) {
        char visible[256];
        const char *label = angdroid_game_ui_text(data->items[oid],
            angdroid_zh_tw_enabled());
        angdroid_menu_copy_columns(visible, sizeof(visible), label,
            (size_t)MIN(width, Term->wid - col));
        if (visible[0]) c_put_str(attr, visible, row, col);
    }
}

/**
 * Our custom menu iterator, only really needed to allow us to override
 * the default handling of "commands" in the standard iterators (hence
 * only defining the display and handler parts).
 */
static const menu_iter birth_iter = { NULL, NULL, birthmenu_display, NULL, NULL };

static void skill_help(const int r_skills[], const int c_skills[], int mhp, int infra)
{
	int16_t skills[SKILL_MAX];
	unsigned i;

	for (i = 0; i < SKILL_MAX ; ++i)
		skills[i] = (r_skills ? r_skills[i] : 0 ) + (c_skills ? c_skills[i] : 0);

	text_out_e(angdroid_zh_tw_enabled() ?
        "近戰／射擊／投擲：%+d/%+d/%+d  生命骰：%2d\n" : "Hit/Shoot/Throw: %+d/%+d/%+d  Hit die: %2d\n",
			   skills[SKILL_TO_HIT_MELEE], skills[SKILL_TO_HIT_BOW],
			   skills[SKILL_TO_HIT_THROW], mhp);
	text_out_e(angdroid_zh_tw_enabled() ?
        "解除陷阱：%+3d/%+3d  魔法裝置：%+3d\n" : "Disarm: %+3d/%+3d   Devices: %+3d\n", skills[SKILL_DISARM_PHYS],
			   skills[SKILL_DISARM_MAGIC], skills[SKILL_DEVICE]);
	text_out_e(angdroid_zh_tw_enabled() ?
        "豁免：%+3d  潛行：%+3d\n" : "Save:       %+3d   Stealth: %+3d\n", skills[SKILL_SAVE],
			   skills[SKILL_STEALTH]);
	text_out_e(angdroid_zh_tw_enabled() ?
        "挖掘：%+3d  搜索：%+3d\n" : "Digging:    %+3d   Search:  %+3d\n", skills[SKILL_DIGGING],
			   skills[SKILL_SEARCH]);
	if (infra >= 0)
		text_out_e(angdroid_zh_tw_enabled() ?
            "夜視：%d 英尺\n" : "Infravision:  %d ft\n",
            infra * 10);
}

/* Show race/class basics beside the list on medium phones, or below
 * the list on narrow ones. Each row has a bounded terminal-cell budget
 * and cannot overwrite the current selection or bottom key hints. */
static int fa_birth_help_start(void)
{
    return Term && Term->wid < 50 ?
        TABLE_ROW + race_region.page_rows + 1 : TABLE_ROW;
}

static int fa_birth_help_col(void)
{
    return Term && Term->wid < 50 ?
        QUESTION_COL : fa_birth_detail_col(RACE_AUX_COL);
}

static void fa_birth_help_clear(void)
{
    int row, col;
    if (!Term || Term->wid <= 0 || Term->hgt <= 0) return;
    row = fa_birth_help_start();
    col = fa_birth_help_col();
    if (col < 0 || col >= Term->wid) return;
    for (; row < Term->hgt - 1; ++row)
        Term_erase(col, row, Term->wid - col);
}

static void fa_birth_help_line(int *row, const char *label)
{
    int col, width;
    char visible[384];
    if (!row) return;
    col = fa_birth_help_col();
    if (Term && label && col >= 0 && col < Term->wid &&
            *row >= 0 && *row < Term->hgt - 1) {
        width = Term->wid - col;
        angdroid_menu_copy_columns(visible, sizeof(visible),
            label, (size_t)width);
        if (visible[0]) c_put_str(COLOUR_L_BLUE, visible, *row, col);
    }
    ++*row;
}

static void fa_birth_compact_details(const struct player_race *race,
    const struct player_class *clazz)
{
    int line, i, adj[STAT_MAX], skills[SKILL_MAX];
    struct player_ability *ability;
    bool zh = angdroid_zh_tw_enabled();
    if (!Term || !race) return;
    line = fa_birth_help_start();
    fa_birth_help_clear();
    if (clazz) {
        fa_birth_help_line(&line, zh ?
            format("職業：%s", angdroid_game_ui_text(clazz->name, true)) :
            format("Class: %s", clazz->name));
    } else {
        fa_birth_help_line(&line, zh ?
            format("種族：%s", angdroid_game_ui_text(race->name, true)) :
            format("Race: %s", race->name));
    }

    for (i = 0; i < STAT_MAX; ++i)
        adj[i] = race->r_adj[i] + (clazz ? clazz->c_adj[i] : 0);
    for (i = 0; i < SKILL_MAX; ++i)
        skills[i] = race->r_skills[i] + (clazz ? clazz->c_skills[i] : 0);

    for (i = 0; i < (STAT_MAX + 1) / 2; ++i) {
        int second = i + (STAT_MAX + 1) / 2;
        if (second < STAT_MAX) {
            fa_birth_help_line(&line, format("%s %+d  %s %+d",
                stat_names_reduced[i], adj[i],
                stat_names_reduced[second], adj[second]));
        } else {
            fa_birth_help_line(&line, format("%s %+d",
                stat_names_reduced[i], adj[i]));
        }
    }
    fa_birth_help_line(&line, zh ?
        format("近戰 %+d  射擊 %+d", skills[SKILL_TO_HIT_MELEE],
            skills[SKILL_TO_HIT_BOW]) :
        format("Melee %+d  Bow %+d", skills[SKILL_TO_HIT_MELEE],
            skills[SKILL_TO_HIT_BOW]));
    fa_birth_help_line(&line, zh ?
        format("生命骰 %+d  投擲 %+d",
            race->r_mhp + (clazz ? clazz->c_mhp : 0),
            skills[SKILL_TO_HIT_THROW]) :
        format("Hit die %+d  Throw %+d",
            race->r_mhp + (clazz ? clazz->c_mhp : 0),
            skills[SKILL_TO_HIT_THROW]));
    fa_birth_help_line(&line, zh ?
        format("豁免 %+d  潛行 %+d", skills[SKILL_SAVE],
            skills[SKILL_STEALTH]) :
        format("Save %+d  Stealth %+d", skills[SKILL_SAVE],
            skills[SKILL_STEALTH]));
    fa_birth_help_line(&line, zh ?
        format("解除 %+d/%+d  裝置 %+d",
            skills[SKILL_DISARM_PHYS], skills[SKILL_DISARM_MAGIC],
            skills[SKILL_DEVICE]) :
        format("Disarm %+d/%+d Device %+d",
            skills[SKILL_DISARM_PHYS], skills[SKILL_DISARM_MAGIC],
            skills[SKILL_DEVICE]));
    fa_birth_help_line(&line, zh ?
        format("挖掘 %+d  搜索 %+d",
            skills[SKILL_DIGGING], skills[SKILL_SEARCH]) :
        format("Dig %+d  Search %+d",
            skills[SKILL_DIGGING], skills[SKILL_SEARCH]));

    if (!clazz) {
        fa_birth_help_line(&line, zh ?
            format("夜視 %d 英尺", race->infra * 10) :
            format("Infravision %d ft", race->infra * 10));
        if (world && world->name && strstr(world->name, "Wilderness"))
            fa_birth_help_line(&line, zh ?
                format("難度 %d  故鄉 %s", race->difficulty,
                    angdroid_game_ui_text(race->hometown, true)) :
                format("Difficulty %d  Home %s", race->difficulty,
                    race->hometown));
    } else if (clazz->magic.total_spells) {
        int count;
        struct magic_realm *realm = class_magic_realms(clazz, &count);
        while (realm) {
            struct magic_realm *next = realm->next;
            fa_birth_help_line(&line, zh ?
                format("魔法：%s", angdroid_game_ui_text(realm->name, true)) :
                format("Magic: %s", realm->name));
            mem_free(realm);
            realm = next;
        }
    }
    for (ability = player_abilities; ability; ability = ability->next) {
        if (streq(ability->type, "object") &&
                !(clazz ? of_has(clazz->flags, ability->index) :
                    of_has(race->flags, ability->index))) continue;
        if (streq(ability->type, "player") &&
                !(clazz ? pf_has(clazz->pflags, ability->index) :
                    pf_has(race->pflags, ability->index))) continue;
        if (streq(ability->type, "element") &&
                (clazz || race->el_info[ability->index].res_level !=
                    ability->value)) continue;
        fa_birth_help_line(&line, angdroid_game_ui_text(ability->name, zh));
    }
    if (Term && line > Term->hgt - 1 &&
            Term->hgt - 2 >= fa_birth_help_start()) {
        int col = fa_birth_help_col();
        char visible[192];
        const char *hint = zh ? "…橫向可查看更多特性" :
            "...rotate for more traits";
        if (col >= 0 && col < Term->wid) {
            Term_erase(col, Term->hgt - 2, Term->wid - col);
            angdroid_menu_copy_columns(visible, sizeof(visible),
                hint, (size_t)(Term->wid - col));
            if (visible[0])
                c_put_str(COLOUR_YELLOW, visible,
                    Term->hgt - 2, col);
        }
    }
}

static void race_help(int i, void *db, const region *l)
{
    int detail_col = fa_birth_detail_col(RACE_AUX_COL);
    int saved_wrap = text_out_wrap;
	int j;
	struct player_race *r = player_id2race(i);
	int len = (STAT_MAX + 1) / 2;

	struct player_ability *ability;
	int n_flags = 0;
	int flag_space = 3;

	if (!r) return;
    if (fa_birth_compact_layout()) {
        fa_birth_compact_details(r, NULL);
        return;
    }

	/* Output to the screen */
	text_out_hook = text_out_to_screen;
	
	/* Indent output */
	text_out_indent = detail_col;
    if (fa_birth_compact_layout() && Term->wid > detail_col + 2)
        text_out_wrap = Term->wid - 1;
    Term_gotoxy(detail_col, TABLE_ROW);

	for (j = 0; j < len; j++) {  
		const char *name = stat_names_reduced[j];
		int adj = r->r_adj[j];

		text_out_e("%s%+3d", name, adj);

		if (j * 2 + 1 < STAT_MAX) {
			name = stat_names_reduced[j + len];
			adj = r->r_adj[j + len];
			text_out_e("  %s%+3d", name, adj);
		}

		text_out("\n");
	}
	
	text_out_e("\n");
	skill_help(r->r_skills, NULL, r->r_mhp, r->infra);
	if (strstr(world->name, "Wilderness")) {
		uint8_t color = COLOUR_RED;
		text_out_e(angdroid_zh_tw_enabled() ?
            "難度：等級 %d\n" : "Difficulty: Level %d\n",
            r->difficulty);

		/* Color code difficulty factor */
		if (r->difficulty < 3) {
			color = COLOUR_GREEN;
		} else if (r->difficulty < 15) {
			color = COLOUR_ORANGE;
		}

		text_out_c(color, angdroid_zh_tw_enabled() ?
            "故鄉：%s" : "Home town: %-15s",
            angdroid_game_ui_text(r->hometown, angdroid_zh_tw_enabled()));
	}
	text_out_e("\n");

	for (ability = player_abilities; ability; ability = ability->next) {
		if (n_flags >= flag_space) break;
		if (streq(ability->type, "object") &&
			!of_has(r->flags, ability->index)) {
			continue;
		} else if (streq(ability->type, "player") &&
				   !pf_has(r->pflags, ability->index)) {
			continue;
		} else if (streq(ability->type, "element") &&
				   (r->el_info[ability->index].res_level != ability->value)) {
			continue;
		}
		text_out_e("\n%s", angdroid_game_ui_text(ability->name,
            angdroid_zh_tw_enabled()));
		n_flags++;
	}

	while (n_flags < flag_space) {
		text_out_e("\n");
		n_flags++;
	}

    /* Do not leak a phone-specific wrap setting into the next screen. */
    text_out_indent = 0;
    text_out_wrap = saved_wrap;
}

static void class_help(int i, void *db, const region *l)
{
    int detail_col = fa_birth_detail_col(CLASS_AUX_COL);
    int saved_wrap = text_out_wrap;
	int j;
	struct player_class *c = player_id2class(i);
	const struct player_race *r = player->race;
	int len = (STAT_MAX + 1) / 2;

	struct player_ability *ability;
	int n_flags = 0;
	int flag_space = 5;

	if (!c) return;
    if (fa_birth_compact_layout()) {
        fa_birth_compact_details(r, c);
        return;
    }

	/* Output to the screen */
	text_out_hook = text_out_to_screen;
	
	/* Indent output */
	text_out_indent = detail_col;
    if (fa_birth_compact_layout() && Term->wid > detail_col + 2)
        text_out_wrap = Term->wid - 1;
    Term_gotoxy(detail_col, TABLE_ROW);

	for (j = 0; j < len; j++) {  
		const char *name = stat_names_reduced[j];
		int adj = c->c_adj[j] + r->r_adj[j];

		text_out_e("%s%+3d", name, adj);

		if (j*2 + 1 < STAT_MAX) {
			name = stat_names_reduced[j + len];
			adj = c->c_adj[j + len] + r->r_adj[j + len];
			text_out_e("  %s%+3d", name, adj);
		}

		text_out("\n");
	}

	text_out_e("\n");
	
	skill_help(r->r_skills, c->c_skills, r->r_mhp + c->c_mhp, -1);

	if (c->magic.total_spells) {
		int count;
		struct magic_realm *realm = class_magic_realms(c, &count), *realm_next;
        char buf[240];
        size_t used = 0;

        /* A list of Chinese magic realms is multi-byte. Safely append
         * whole UTF-8 scalars instead of possibly cutting the last name
         * in the middle when the display buffer fills. */
        used = angdroid_utf8_copy_complete(buf, sizeof(buf),
            angdroid_game_ui_text(realm->name, angdroid_zh_tw_enabled()));
        realm_next = realm->next;
        mem_free(realm);
        realm = realm_next;
        if (count > 1) {
            while (realm) {
                const char *join;
                --count;
                join = angdroid_zh_tw_enabled() ?
                    (count ? "、" : "與") :
                    (count ? ", " : " and ");
                used = angdroid_utf8_append_complete(buf, sizeof(buf),
                    used, join);
                used = angdroid_utf8_append_complete(buf, sizeof(buf),
                    used, angdroid_game_ui_text(realm->name,
                        angdroid_zh_tw_enabled()));
                realm_next = realm->next;
                mem_free(realm);
                realm = realm_next;
            }
        }
		text_out_e(angdroid_zh_tw_enabled() ?
            "\n可學習%s魔法" : "\nLearns %s magic", buf);
	}

	for (ability = player_abilities; ability; ability = ability->next) {
		if (n_flags >= flag_space) break;
		if (streq(ability->type, "object") &&
			!of_has(c->flags, ability->index)) {
			continue;
		} else if (streq(ability->type, "player") &&
				   !pf_has(c->pflags, ability->index)) {
			continue;
		} else if (streq(ability->type, "element")) {
			continue;
		}

		text_out_e("\n%s", angdroid_game_ui_text(ability->name,
            angdroid_zh_tw_enabled()));
		n_flags++;
	}

	while (n_flags < flag_space) {
		text_out_e("\n");
		n_flags++;
	}

    /* Do not leak a phone-specific wrap setting into the next screen. */
    text_out_indent = 0;
    text_out_wrap = saved_wrap;
}

/**
 * Display and handle user interaction with a context menu appropriate for the
 * current stage.  That way actions available with certain keys are also
 * available if only using the mouse.
 *
 * \param current_menu is the standard (not contextual) menu for the stage.
 * \param in is the event triggering the context menu.  in->type must be
 * EVT_MOUSE.
 * \param out is the event to be passed upstream (to internal handling in
 * menu_select() or, potentially, menu_select()'s caller).
 * \return true if the event was handled; otherwise, return false.
 *
 * The logic here overlaps with what's done to handle cmd_keys in
 * menu_question().
 */
static bool use_context_menu_birth(struct menu *current_menu,
		const ui_event *in, ui_event *out)
{
	enum {
		ACT_CTX_BIRTH_OPT,
		ACT_CTX_BIRTH_RAND,
		ACT_CTX_BIRTH_FINISH_RAND,
		ACT_CTX_BIRTH_QUIT,
		ACT_CTX_BIRTH_HELP
	};
	struct birthmenu_data *menu_data = menu_priv(current_menu);
	char *labels;
	struct menu *m;
	int selected;

	assert(in->type == EVT_MOUSE);
	if (in->mouse.y != QUESTION_ROW && in->mouse.y != QUESTION_ROW + 1) {
		return false;
	}

	labels = string_make(lower_case);
	m = menu_dynamic_new();

	m->selections = labels;
	menu_dynamic_add_label(m, angdroid_game_ui_text("Show birth options",
            angdroid_zh_tw_enabled()), '=',
		ACT_CTX_BIRTH_OPT, labels);
	if (menu_data->allow_random) {
		menu_dynamic_add_label(m, angdroid_game_ui_text("Select one at random",
            angdroid_zh_tw_enabled()), '*',
			ACT_CTX_BIRTH_RAND, labels);
	}
	menu_dynamic_add_label(m, angdroid_game_ui_text("Finish with random choices",
            angdroid_zh_tw_enabled()), '@',
		ACT_CTX_BIRTH_FINISH_RAND, labels);
	menu_dynamic_add_label(m, angdroid_game_ui_text("Quit",
            angdroid_zh_tw_enabled()), 'q', ACT_CTX_BIRTH_QUIT, labels);
	menu_dynamic_add_label(m, angdroid_game_ui_text("Help",
            angdroid_zh_tw_enabled()), '?', ACT_CTX_BIRTH_HELP, labels);

	screen_save();

	menu_dynamic_calc_location(m, in->mouse.x, in->mouse.y);
	region_erase_bordered(&m->boundary);

	selected = menu_dynamic_select(m);

	menu_dynamic_free(m);
	string_free(labels);

	screen_load();

	switch (selected) {
	case ACT_CTX_BIRTH_OPT:
		do_cmd_options_birth();
		/* The stage remains the same so leave stage_inout as is. */
		out->type = EVT_SWITCH;
		break;

	case ACT_CTX_BIRTH_RAND:
		current_menu->cursor = randint0(current_menu->count);
		out->type = EVT_SELECT;
		break;

	case ACT_CTX_BIRTH_FINISH_RAND:
		finish_with_random_choices(menu_data->stage_inout);
		menu_data->stage_inout = BIRTH_FINAL_CONFIRM;
		out->type = EVT_SWITCH;
		break;

	case ACT_CTX_BIRTH_QUIT:
		quit(NULL);
		break;

	case ACT_CTX_BIRTH_HELP:
		do_cmd_help();
		menu_data->stage_inout = BIRTH_RESET;
		out->type = EVT_SWITCH;

	default:
		/* There's nothing to do. */
		break;
	}

	return true;
}

/**
 * Set up one of our menus ready to display choices for a birth question.
 * This is slightly involved.
 */
static void init_birth_menu(struct menu *menu, int n_choices,
							int initial_choice, const region *reg,
							bool allow_random, browse_f aux)
{
	struct birthmenu_data *menu_data;

	/* Initialise a basic menu */
	menu_init(menu, MN_SKIN_SCROLL, &birth_iter);

	/* A couple of behavioural flags - we want selections as letters
	   skipping the rogue-like cardinal direction movements and a
	   double tap to act as a selection. */
	menu->selections = all_letters_nohjkl;
	menu->flags = MN_DBL_TAP;

	/* Copy across the game's suggested initial selection, etc. */
	menu->cursor = initial_choice;

	/* Allocate sufficient space for our own bits of menu information. */
	menu_data = mem_alloc(sizeof *menu_data);

	/* Allocate space for an array of menu item texts and help texts
	   (where applicable) */
	menu_data->items = mem_alloc(n_choices * sizeof *menu_data->items);
	menu_data->allow_random = allow_random;

	/* Set private data */
	menu_setpriv(menu, n_choices, menu_data);

	/* Set up the "browse" hook to display help text (where applicable). */
	menu->browse_hook = aux;

	/*
	 * All use the same hook to display a context menu so that
	 * functionality driven by keyboard input (see how cmd_keys is used
	 * in menu_question()) is also available using the mouse.
	 */
	menu->context_hook = use_context_menu_birth;

	/* Lay out the menu appropriately */
	menu_layout(menu, reg);
}



static void setup_menus(void)
{
	int i, n;
	struct player_class *c;
	struct player_race *r;

	const char *roller_choices[MAX_BIRTH_ROLLERS] = { 
		"Point-based", 
		"Standard roller" 
	};

	struct birthmenu_data *mdata;

    fa_birth_configure_layout();

	/* Count the races */
	n = 0;
	for (r = races; r; r = r->next) n++;

	/* Race menu. */
	init_birth_menu(&race_menu, n, player->race ? player->race->ridx : 0,
	                &race_region, true, race_help);
	mdata = race_menu.menu_data;

	for (i = 0, r = races; r; r = r->next, i++)
		mdata->items[r->ridx] = r->name;
	mdata->hint = angdroid_game_ui_text("Race affects stats and skills, and may confer resistances and abilities.",
        angdroid_zh_tw_enabled());

	/* Count the classes */
	n = 0;
	for (c = classes; c; c = c->next) n++;

	/* Class menu similar to race. */
	init_birth_menu(&class_menu, n, player->class ? player->class->cidx : 0,
	                &class_region, true, class_help);
	mdata = class_menu.menu_data;

	for (i = 0, c = classes; c; c = c->next, i++)
		mdata->items[c->cidx] = c->name;
	mdata->hint = angdroid_game_ui_text("Class affects stats, skills, and other character traits.",
        angdroid_zh_tw_enabled());
		
	/* Roller menu straightforward */
	init_birth_menu(&roller_menu, MAX_BIRTH_ROLLERS, 0, &roller_region, false,
					NULL);
	mdata = roller_menu.menu_data;
	for (i = 0; i < MAX_BIRTH_ROLLERS; i++)
		mdata->items[i] = angdroid_game_ui_text(
            roller_choices[i], angdroid_zh_tw_enabled());
	mdata->hint = angdroid_game_ui_text("Choose how to generate your intrinsic stats. Point-based is recommended.",
        angdroid_zh_tw_enabled());
}

/**
 * Cleans up our stored menu info when we've finished with it.
 */
static void free_birth_menu(struct menu *menu)
{
	struct birthmenu_data *data = menu->menu_data;

	if (data) {
		mem_free(data->items);
		mem_free(data);
	}
}

static void free_birth_menus(void)
{
	/* We don't need these any more. */
	free_birth_menu(&race_menu);
	free_birth_menu(&class_menu);
	free_birth_menu(&roller_menu);
}

/**
 * Clear the previous question
 */
static void clear_question(void)
{
	int i;

	for (i = QUESTION_ROW; i < TABLE_ROW; i++)
		/* Clear line, position cursor */
		Term_erase(0, i, 255);
}


#define BIRTH_MENU_HELPTEXT \
	"{light blue}Please select your character traits from the menus below:{/}\n\n" \
	"Use the {light green}movement keys{/} to scroll the menu, " \
	"{light green}Enter{/} to select the current menu item, '{light green}*{/}' " \
	"for a random menu item, '{light green}@{/}' to finish the character with random selections, " \
	"'{light green}ESC{/}' to step back through the birth process, " \
	"'{light green}={/}' for the birth options, '{light green}?{/}' " \
	"for help, or '{light green}Ctrl-X{/}' to quit."

/* Keep tagged colors and the exact real shortcut characters in both
 * languages; only the on-screen explanation changes. */
#define BIRTH_MENU_HELPTEXT_ZH \
    "{light blue}請從下方選單設定角色特性：{/}\n\n" \
    "使用{light green}方向鍵{/}瀏覽，{light green}Enter{/}確認目前選項，" \
    "{light green}*{/}隨機選擇、{light green}@{/}完成剩餘隨機設定。 " \
    "{light green}Esc{/}返回、{light green}={/}角色建立選項、" \
    "{light green}?{/}說明、{light green}Ctrl-X{/}離開。"

/**
 * Show the birth instructions on an otherwise blank screen
 */	
static void print_menu_instructions(void)
{
    int old_wrap = text_out_wrap;
	/* Clear screen */
	Term_clear();
	
	/* Output to the screen */
	text_out_hook = text_out_to_screen;
	
	/* Indent output */
	text_out_indent = QUESTION_COL;
	Term_gotoxy(QUESTION_COL, HEADER_ROW);
	
	/* Keep the multi-line translated help within the actual phone
     * terminal and restore the text renderer state afterward. */
    text_out_wrap = Term && Term->wid > QUESTION_COL + 1 ?
        Term->wid - 1 : 0;
    text_out_e(angdroid_zh_tw_enabled() ?
        BIRTH_MENU_HELPTEXT_ZH : BIRTH_MENU_HELPTEXT);

    text_out_wrap = old_wrap;
	text_out_indent = 0;
}

/**
 * Advance character generation to the confirmation step using random choices
 * and a default point buy for the statistics.
 *
 * \param current is the current stage for character generation.
 */
static void finish_with_random_choices(enum birth_stage current)
{
	struct {
		cmd_code code;
		const char* arg_name;
		int arg_choice;
		char* arg_str;
		bool arg_is_choice;
	} cmds[4];
	int ncmd = 0;
	const struct player_race *pr;
	char name[PLAYER_NAME_LEN];
	char history[240];

	if (current <= BIRTH_RACE_CHOICE) {
		int n, i;

		for (pr = races, n = 0; pr; pr = pr->next, ++n) {}
		i = randint0(n);
		pr = player_id2race(i);

		assert(ncmd < (int)N_ELEMENTS(cmds));
		cmds[ncmd].code = CMD_CHOOSE_RACE;
		cmds[ncmd].arg_name = "choice";
		cmds[ncmd].arg_choice = i;
		cmds[ncmd].arg_is_choice = true;
		++ncmd;
	} else {
		pr = player->race;
	}

	if (current <= BIRTH_CLASS_CHOICE) {
		struct player_class *pc;
		int n, i;

		for (pc = classes, n = 0; pc; pc = pc->next, ++n) {}
		i = randint0(n);

		assert(ncmd < (int)N_ELEMENTS(cmds));
		cmds[ncmd].code = CMD_CHOOSE_CLASS;
		cmds[ncmd].arg_name = "choice";
		cmds[ncmd].arg_choice = i;
		cmds[ncmd].arg_is_choice = true;
		++ncmd;
	}

	if (current <= BIRTH_NAME_CHOICE) {
		/*
		 * Mimic what happens in get_name_command() for the
		 * arg_force_name case.
		 */
		if (arg_force_name) {
			if (arg_name[0]) {
				my_strcpy(player->full_name, arg_name,
					sizeof(player->full_name));
			}
		} else {
			int ntry = 0;

			while (1) {
				if (ntry > 100) {
					quit("Likely bug:  could not generate "
						"a random name that was not "
						"in use for a savefile");
				}
				player_random_name(name, sizeof(name));
				/*
				 * We're good to go if the frontend specified
				 * a savefile to use or the savefile name
				 * corresponding to the random name is not
				 * already in use.
				 */
				if (savefile[0] || !savefile_name_already_used(name, true, true)) {
					break;
				}
				++ntry;
			}
			assert(ncmd < (int)N_ELEMENTS(cmds));
			cmds[ncmd].code = CMD_NAME_CHOICE;
			cmds[ncmd].arg_name = "name";
			cmds[ncmd].arg_str = name;
			cmds[ncmd].arg_is_choice = false;
			++ncmd;
		}
	}

	if (current <= BIRTH_HISTORY_CHOICE) {
		char *buf;

		buf = get_history(pr->history);
		my_strcpy(history, buf, sizeof(history));
		string_free(buf);

		assert(ncmd < (int)N_ELEMENTS(cmds));
		cmds[ncmd].code = CMD_HISTORY_CHOICE;
		cmds[ncmd].arg_name = "history";
		cmds[ncmd].arg_str = history;
		cmds[ncmd].arg_is_choice = false;
		++ncmd;
	}

	/* Push in reverse order:  the last pushed will be executed first. */
	while (ncmd > 0) {
		--ncmd;
		cmdq_push(cmds[ncmd].code);
		if (cmds[ncmd].arg_name) {
			if (cmds[ncmd].arg_is_choice) {
				cmd_set_arg_choice(cmdq_peek(),
					cmds[ncmd].arg_name,
					cmds[ncmd].arg_choice);
			} else {
				cmd_set_arg_string(cmdq_peek(),
					cmds[ncmd].arg_name,
					cmds[ncmd].arg_str);
			}
		}
	}
}

/**
 * Allow the user to select from the current menu, and return the 
 * corresponding command to the game.  Some actions are handled entirely
 * by the UI (displaying help text, for instance).
 */
static enum birth_stage menu_question(enum birth_stage current,
									  struct menu *current_menu,
									  cmd_code choice_command)
{
	struct birthmenu_data *menu_data = menu_priv(current_menu);
	ui_event cx;

	enum birth_stage next = BIRTH_RESET;
	
    /* A Chinese hint may occupy two cells per glyph. Keep it in the
     * terminal and leave the actual menu command keys untouched. */
    clear_question();
    if (Term && Term->wid > QUESTION_COL &&
        QUESTION_ROW >= 0 && QUESTION_ROW < Term->hgt) {
        char visible[384];
        angdroid_menu_copy_columns(visible, sizeof(visible),
            menu_data->hint, (size_t)(Term->wid - QUESTION_COL));
        if (visible[0])
            Term_putstr(QUESTION_COL, QUESTION_ROW, -1,
                COLOUR_YELLOW, visible);
    }

	current_menu->cmd_keys = "?=*@\x18";	 /* ?, =, *, @, <ctl-X> */

	while (next == BIRTH_RESET) {
		/* Display the menu, wait for a selection of some sort to be made. */
		menu_data->stage_inout = current;
		cx = menu_select(current_menu, EVT_KBRD, false);

		/* As all the menus are displayed in "hierarchical" style, we allow
		   use of "back" (left arrow key or equivalent) to step back in 
		   the proces as well as "escape". */
		if (cx.type == EVT_ESCAPE) {
			next = BIRTH_BACK;
		} else if (cx.type == EVT_SELECT) {
			if (current == BIRTH_ROLLER_CHOICE) {
				if (current_menu->cursor) {
					/* Do a first roll of the stats */
					cmdq_push(CMD_ROLL_STATS);
					next = current + 2;
				} else {
					/* 
					 * Make sure we've got a point-based char to play with. 
					 * We call point_based_start here to make sure we get
					 * an update on the points totals before trying to
					 * display the screen.  The call to CMD_RESET_STATS
					 * forces a rebuying of the stats to give us up-to-date
					 * totals.  This is, it should go without saying, a hack.
					 */
					point_based_start();
					cmdq_push(CMD_RESET_STATS);
					cmd_set_arg_choice(cmdq_peek(), "choice", true);
					next = current + 1;
				}
			} else {
				cmdq_push(choice_command);
				cmd_set_arg_choice(cmdq_peek(), "choice", current_menu->cursor);
				next = current + 1;
			}
		} else if (cx.type == EVT_SWITCH) {
			next = menu_data->stage_inout;
		} else if (cx.type == EVT_KBRD) {
			/* '*' chooses an option at random from those the game's provided */
			if (cx.key.code == '*' && menu_data->allow_random) {
				current_menu->cursor = randint0(current_menu->count);
				cmdq_push(choice_command);
				cmd_set_arg_choice(cmdq_peek(), "choice", current_menu->cursor);

				menu_refresh(current_menu, false);
				next = current + 1;
			} else if (cx.key.code == '=') {
				do_cmd_options_birth();
				next = current;
			} else if (cx.key.code == '@') {
				/*
				 * Use random choices to complete the character.
				 */
				finish_with_random_choices(current);
				next = BIRTH_FINAL_CONFIRM;
			} else if (cx.key.code == KTRL('X')) {
				quit(NULL);
			} else if (cx.key.code == '?') {
				do_cmd_help();
			}
		}
	}
	
	return next;
}

/**
 * ------------------------------------------------------------------------
 * The rolling bit of the roller.
 * ------------------------------------------------------------------------ */
static enum birth_stage roller_command(bool first_call)
{
	enum {
		ACT_CTX_BIRTH_ROLL_NONE,
		ACT_CTX_BIRTH_ROLL_ESCAPE,
		ACT_CTX_BIRTH_ROLL_REROLL,
		ACT_CTX_BIRTH_ROLL_PREV,
		ACT_CTX_BIRTH_ROLL_ACCEPT,
		ACT_CTX_BIRTH_ROLL_QUIT,
		ACT_CTX_BIRTH_ROLL_HELP
	};
	char prompt[256] = "";
	size_t promptlen = 0;
	int action = ACT_CTX_BIRTH_ROLL_NONE;
	ui_event in;

	enum birth_stage next = BIRTH_ROLLER;

	/* Used to keep track of whether we've rolled a character before or not. */
	static bool prev_roll = false;

	/* Display the player - a bit cheaty, but never mind. */
	display_player(0);

	if (first_call)
		prev_roll = false;

	/* Prepare a prompt (must squeeze everything in) */
	strnfcat(prompt, sizeof(prompt), &promptlen, "%s",
        angdroid_game_ui_text("['r' to reroll",
            angdroid_zh_tw_enabled()));
	if (prev_roll) {
		soft_kbd_flash("rp?\x18");
		strnfcat(prompt, sizeof(prompt), &promptlen, "%s",
            angdroid_game_ui_text(", 'p' for previous roll",
                angdroid_zh_tw_enabled()));
	}
	else {
		soft_kbd_flash("r?\x18");
	}
	strnfcat(prompt, sizeof(prompt), &promptlen, "%s",
        angdroid_game_ui_text(" or 'Enter' to accept]",
            angdroid_zh_tw_enabled()));

	/* Prompt for it */
	fa_birth_prompt_centered(prompt, Term->hgt - 1);
	
	/*
	 * Get the response.  Emulate what inkey_does() without coercing mouse
	 * events to look like keystrokes.
	 */
	while (1) {
		in = inkey_ex();
		if (in.type == EVT_KBRD || in.type == EVT_MOUSE) {
			break;
		}
		if (in.type == EVT_BUTTON) {
			in.type = EVT_KBRD;
			break;
		}
		if (in.type == EVT_ESCAPE) {
			in.type = EVT_KBRD;
			in.key.code = ESCAPE;
			in.key.mods = 0;
			break;
		}
	}

	/* Analyse the command */
	if (in.type == EVT_KBRD) {
		if (in.key.code == ESCAPE) {
			action = ACT_CTX_BIRTH_ROLL_ESCAPE;
		} else if (in.key.code == KC_ENTER) {
			action = ACT_CTX_BIRTH_ROLL_ACCEPT;
		} else if (in.key.code == ' ' || in.key.code == 'r') {
			action = ACT_CTX_BIRTH_ROLL_REROLL;
		} else if (prev_roll && in.key.code == 'p') {
			action = ACT_CTX_BIRTH_ROLL_PREV;
		} else if (in.key.code == KTRL('X')) {
			action = ACT_CTX_BIRTH_ROLL_QUIT;
		} else if (in.key.code == '?') {
			action = ACT_CTX_BIRTH_ROLL_HELP;
		} else {
			/* Nothing handled directly here */
			bell();
		}
	} else if (in.type == EVT_MOUSE) {
		if (in.mouse.button == 2) {
			action = ACT_CTX_BIRTH_ROLL_ESCAPE;
		} else {
			/* Present a context menu with the other actions. */
			char *labels = string_make(lower_case);
			struct menu *m = menu_dynamic_new();

			m->selections = labels;
			menu_dynamic_add_label(m, angdroid_game_ui_text("Reroll",
                angdroid_zh_tw_enabled()), 'r',
				ACT_CTX_BIRTH_ROLL_REROLL, labels);
			if (prev_roll) {
				menu_dynamic_add_label(m, angdroid_game_ui_text("Retrieve previous",
                angdroid_zh_tw_enabled()),
					'p', ACT_CTX_BIRTH_ROLL_PREV, labels);
			}
			menu_dynamic_add_label(m, angdroid_game_ui_text("Accept",
                angdroid_zh_tw_enabled()), 'a',
				ACT_CTX_BIRTH_ROLL_ACCEPT, labels);
			menu_dynamic_add_label(m, angdroid_game_ui_text("Quit",
            angdroid_zh_tw_enabled()), 'q',
				ACT_CTX_BIRTH_ROLL_QUIT, labels);
			menu_dynamic_add_label(m, angdroid_game_ui_text("Help",
            angdroid_zh_tw_enabled()), '?',
				ACT_CTX_BIRTH_ROLL_HELP, labels);

			screen_save();

			menu_dynamic_calc_location(m, in.mouse.x, in.mouse.y);
			region_erase_bordered(&m->boundary);

			action = menu_dynamic_select(m);

			menu_dynamic_free(m);
			string_free(labels);

			screen_load();
		}
	}

	switch (action) {
	case ACT_CTX_BIRTH_ROLL_ESCAPE:
		/* Back out to the previous birth stage. */
		next = BIRTH_BACK;
		break;

	case ACT_CTX_BIRTH_ROLL_REROLL:
		/* Reroll this character. */
		cmdq_push(CMD_ROLL_STATS);
		prev_roll = true;
		break;

	case ACT_CTX_BIRTH_ROLL_PREV:
		/* Swap with previous roll. */
		cmdq_push(CMD_PREV_STATS);
		break;

	case ACT_CTX_BIRTH_ROLL_ACCEPT:
		/* Accept the roll.  Go to the next stage. */
		next = BIRTH_NAME_CHOICE;
		break;

	case ACT_CTX_BIRTH_ROLL_QUIT:
		quit(NULL);
		break;

	case ACT_CTX_BIRTH_ROLL_HELP:
		do_cmd_help();
		break;
	}

	return next;
}

/**
 * ------------------------------------------------------------------------
 * Point-based stat allocation.
 * ------------------------------------------------------------------------ */

/* The locations of the "costs" area on the birth screen. */
#define COSTS_ROW 2

/* FA's original x=74 cost column disappears on portrait terminals.
 * Keep the exact original 80-column positions, but fit the same
 * stat values/cursor in the available width on smaller phones. */
static int fa_birth_costs_col(void)
{
    int wid = Term ? Term->wid : 80;
    if (wid >= 80) return 42 + 32;
    return MAX(0, wid - 9);
}

static int fa_birth_total_col(void)
{
    int wid = Term ? Term->wid : 80;
    if (wid >= 80) return 42 + 19;
    return MAX(0, wid - 17);
}

#define COSTS_COL fa_birth_costs_col()
#define TOTAL_COL fa_birth_total_col()

static void fa_birth_put_clipped(const char *message, int row, int col)
{
    char visible[192];
    if (!Term || !message || row < 0 || row >= Term->hgt ||
        col < 0 || col >= Term->wid) return;
    angdroid_menu_copy_columns(visible, sizeof(visible), message,
        (size_t)(Term->wid - col));
    if (visible[0]) put_str(visible, row, col);
}

/*
 * Remember what's possible for a given stat.  0 means can't buy or sell.
 * 1 means can sell.  2 means can buy.  3 means can buy or sell.
 */
static int buysell[STAT_MAX];

/**
 * This is called whenever a stat changes.  We take the easy road, and just
 * redisplay them all using the standard function.
 */
static void point_based_stats(game_event_type type, game_event_data *data,
							  void *user)
{
	display_player_stat_info();
}

/**
 * This is called whenever any of the other miscellaneous stat-dependent things
 * changed.  We are hooked into changes in the amount of gold in this case,
 * but redisplay everything because it's easier.
 */
static void point_based_misc(game_event_type type, game_event_data *data,
							 void *user)
{
	display_player_xtra_info(0);
}


/**
 * This is called whenever the points totals are changed (in birth.c), so
 * that we can update our display of how many points have been spent and
 * are available.
 */
static void point_based_points(game_event_type type, game_event_data *data,
							   void *user)
{
	int i;
	int sum = 0;
	const int *spent = data->birthpoints.points;
	const int *inc = data->birthpoints.inc_points;
	int remaining = data->birthpoints.remaining;

	/* Display the costs header */
	fa_birth_put_clipped(angdroid_game_ui_text("Cost",
        angdroid_zh_tw_enabled()), COSTS_ROW - 1, COSTS_COL);
	
	for (i = 0; i < STAT_MAX; i++) {
		/* Remember what's allowed. */
		buysell[i] = 0;
		if (spent[i] > 0) {
			buysell[i] |= 1;
		}
		if (inc[i] <= remaining) {
			buysell[i] |= 2;
		}
		/* Display cost */
		fa_birth_put_clipped(format("%4d", spent[i]),
            COSTS_ROW + i, COSTS_COL);
		sum += spent[i];
	}
	
	fa_birth_put_clipped(format(angdroid_game_ui_text(
        "Total Cost: %2d/%2d", angdroid_zh_tw_enabled()),
        sum, remaining + sum), COSTS_ROW + STAT_MAX, TOTAL_COL);
}

static void point_based_start(void)
{
	const char *prompt = angdroid_game_ui_text(
        "[up/down to move, left/right to modify, 'r' to reset, 'Enter' to accept]",
        angdroid_zh_tw_enabled());
	int i;

	/* Clear */
	Term_clear();

	/* Display the player */
	display_player_xtra_info(0);
	display_player_stat_info();

	fa_birth_prompt_centered(prompt, Term->hgt - 1);

	for (i = 0; i < STAT_MAX; ++i) {
		buysell[i] = 0;
	}

	/* Register handlers for various events - cheat a bit because we redraw
	   the lot at once rather than each bit at a time. */
	event_add_handler(EVENT_BIRTHPOINTS, point_based_points, NULL);	
	event_add_handler(EVENT_STATS, point_based_stats, NULL);	
	event_add_handler(EVENT_GOLD, point_based_misc, NULL);	
}

static void point_based_stop(void)
{
	event_remove_handler(EVENT_BIRTHPOINTS, point_based_points, NULL);	
	event_remove_handler(EVENT_STATS, point_based_stats, NULL);	
	event_remove_handler(EVENT_GOLD, point_based_misc, NULL);	
}

static enum birth_stage point_based_command(void)
{
	static int stat = 0;
	enum {
		ACT_CTX_BIRTH_PTS_NONE,
		ACT_CTX_BIRTH_PTS_BUY,
		ACT_CTX_BIRTH_PTS_SELL,
		ACT_CTX_BIRTH_PTS_ESCAPE,
		ACT_CTX_BIRTH_PTS_RESET,
		ACT_CTX_BIRTH_PTS_ACCEPT,
		ACT_CTX_BIRTH_PTS_QUIT
	};
	int action = ACT_CTX_BIRTH_PTS_NONE;
	ui_event in;
	enum birth_stage next = BIRTH_POINTBASED;

	/* Place cursor just after cost of current stat */
    if (Term && Term->wid > 0 && Term->hgt > COSTS_ROW + stat)
        Term_gotoxy(MIN(Term->wid - 1, COSTS_COL + 4),
            COSTS_ROW + stat);

	soft_kbd_flash("r\x18");

	/*
	 * Get input.  Emulate what inkey() does without coercing mouse events
	 * to look like keystrokes.
	 */
	while (1) {
		in = inkey_ex();
		if (in.type == EVT_KBRD || in.type == EVT_MOUSE) {
			break;
		}
		if (in.type == EVT_BUTTON) {
			in.type = EVT_KBRD;
		}
		if (in.type == EVT_ESCAPE) {
			in.type = EVT_KBRD;
			in.key.code = ESCAPE;
			in.key.mods = 0;
			break;
		}
	}

	/* Figure out what to do. */
	if (in.type == EVT_KBRD) {
		if (in.key.code == KTRL('X')) {
			action = ACT_CTX_BIRTH_PTS_QUIT;
		} else if (in.key.code == ESCAPE) {
			action = ACT_CTX_BIRTH_PTS_ESCAPE;
		} else if (in.key.code == 'r' || in.key.code == 'R') {
			action = ACT_CTX_BIRTH_PTS_RESET;
		} else if (in.key.code == KC_ENTER) {
			action = ACT_CTX_BIRTH_PTS_ACCEPT;
		} else {
			int dir;

			if (in.key.code == '-') {
				dir = 4;
			} else if (in.key.code == '+') {
				dir = 6;
			} else {
				dir = target_dir(in.key);
			}

			/*
			 * Go to previous stat.  Loop back to the last if at
			 * the first.
			 */
			if (dir == 8) {
				stat = (stat + STAT_MAX - 1) % STAT_MAX;
			}

			/*
			 * Go to next stat.  Loop back to the first if at the
			 * last.
			 */
			if (dir == 2) {
				stat = (stat + 1) % STAT_MAX;
			}

			/* Decrease stat (if possible). */
			if (dir == 4) {
				action = ACT_CTX_BIRTH_PTS_SELL;
			}

			/* Increase stat (if possible). */
			if (dir == 6) {
				action = ACT_CTX_BIRTH_PTS_BUY;
			}
		}
	} else if (in.type == EVT_MOUSE) {
		assert(stat >= 0 && stat < STAT_MAX);
		if (in.mouse.button == 2) {
			action = ACT_CTX_BIRTH_PTS_ESCAPE;
		} else if (in.mouse.y >= COSTS_ROW
				&& in.mouse.y < COSTS_ROW + STAT_MAX
				&& in.mouse.y != COSTS_ROW + stat) {
			/*
			 * Make that stat the current one if buying or selling.
			 */
			stat = in.mouse.y - COSTS_ROW;
		} else {
			/* Present a context menu with the other actions. */
			char *labels = string_make(lower_case);
			struct menu *m = menu_dynamic_new();

			m->selections = labels;
			if (in.mouse.y == COSTS_ROW + stat
					&& (buysell[stat] & 1)) {
				menu_dynamic_add_label(m, angdroid_game_ui_text("Sell",
                angdroid_zh_tw_enabled()), 's',
					ACT_CTX_BIRTH_PTS_SELL, labels);
			}
			if (in.mouse.y == COSTS_ROW + stat
					&& (buysell[stat] & 2)) {
				menu_dynamic_add_label(m, angdroid_game_ui_text("Buy",
                angdroid_zh_tw_enabled()), 'b',
					ACT_CTX_BIRTH_PTS_BUY, labels);
			}
			menu_dynamic_add_label(m, angdroid_game_ui_text("Accept",
                angdroid_zh_tw_enabled()), 'a',
				ACT_CTX_BIRTH_PTS_ACCEPT, labels);
			menu_dynamic_add_label(m, angdroid_game_ui_text("Reset",
                angdroid_zh_tw_enabled()), 'r',
				ACT_CTX_BIRTH_PTS_RESET, labels);
			menu_dynamic_add_label(m, angdroid_game_ui_text("Quit",
            angdroid_zh_tw_enabled()), 'q',
				ACT_CTX_BIRTH_PTS_QUIT, labels);

			screen_save();

			menu_dynamic_calc_location(m, in.mouse.x, in.mouse.y);
			region_erase_bordered(&m->boundary);

			action = menu_dynamic_select(m);

			menu_dynamic_free(m);
			string_free(labels);

			screen_load();
		}
	}

	/* Do it. */
	switch (action) {
	case ACT_CTX_BIRTH_PTS_SELL:
		assert(stat >= 0 && stat < STAT_MAX);
		cmdq_push(CMD_SELL_STAT);
		cmd_set_arg_choice(cmdq_peek(), "choice", stat);
		break;

	case ACT_CTX_BIRTH_PTS_BUY:
		assert(stat >= 0 && stat < STAT_MAX);
		cmdq_push(CMD_BUY_STAT);
		cmd_set_arg_choice(cmdq_peek(), "choice", stat);
		break;

	case ACT_CTX_BIRTH_PTS_ESCAPE:
		/* Go back a step or back to the start of this step. */
		next = BIRTH_BACK;
		break;

	case ACT_CTX_BIRTH_PTS_RESET:
		cmdq_push(CMD_RESET_STATS);
		cmd_set_arg_choice(cmdq_peek(), "choice", false);
		break;

	case ACT_CTX_BIRTH_PTS_ACCEPT:
		/* Done with this stage.  Proceed to the next. */
		next = BIRTH_NAME_CHOICE;
		break;

	case ACT_CTX_BIRTH_PTS_QUIT:
		quit(NULL);
		break;

	default:
		/* Do nothing and remain at this stage. */
		break;
	}

	return next;
}
	
/**
 * ------------------------------------------------------------------------
 * Asking for the player's chosen name.
 * ------------------------------------------------------------------------ */
//phantom changes for server
static enum birth_stage get_name_command(void)
{
	enum birth_stage next;
	char name[PLAYER_NAME_LEN];

	/* Use frontend-provided savefile name if requested */
	if (arg_name[0]) {
		my_strcpy(player->full_name, arg_name, sizeof(player->full_name));
	}

	/*
	 * If not forcing the character's name, the front end didn't set the
	 * savefile to use, and the chosen name for the character would lead
	 * to overwriting an existing savefile, confirm that's okay with the
	 * player.
	 */
	if (arg_force_name) {
		next = BIRTH_HISTORY_CHOICE;
	} else if (get_character_name(name, sizeof(name))
			&& (savefile[0]
			|| !savefile_name_already_used(name, true, true)
			|| get_check(angdroid_game_ui_text(
            "A savefile for that name exists.  Overwrite it? ",
            angdroid_zh_tw_enabled())))) {
		cmdq_push(CMD_NAME_CHOICE);
		cmd_set_arg_string(cmdq_peek(), "name", name);
		next = BIRTH_HISTORY_CHOICE;
	} else {
		next = BIRTH_BACK;
	}

	
	return next;
}

static void get_screen_loc(size_t cursor, int *x, int *y, size_t n_lines,
    const size_t *line_starts, const size_t *line_lengths)
{
    size_t i, line = 0;
    if (!x || !y) return;
    *x = *y = 0;
    if (!n_lines || !line_starts || !line_lengths) return;

    /* At a soft wrap the next line shares the previous line's ending
     * scalar offset. Prefer the next line so the caret never gets stuck
     * against the right edge when the user types a new character. */
    for (i = 1; i < n_lines; ++i) {
        if (cursor >= line_starts[i]) line = i;
        else break;
    }
    *y = (int)line;
    *x = (int)MIN(cursor - line_starts[line], line_lengths[line]);
}

/* The history editor is shown inside the character sheet. Its old
 * 71-column rectangle overflowed portrait phones, while the cursor was
 * positioned in Unicode scalars rather than terminal display cells. */
static region fa_birth_history_area(void)
{
    region area = { 1, HIST_INSTRUCT_ROW + 1, 71, 5 };
    if (Term && Term->wid > 0 && Term->hgt > 0) {
        area.col = Term->wid > 2 ? 1 : 0;
        area.row = MIN(HIST_INSTRUCT_ROW + 1, MAX(0, Term->hgt - 4));
        area.width = MIN(71, MAX(1, Term->wid - area.col - 1));
        area.page_rows = MIN(5, MAX(1, Term->hgt - area.row));
    }
    return area;
}

static int edit_text(char *buffer, int buflen) {
	int len = strlen(buffer);
	bool done = false;
	int cursor = 0;

	while (!done) {
		int x = 0, y = 0;
		struct keypress ke;

        region area = fa_birth_history_area();
        size_t line_starts[256], line_lengths[256];
        size_t n_lines, first_visible, draw;
        int ulen;

        /* Wrap by terminal *cells*, not wide-character scalar counts.
         * A 40-column portrait terminal must not display 40 Han glyphs
         * on one textblock row. Use stack arrays: no ESC allocation leak. */
        n_lines = angdroid_fa_history_wrap(buffer, (size_t)area.width,
            line_starts, line_lengths, N_ELEMENTS(line_starts));
        ulen = n_lines ? (int)(line_starts[n_lines - 1] +
            line_lengths[n_lines - 1]) : 0;
        if (cursor > ulen) cursor = ulen;

        get_screen_loc((size_t)cursor, &x, &y, n_lines,
            line_starts, line_lengths);
        first_visible = y >= area.page_rows ?
            (size_t)(y - area.page_rows + 1) : 0;

        clear_from(MAX(0, area.row - 1));
        /* When a background story spans more rows than the phone can
         * show, tell the player which wrapped line they're editing.
         * The cursor and text remain editable above/below the viewport. */
        if (Term && area.row > 0) {
            const char *status = angdroid_zh_tw_enabled() ?
                "背景第 %d/%d 行 · ↑↓ 移動 · Enter 完成／Esc 取消" :
                "History line %d/%d - arrows move - Enter/Esc";
            fa_birth_prompt_left(format(status, y + 1, (int)n_lines),
                area.row - 1);
        }
        for (draw = 0; draw < (size_t)area.page_rows &&
                first_visible + draw < n_lines; ++draw) {
            char original[256], visible[256];
            size_t at = first_visible + draw;
            angdroid_fa_history_copy_line(original, sizeof(original),
                buffer, line_starts[at], line_lengths[at]);
            angdroid_menu_copy_columns(visible, sizeof(visible),
                original, (size_t)area.width);
            if (visible[0] && Term)
                c_put_str(COLOUR_WHITE, visible,
                    area.row + (int)draw, area.col);
        }
        if (n_lines && (size_t)y < n_lines) {
            size_t before = angdroid_input_cursor_columns(buffer,
                line_starts[y]);
            size_t now = angdroid_input_cursor_columns(buffer,
                (size_t)cursor);
            size_t columns = now >= before ? now - before : 0;
            x = (int)MIN(columns, (size_t)MAX(0, area.width - 1));
        }
        if (Term && Term->wid > 0 && Term->hgt > 0) {
            int at_x = MIN(Term->wid - 1, area.col + x);
            int at_y = MIN(Term->hgt - 1,
                area.row + y - (int)first_visible);
            Term_gotoxy(at_x, at_y);
        }

		ke = inkey();
		switch (ke.code) {
			case ESCAPE:
                /* The source string is unmodified on cancellation; all
                 * wrapping state lives on the stack and needs no cleanup. */
                return -1;

			case KC_ENTER:
				done = true;
				break;

			case ARROW_LEFT:
				if (cursor > 0) cursor--;
				break;

			case ARROW_RIGHT:
				if (cursor < ulen) cursor++;
				break;

			case ARROW_DOWN:
                if (n_lines && y >= 0 && (size_t)(y + 1) < n_lines) {
                    size_t across = (size_t)cursor - line_starts[y];
                    cursor = (int)(line_starts[y + 1] +
                        MIN(across, line_lengths[y + 1]));
                }
                break;

			case ARROW_UP:
                if (n_lines && y > 0 && (size_t)y < n_lines) {
                    size_t across = (size_t)cursor - line_starts[y];
                    cursor = (int)(line_starts[y - 1] +
                        MIN(across, line_lengths[y - 1]));
                }
                break;

			case KC_END:
				cursor = MAX(0, ulen);
				break;

			case KC_HOME:
				cursor = 0;
				break;

			case KC_BACKSPACE:
			case KC_DELETE: {
				char *ocurs, *oshift;

				/* Refuse to backspace into oblivion */
				if ((ke.code == KC_BACKSPACE && cursor == 0) ||
						(ke.code == KC_DELETE && cursor >= ulen))
					break;

				/*
				 * Move the string from k to nul along to the
				 * left by 1.  First, have to get offset
				 * corresponding to the cursor position.
				 */
				ocurs = utf8_fskip(buffer, cursor, NULL);
				assert(ocurs);
				if (ke.code == KC_BACKSPACE) {
					/* Get offset of the previous character. */
					oshift = utf8_rskip(ocurs, 1, buffer);
					assert(oshift);
					memmove(oshift, ocurs,
						len - (ocurs - buffer));
					/* Decrement */
					--cursor;
					len -= ocurs - oshift;
				} else {
					/* Get offset of the next character. */
					oshift = utf8_fskip(ocurs, 1, NULL);
					assert(oshift);
					memmove(ocurs, oshift,
						len - (oshift - buffer));
					/* Decrement. */
					len -= oshift - ocurs;
				}

				/* Terminate */
				buffer[len] = '\0';

				break;
			}
			
			default: {
				bool atnull = (cursor == ulen);
				char encoded[5];
				int n_enc;
				char *ocurs;

				if (!keycode_isprint(ke.code))
					break;

				n_enc = utf32_to_utf8(encoded,
					N_ELEMENTS(encoded), &ke.code, 1, NULL);

				/*
				 * Make sure we have something to add and have
				 * enough space.
				 */
				if (n_enc == 0 || n_enc + len >= buflen) {
					break;
				}

				/* Insert the encoded character. */
				if (atnull) {
					ocurs = buffer + len;
				} else {
					ocurs = utf8_fskip(buffer, cursor, NULL);
					assert(ocurs);
					/*
					 * Move the rest of the buffer along
					 * to make room.
					 */
					memmove(ocurs + n_enc, ocurs,
						len - (ocurs - buffer));
				}
				memcpy(ocurs, encoded, n_enc);

				/* Update cursor position and length. */
				++cursor;
				len += n_enc;

				/* Terminate */
				buffer[len] = '\0';

				break;
			}
		}

	}

	return 0;
}

/**
 * ------------------------------------------------------------------------
 * Allowing the player to choose their history.
 * ------------------------------------------------------------------------ */
static enum birth_stage get_history_command(void)
{
	enum birth_stage next = 0;
	struct keypress ke;
	/* Ask for some history */
	fa_birth_prompt_left(angdroid_game_ui_text(
        "Accept character history? [y/n]",
        angdroid_zh_tw_enabled()), 0);

	soft_kbd_flash("yn\x18");

	ke = inkey();

	/* Quit, go back, change history, or accept */
	if (ke.code == KTRL('X')) {
		quit(NULL);
	} else if (ke.code == ESCAPE) {
		next = BIRTH_BACK;
	} else if (ke.code == 'N' || ke.code == 'n') {
		char history[240];
        /* Never save an arbitrarily truncated UTF-8 prefix of a
         * user-owned history when it does not fit the editor buffer. */
        if (!angdroid_input_prefill_copy(history, sizeof(history),
                player->history)) {
            msg("%s", angdroid_zh_tw_enabled() ?
                "角色背景故事過長或含有無效 UTF-8，無法安全編輯。" :
                "Character history is too long or contains invalid UTF-8.");
            return BIRTH_HISTORY_CHOICE;
        }

		switch (edit_text(history, sizeof(history))) {
			case -1:
				next = BIRTH_BACK;
				break;
			case 0:
				cmdq_push(CMD_HISTORY_CHOICE);
				cmd_set_arg_string(cmdq_peek(), "history", history);
				next = BIRTH_HISTORY_CHOICE;
		}
	} else {
		next = BIRTH_FINAL_CONFIRM;
	}

	return next;
}

/**
 * ------------------------------------------------------------------------
 * Final confirmation of character.
 * ------------------------------------------------------------------------ */
static enum birth_stage get_confirm_command(void)
{
	const char *prompt = angdroid_game_ui_text(
        "['ESC' to step back, 'S' to start over, or any other key to continue]",
        angdroid_zh_tw_enabled());
	struct keypress ke;

	enum birth_stage next = BIRTH_RESET;

	/* Prompt for it */
	fa_birth_prompt_centered(prompt, Term->hgt - 1);

	soft_kbd_flash("S\x18");

	/* Get a key */
	ke = inkey();
	
	/* Start over */
	if (ke.code == 'S' || ke.code == 's') {
		next = BIRTH_RESET;
	} else if (ke.code == KTRL('X')) {
		quit(NULL);
	} else if (ke.code == ESCAPE) {
		next = BIRTH_BACK;
	} else {
		cmdq_push(CMD_ACCEPT_CHARACTER);
		next = BIRTH_COMPLETE;
	}

	/* Clear prompt */
	clear_from(23);

	return next;
}



/**
 * ------------------------------------------------------------------------
 * Things that relate to the world outside this file: receiving game events
 * and being asked for game commands.
 * ------------------------------------------------------------------------ */

/**
 * This is called when we receive a request for a command in the birth 
 * process.

 * The birth process continues until we send a final character confirmation
 * command (or quit), so this is effectively called in a loop by the main
 * game.
 *
 * We're imposing a step-based system onto the main game here, so we need
 * to keep track of where we're up to, where each step moves on to, etc.
 */
int textui_do_birth(void)
{
	enum birth_stage current_stage = BIRTH_RESET;
	enum birth_stage prev = BIRTH_BACK;
	enum birth_stage roller = BIRTH_RESET;
	enum birth_stage next = current_stage;

	bool done = false;

	cmdq_push(CMD_BIRTH_INIT);
	cmdq_execute(CTX_BIRTH);

	while (!done) {

		switch (current_stage)
		{
			case BIRTH_RESET:
			{
				cmdq_push(CMD_BIRTH_RESET);

				roller = BIRTH_RESET;
				
				if (quickstart_allowed)
					next = BIRTH_QUICKSTART;
				else
					next = BIRTH_MAP_CHOICE;

				break;
			}

			case BIRTH_QUICKSTART:
			{
				display_player(0);
				next = textui_birth_quickstart();
				if (next == BIRTH_COMPLETE)
					done = true;
				break;
			}

			case BIRTH_MAP_CHOICE:
			{
				print_map_instructions();
				next = get_map_command();
				if (next == BIRTH_BACK)
					next = BIRTH_RESET;
				break;
			}

			case BIRTH_CLASS_CHOICE:
			case BIRTH_RACE_CHOICE:
			case BIRTH_ROLLER_CHOICE:
			{
				struct menu *menu = &race_menu;
				cmd_code command = CMD_CHOOSE_RACE;

                /* A phone may rotate after the first creation step.
                 * Rebuild terminal regions without touching cursor, race
                 * IDs, choices, or the CMD_CHOOSE_* queue. */
                fa_birth_configure_layout();
                menu_layout(&race_menu, &race_region);
                menu_layout(&class_menu, &class_region);
                menu_layout(&roller_menu, &roller_region);

				Term_clear();
				print_menu_instructions();

				if (current_stage > BIRTH_RACE_CHOICE) {
                    /* Compact mode deliberately shows only the current
                     * choice list, not three overlapping columns. */
                    if (!fa_birth_compact_layout())
                        menu_refresh(&race_menu, false);
					menu = &class_menu;
					command = CMD_CHOOSE_CLASS;
				}

				if (current_stage > BIRTH_CLASS_CHOICE) {
                    if (!fa_birth_compact_layout())
                        menu_refresh(&class_menu, false);
					menu = &roller_menu;
				}

				next = menu_question(current_stage, menu, command);

				if (next == BIRTH_BACK)
					next = current_stage - 1;

				/* Make sure the character gets reset before quickstarting */
				if (next == BIRTH_QUICKSTART) 
					next = BIRTH_RESET;

				break;
			}

			case BIRTH_POINTBASED:
			{
				roller = BIRTH_POINTBASED;
		
				if (prev > BIRTH_POINTBASED) {
					point_based_start();
					/*
					 * Force a redraw of the point
					 * allocations but do not reset them.
					 */
					cmdq_push(CMD_REFRESH_STATS);
					cmdq_execute(CTX_BIRTH);
				}

				next = point_based_command();

				if (next == BIRTH_BACK)
					next = BIRTH_ROLLER_CHOICE;

				if (next != BIRTH_POINTBASED)
					point_based_stop();

				break;
			}

			case BIRTH_ROLLER:
			{
				roller = BIRTH_ROLLER;
				next = roller_command(prev < BIRTH_ROLLER);
				if (next == BIRTH_BACK)
					next = BIRTH_ROLLER_CHOICE;

				break;
			}

			case BIRTH_NAME_CHOICE:
			{
				if (prev < BIRTH_NAME_CHOICE)
					display_player(0);

				next = get_name_command();
				if (next == BIRTH_BACK)
					next = roller;

				break;
			}

			case BIRTH_HISTORY_CHOICE:
			{
				if (prev < BIRTH_HISTORY_CHOICE)
					display_player(0);

				next = get_history_command();
				if (next == BIRTH_BACK)
					next = BIRTH_NAME_CHOICE;

				break;
			}

			case BIRTH_FINAL_CONFIRM:
			{
				if (prev < BIRTH_FINAL_CONFIRM)
					display_player(0);

				next = get_confirm_command();
				if (next == BIRTH_BACK)
					next = BIRTH_HISTORY_CHOICE;

				if (next == BIRTH_COMPLETE)
					done = true;

				break;
			}

			default:
			{
				/* Remove dodgy compiler warning, */
			}
		}

		prev = current_stage;
		current_stage = next;

		/* Execute whatever commands have been sent */
		cmdq_execute(CTX_BIRTH);
	}

	return 0;
}

/**
 * Called when we enter the birth mode - so we set up handlers, command hooks,
 * etc, here.
 */
static void ui_enter_birthscreen(game_event_type type, game_event_data *data,
								 void *user)
{
	/* Set the ugly static global that tells us if quickstart's available. */
	quickstart_allowed = data->flag;

	setup_menus();
}

static void ui_leave_birthscreen(game_event_type type, game_event_data *data,
								 void *user)
{
	/* Set the savefile name if it's not already set */
	if (!savefile[0])
		savefile_set_name(player->full_name, true, true);

	free_birth_menus();
}


void ui_init_birthstate_handlers(void)
{
	event_add_handler(EVENT_ENTER_BIRTH, ui_enter_birthscreen, NULL);
	event_add_handler(EVENT_LEAVE_BIRTH, ui_leave_birthscreen, NULL);
}

