/**
 * \file ui-store.c
 * \brief Store UI
 *
 * Copyright (c) 1997 Robert A. Koeneke, James E. Wilson, Ben Harrison
 * Copyright (c) 1998-2014 Angband developers
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
#include "game-event.h"
#include "game-input.h"
#include "hint.h"
#include "init.h"
#include "monster.h"
#include "obj-desc.h"
#include "obj-gear.h"
#include "obj-ignore.h"
#include "obj-info.h"
#include "obj-knowledge.h"
#include "obj-make.h"
#include "obj-pile.h"
#include "obj-tval.h"
#include "obj-util.h"
#include "player-calcs.h"
#include "player-history.h"
#include "player-util.h"
#include "store.h"
#include "target.h"
#include "ui-display.h"
#include "ui-input.h"
#include "ui-menu.h"
#include "ui-object.h"
#include "ui-options.h"
#include "ui-knowledge.h"
#include "ui-object.h"
#include "ui-player.h"
#include "ui-spell.h"
#include "ui-command.h"
#include "ui-store.h"
#include "droid.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/zh-tw-store-names.h"
#include "../angband/zh-tw-store-text.h"
#include "../angband/store-column-layout.h"
#include "../angband/store-detail-text.h"
#include "../angband/menu-text-fit.h"
#include "z-debug.h"


/**
 * Shopkeeper welcome messages.
 *
 * The shopkeeper's name must come first, then the character's name.
 */
static const char *comment_welcome[] =
{
	"",
	"%s nods to you.",
	"%s says hello.",
	"%s: \"See anything you like, adventurer?\"",
	"%s: \"How may I help you, %s?\"",
	"%s: \"Welcome back, %s.\"",
	"%s: \"A pleasure to see you again, %s.\"",
	"%s: \"How may I be of assistance, good %s?\"",
	"%s: \"You do honour to my humble store, noble %s.\"",
	"%s: \"I and my family are entirely at your service, %s.\""
};

static const char *comment_hint[] =
{
/*	"%s tells you soberly: \"%s\".",
	"(%s) There's a saying round here, \"%s\".",
	"%s offers to tell you a secret next time you're about."*/
	"\"%s\""
};


/**
 * Easy names for the elements of the 'scr_places' arrays.
 */
enum
{
	LOC_PRICE = 0,
	LOC_OWNER,
	LOC_HEADER,
	LOC_MORE,
	LOC_HELP_CLEAR,
	LOC_HELP_PROMPT,
	LOC_AU,
	LOC_WEIGHT,

	LOC_MAX
};

/* Store rows may occupy a smaller region than the terminal.
 * Render by terminal display columns and preserve full UTF-8 glyphs. */
static void fa_store_put_clipped(int attr, const char *text,
    int row, int col, int columns)
{
    char clipped[512];
    if (!Term || !text || row < 0 || row >= Term->hgt ||
        col < 0 || col >= Term->wid || columns <= 0)
        return;
    columns = MIN(columns, Term->wid - col);
    angdroid_menu_copy_columns(clipped, sizeof(clipped),
        text, (size_t)columns);
    if (clipped[0])
        c_put_str(attr, clipped, row, col);
}

/* State flags */
#define STORE_GOLD_CHANGE      0x01
#define STORE_FRAME_CHANGE     0x02
#define STORE_SHOW_HELP        0x04

/* Compound flag for the initial display of a store */
#define STORE_INIT_CHANGE		(STORE_FRAME_CHANGE | STORE_GOLD_CHANGE)

struct store_context {
	struct menu menu;			/* Menu instance */
	struct store *store;	/* Pointer to store */
	struct object **list;	/* List of objects (unused) */
	int flags;				/* Display flags */
	bool inspect_only;		/* Only allow looking */

	/* Places for the various things displayed onscreen */
	unsigned int scr_places_x[LOC_MAX];
	unsigned int scr_places_y[LOC_MAX];
};

/* Return a random hint from the global hints list */
static const char *random_hint(void)
{
	struct hint *v, *r = NULL;
	int n;
	for (v = hints, n = 1; v; v = v->next, n++)
		if (one_in_(n))
			r = v;
	return r->hint;
}

/**
 * The greeting a shopkeeper gives the character says a lot about his
 * general attitude.
 *
 * Taken and modified from Sangband 1.0.
 *
 * Note that each comment_hint should have exactly one %s
 */
static void prt_welcome(const struct owner *proprietor)
{
	char short_name[20];
	const char *owner_name = proprietor->name;

	size_t used = 0;

	if (one_in_(2))
		return;

	/* Preserve the owner's first name without overrunning this 20-byte
	 * message-local buffer or cutting a multibyte Unicode scalar. */
	while (owner_name[used] && owner_name[used] != ' ') {
		uint32_t cp;
		size_t width = angdroid_utf8_next_scalar(owner_name + used, &cp);
		if (!width || width > sizeof(short_name) - used - 1)
			break;
		memcpy(short_name + used, owner_name + used, width);
		used += width;
	}
	short_name[used] = '\0';

	if (one_in_(3)) {
		size_t i = randint0(N_ELEMENTS(comment_hint));
		msg(comment_hint[i], random_hint());
	} else if (player->lev > 5) {
		const char *player_name;

		/* We go from level 1 - 50  */
		size_t i = ((unsigned)player->lev - 1) / 5;
		i = MIN(i, N_ELEMENTS(comment_welcome) - 1);

		/* Get a title for the character */
		if ((i % 2) && randint0(2))
			player_name = player->class->title[(player->lev - 1) / 5];
		else if (randint0(2))
			player_name = player->full_name;
		else
			player_name = "valued customer";

		/* Balthazar says "Welcome" */
		prt(format(comment_welcome[i], short_name, player_name), 0, 0);
	}
}


/*** Display code ***/


/**
 * This function sets up screen locations based on the current term size.
 *
 * Current screen layout:
 *  line 0: reserved for messages
 *  line 1: shopkeeper and their purse / item buying price
 *  line 2: empty
 *  line 3: table headers
 *
 *  line 4: Start of items
 *
 * If help is turned off, then the rest of the display goes as:
 *
 *  line (height - 4): end of items
 *  line (height - 3): "more" prompt
 *  line (height - 2): empty
 *  line (height - 1): Help prompt and remaining gold
 *
 * If help is turned on, then the rest of the display goes as:
 *
 *  line (height - 7): end of items
 *  line (height - 6): "more" prompt
 *  line (height - 4): gold remaining
 *  line (height - 3): command help 
 */
static void store_display_recalc(struct store_context *ctx)
{
    int wid, hgt, content_height;
    bool show_help;
    region loc;
    angdroid_store_column_layout columns;
    struct menu *m = &ctx->menu;

    Term_get_size(&wid, &hgt);
    columns = angdroid_store_columns(
        wid, 1, store_is_home(ctx->store));
    /* Keep signed negative coordinates out of unsigned screen arrays. */
    text_out_wrap = MAX(1, columns.width - 2);
    ctx->scr_places_x[LOC_PRICE] =
        columns.price_col >= 0 ? columns.price_col : 0;
    ctx->scr_places_x[LOC_WEIGHT] =
        columns.weight_col >= 0 ? columns.weight_col : 0;
    ctx->scr_places_x[LOC_AU] = columns.gold_col;
    ctx->scr_places_x[LOC_OWNER] = columns.owner_end;

    /* A portrait viewport cannot safely fit the multi-line help footer.
     * Preserve selectable stock rows instead of assigning negative Y. */
    show_help = (ctx->flags & STORE_SHOW_HELP) &&
        wid >= 70 && hgt >= 16;
    content_height = show_help ? hgt - 3 : hgt;
    ctx->scr_places_y[LOC_OWNER] =
        angdroid_store_safe_row(1, hgt);
    ctx->scr_places_y[LOC_HEADER] =
        angdroid_store_safe_row(3, hgt);
    ctx->scr_places_y[LOC_MORE] =
        angdroid_store_safe_row(content_height - 3, hgt);
    ctx->scr_places_y[LOC_AU] =
        angdroid_store_safe_row(content_height - 1, hgt);

    loc = m->boundary;
    if (show_help) {
        ctx->scr_places_y[LOC_HELP_CLEAR] =
            angdroid_store_safe_row(content_height - 1, hgt);
        ctx->scr_places_y[LOC_HELP_PROMPT] =
            angdroid_store_safe_row(content_height, hgt);
        loc.page_rows = -5;
    } else {
        ctx->scr_places_y[LOC_HELP_CLEAR] =
            angdroid_store_safe_row(hgt - 2, hgt);
        ctx->scr_places_y[LOC_HELP_PROMPT] =
            angdroid_store_safe_row(hgt - 1, hgt);
        loc.page_rows = -2;
    }
    menu_layout(m, &loc);
}

/**
 * Redisplay a single store entry
 */
static void store_display_entry(struct menu *menu, int oid,
    bool cursor, int row, int col, int width)
{
    struct store_context *ctx = menu_priv(menu);
    struct store *store = ctx->store;
    angdroid_store_column_layout columns;
    struct object *obj;
    uint32_t desc = ODESC_PREFIX | ODESC_ZH_TW;
    int name_width;
    int32_t x;
    char o_name[256];
    char out_val[160];
    uint8_t colour;

    assert(store);
    if (!Term || !ctx->list || oid < 0 || oid >= store->stock_num ||
        !ctx->list[oid] || row < 0 || row >= Term->hgt ||
        col < 0 || col >= Term->wid || width <= 0)
        return;
    obj = ctx->list[oid];
    columns = angdroid_store_columns(
        Term->wid, 1, store_is_home(store));
    width = MIN(width, Term->wid - col);
    name_width = angdroid_store_name_budget(columns, col, width);

    /* Home descriptions retain the actual inscriptions. The screen name
     * is shortened only in the temporary buffer, never in game data. */
    if (store_is_home(store))
        desc |= ODESC_FULL;
    else
        desc |= ODESC_FULL | ODESC_STORE;
    object_desc(o_name, sizeof(o_name), obj, desc, player);
    fa_store_put_clipped(obj->kind->base->attr,
        o_name, row, col, name_width);

    colour = curs_attrs[CURS_KNOWN][(int)cursor];
    if (columns.weight_col >= 0 &&
        columns.weight_col >= col &&
        columns.weight_col < col + width) {
        int weight = object_weight_one(obj);
        int budget = MIN(10, col + width - columns.weight_col);
        if (angdroid_store_weight_text(
                out_val, sizeof(out_val), weight, angdroid_zh_tw_enabled()) &&
            angdroid_utf8_display_columns(out_val) <= (size_t)budget)
            fa_store_put_clipped(colour, out_val, row,
                columns.weight_col, budget);
    }

    if (!store_is_home(store) && columns.price_col >= 0 &&
        columns.price_col >= col &&
        columns.price_col < col + width) {
        int budget = MIN(14, col + width - columns.price_col);
        x = price_item(store, obj, false, 1);
        if (player->au < x)
            colour = curs_attrs[CURS_UNKNOWN][(int)cursor];
        if (angdroid_store_price_text(out_val, sizeof(out_val),
                (long)x, tval_can_have_charges(obj) && obj->number > 1,
                angdroid_zh_tw_enabled()) &&
            angdroid_utf8_display_columns(out_val) <= (size_t)budget)
            fa_store_put_clipped(colour, out_val, row,
                columns.price_col, budget);
    }
}

/**
 * Display store (after clearing screen)
 */
static void store_display_frame(struct store_context *ctx)
{
    char buf[160];
    struct store *store = ctx->store;
    struct owner *proprietor = store->owner;
    bool home = store_is_home(store);
    angdroid_store_column_layout layout =
        angdroid_store_columns(Term->wid, 1, home);
    int owner_row = ctx->scr_places_y[LOC_OWNER];
    int header_row = ctx->scr_places_y[LOC_HEADER];

    Term_clear();
    if (home) {
        fa_store_put_clipped(COLOUR_WHITE,
            angdroid_game_ui_text("Your Home", angdroid_zh_tw_enabled()),
            owner_row, 1, MAX(0, layout.width - 2));
        fa_store_put_clipped(COLOUR_WHITE,
            angdroid_game_ui_text("Home Inventory",
                angdroid_zh_tw_enabled()),
            header_row, 1, layout.name_cols);
    } else {
        const char *shop = angdroid_store_display_name(
            store->name, angdroid_zh_tw_enabled());
        const char *owner_name = proprietor->name;
        const char *owner_race = proprietor->race->name;
        size_t owner_cols, summary_cols;

        strnfmt(buf, sizeof(buf), "%s (%s)", owner_name, owner_race);
        owner_cols = angdroid_utf8_display_columns(buf);
        fa_store_put_clipped(COLOUR_WHITE, buf,
            owner_row, 1, MAX(0, layout.width - 2));

        /* A right-aligned stock/price summary is optional: do not
         * overwrite the shopkeeper's name in a portrait window. */
        strnfmt(buf, sizeof(buf), "%s (%ld)",
            shop, (long)proprietor->max_cost);
        summary_cols = angdroid_utf8_display_columns(buf);
        if (summary_cols + owner_cols + 4 <= (size_t)layout.width) {
            int summary_col = layout.width - 2 - (int)summary_cols;
            fa_store_put_clipped(COLOUR_WHITE, buf,
                owner_row, summary_col, (int)summary_cols);
        }

        fa_store_put_clipped(COLOUR_WHITE,
            angdroid_game_ui_text(
                "Store Inventory", angdroid_zh_tw_enabled()),
            header_row, 1, layout.name_cols);
    }

    if (layout.weight_col >= 0)
        fa_store_put_clipped(COLOUR_WHITE,
            angdroid_game_ui_text("Weight", angdroid_zh_tw_enabled()),
            header_row, layout.weight_col + 2, 8);
    if (layout.price_col >= 0)
        fa_store_put_clipped(COLOUR_WHITE,
            angdroid_game_ui_text("Price", angdroid_zh_tw_enabled()),
            header_row, layout.price_col + 4, 10);
}

/**
 * Display help.
 */
static void store_display_help(struct store_context *ctx)
{
	struct store *store = ctx->store;
	int help_loc = ctx->scr_places_y[LOC_HELP_PROMPT];
	bool is_home = store_is_home(store);
	bool zh_tw = angdroid_zh_tw_enabled();

	/* Clear */
	clear_from(ctx->scr_places_y[LOC_HELP_CLEAR]);

	/* Prepare help hooks */
	text_out_hook = text_out_to_screen;
	text_out_indent = 1;
	Term_gotoxy(1, help_loc);

	if (OPT(player, rogue_like_commands))
		text_out_c(COLOUR_L_GREEN, "x");
	else
		text_out_c(COLOUR_L_GREEN, "l");

	text_out("%s", angdroid_store_ui_text(" examines", zh_tw));
	if (!ctx->inspect_only) {
		text_out("%s", angdroid_store_ui_text(" and ", zh_tw));
		text_out_c(COLOUR_L_GREEN, "p");
		text_out("%s", angdroid_store_ui_text(" (or ", zh_tw));
		text_out_c(COLOUR_L_GREEN, "g");
		text_out("%s", angdroid_store_ui_text(")", zh_tw));

		if (is_home) text_out("%s", angdroid_store_ui_text(" picks up", zh_tw));
		else text_out("%s", angdroid_store_ui_text(" purchases", zh_tw));
	}
	text_out("%s", angdroid_store_ui_text(" an item. ", zh_tw));

	if (!ctx->inspect_only) {
		if (OPT(player, birth_no_selling) && !is_home) {
			text_out_c(COLOUR_L_GREEN, "d");
			text_out("%s", angdroid_store_ui_text(" (or ", zh_tw));
			text_out_c(COLOUR_L_GREEN, "s");
			text_out("%s", angdroid_store_ui_text(")", zh_tw));
			text_out("%s", angdroid_store_ui_text(" gives an item to the store in return for its identification. Some wands and staves will also be recharged. ", zh_tw));
		} else {
			text_out_c(COLOUR_L_GREEN, "d");
			text_out("%s", angdroid_store_ui_text(" (or ", zh_tw));
			text_out_c(COLOUR_L_GREEN, "s");
			text_out("%s", angdroid_store_ui_text(")", zh_tw));
			if (is_home) text_out("%s", angdroid_store_ui_text(" drops", zh_tw));
			else text_out("%s", angdroid_store_ui_text(" sells", zh_tw));
			text_out("%s", angdroid_store_ui_text(" an item from your inventory. ", zh_tw));
		}
	}
	text_out_c(COLOUR_L_GREEN, "I");
	text_out("%s", angdroid_store_ui_text(" inspects an item from your inventory. ", zh_tw));

	text_out_c(COLOUR_L_GREEN, "ESC");
	if (!ctx->inspect_only)
		text_out("%s", angdroid_store_ui_text(" exits the building.", zh_tw));
	else
		text_out("%s", angdroid_store_ui_text(" exits this screen.", zh_tw));

	text_out_indent = 0;
}

/**
 * Decides what parts of the store display to redraw.  Called on terminal
 * resizings and the redraw command.
 */
static void store_redraw(struct store_context *ctx)
{
	if (ctx->flags & (STORE_FRAME_CHANGE)) {
		store_display_frame(ctx);

		/* Multiline help requires a wide/tall surface. On phones,
		 * keep the stock rows and display only a one-line shortcut. */
		if ((ctx->flags & STORE_SHOW_HELP) &&
			Term->wid >= 70 && Term->hgt >= 16)
			store_display_help(ctx);
		else
			fa_store_put_clipped(COLOUR_WHITE,
				angdroid_zh_tw_enabled() ? "按 ? 查看說明" :
				"Press '?' for help.",
				ctx->scr_places_y[LOC_HELP_PROMPT], 1,
				Term->wid - 2);

		ctx->flags &= ~(STORE_FRAME_CHANGE);
	}

	if (ctx->flags & (STORE_GOLD_CHANGE)) {
		char gold_text[80];
		strnfmt(gold_text, sizeof(gold_text),
			angdroid_zh_tw_enabled() ? "剩餘金幣：%9ld" :
			"Gold Remaining: %9ld", (long)player->au);
		fa_store_put_clipped(COLOUR_WHITE, gold_text,
			ctx->scr_places_y[LOC_AU], ctx->scr_places_x[LOC_AU],
			Term->wid - (int)ctx->scr_places_x[LOC_AU]);
		ctx->flags &= ~(STORE_GOLD_CHANGE);
	}
}

static bool store_get_check(const char *prompt)
{
	struct keypress ch;

	/* Prompt for it */
	prt(prompt, 0, 0);

	/* Get an answer */
	ch = inkey();

	/* Erase the prompt */
	prt("", 0, 0);

	if (ch.code == ESCAPE) return (false);
	if (strchr("Nn", ch.code)) return (false);

	/* Success */
	return (true);
}

/*
 * Sell an object, or drop if it we're in the home.
 */
static bool store_sell(struct store_context *ctx)
{
	int amt;
	int get_mode = USE_EQUIP | USE_INVEN | USE_FLOOR | USE_QUIVER;

	struct store *store = ctx->store;

	struct object *obj;
	struct object object_type_body = OBJECT_NULL;
	struct object *temp_obj = &object_type_body;

	char o_name[120];

	item_tester tester = NULL;

	const char *reject = "You have nothing that I want. ";
	const char *prompt = OPT(player, birth_no_selling) ? "Give which item? " : "Sell which item? ";

	assert(store);

	/* Clear all current messages */
	msg_flag = false;
	prt("", 0, 0);

	if (store_is_home(store)) {
		prompt = "Drop which item? ";
	} else {
		tester = store_will_buy_tester;
		get_mode |= SHOW_PRICES;
	}

	/* Get an item */
	player->upkeep->command_wrk = USE_INVEN;

	if (!get_item(&obj, prompt, reject, CMD_DROP, tester, get_mode))
		return false;

	/* Cannot remove stickied objects */
	if (object_is_equipped(player->body, obj) && !obj_can_takeoff(obj)) {
		/* Oops */
		msg("Hmmm, it seems to be stuck.");

		/* Nope */
		return false;
	}

	/* Get a quantity */
	amt = get_quantity(NULL, obj->number);

	/* Allow user abort */
	if (amt <= 0) return false;

	/* Get a copy of the object representing the number being sold */
	object_copy_amt(temp_obj, obj, amt);

	if (!store_check_num(store, temp_obj)) {
		object_wipe(temp_obj);
		if (store_is_home(store))
			msg("Your home is full.");
		else
			msg("I have not the room in my store to keep it.");

		return false;
	}

	/* Get a full description */
	object_desc(o_name, sizeof(o_name), temp_obj,
		ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW, player);

	/* Real store */
	if (!store_is_home(store)) {
		/* Extract the value of the items */
		int32_t price = price_item(store, temp_obj, true, amt);

		object_wipe(temp_obj);
		screen_save();

		/* Show price */
		if (!OPT(player, birth_no_selling))
			prt(format("Price: %ld", (long)price), 1, 0);

		/* Confirm sale */
		if (!store_get_check(format("%s %s? [ESC, any other key to accept]",
				OPT(player, birth_no_selling) ? "Give" : "Sell", o_name))) {
			screen_load();
			return false;
		}

		screen_load();

		cmdq_push(CMD_SELL);
		cmd_set_arg_item(cmdq_peek(), "item", obj);
		cmd_set_arg_number(cmdq_peek(), "quantity", amt);
	} else { /* Player is at home */
		object_wipe(temp_obj);
		cmdq_push(CMD_STASH);
		cmd_set_arg_item(cmdq_peek(), "item", obj);
		cmd_set_arg_number(cmdq_peek(), "quantity", amt);
	}

	/* Update the display */
	ctx->flags |= STORE_GOLD_CHANGE;

	return true;
}



/**
 * Buy an object from a store
 */
static bool store_purchase(struct store_context *ctx, int item, bool single)
{
	struct store *store = ctx->store;

	struct object *obj = ctx->list[item];
	struct object *dummy = NULL;

	char o_name[80];

	int amt, num;

	int32_t price;

	/* Clear all current messages */
	msg_flag = false;
	prt("", 0, 0);


	/*** Check the player can get any at all ***/

	/* Get an amount if we weren't given one */
	if (single) {
		amt = 1;

		/* Check if the player can afford any at all */
		if (!store_is_home(store) &&
				player->au < price_item(store, obj, false, 1)) {
			msg("You do not have enough gold for this item.");
			return false;
		}
	} else {
		bool flavor_aware;

		if (store_is_home(store)) {
			amt = obj->number;
		} else {
			/* Price of one */
			price = price_item(store, obj, false, 1);

			/* Check if the player can afford any at all */
			if ((uint32_t)player->au < (uint32_t)price) {
				msg("You do not have enough gold for this item.");
				return false;
			}

			/* Work out how many the player can afford */
			if (price == 0)
				amt = obj->number; /* Prevent division by zero */
			else
				amt = player->au / price;

			if (amt > obj->number) amt = obj->number;

			/* Double check for wands/staves */
			if ((player->au >= price_item(store, obj, false, amt+1)) &&
				(amt < obj->number))
				amt++;
		}

		/* Limit to the number that can be carried */
		amt = MIN(amt, inven_carry_num(player, obj));

		/* Fail if there is no room.  Don't leak information about
		 * unknown flavors for a purchase (getting it from home doesn't
		 * leak information since it doesn't show the true flavor). */
		flavor_aware = object_flavor_is_aware(obj);
		if (amt <= 0 || (!flavor_aware && !store_is_home(store) &&
						 pack_is_full())) {
			msg("You cannot carry that many items.");
			return false;
		}

		/* Find the number of this item in the inventory.  As above,
		 * avoid leaking information about unknown flavors. */
		if (!flavor_aware && !store_is_home(store))
			num = 0;
		else
			num = find_inven(obj);

		strnfmt(o_name, sizeof o_name, "%s how many%s? (max %d) ",
				(store_is_home(store)) ? "Take" : "Buy",
				num ? format(" (you have %d)", num) : "", amt);

		/* Get a quantity */
		amt = get_quantity(o_name, amt);

		/* Allow user abort */
		if (amt <= 0) return false;
	}

	/* Get desired object */
	dummy = object_new();
	object_copy_amt(dummy, obj, amt);

	/* Ensure we have room */
	if (!inven_carry_okay(dummy)) {
		msg("You cannot carry that many items.");
		object_delete(NULL, NULL, &dummy);
		return false;
	}

	/* Attempt to buy it */
	if (!store_is_home(store)) {
		bool response;

		bool obj_is_book = tval_is_book_k(obj->kind);
		bool obj_can_use = !obj_is_book || obj_can_browse(obj);

		/* Describe the object (fully) */
		object_desc(o_name, sizeof(o_name), dummy,
			ODESC_PREFIX | ODESC_FULL | ODESC_STORE | ODESC_ZH_TW, player);

		/* Extract the price for the entire stack */
		price = price_item(store, dummy, false, dummy->number);

		screen_save();

		/* Show price */
		prt(format("Price: %ld", (long)price), 1, 0);

		/* Confirm purchase */
		response = store_get_check(format("Buy %s?%s %s",
					o_name,
					obj_can_use ? "" : " (Can't use!)",
					"[ESC, any other key to accept]"));

		screen_load();

		/* Negative response, so give up */
		if (!response) return false;

		cmdq_push(CMD_BUY);
		cmd_set_arg_item(cmdq_peek(), "item", obj);
		cmd_set_arg_number(cmdq_peek(), "quantity", amt);
	} else {
		/* Home is much easier */
		cmdq_push(CMD_RETRIEVE);
		cmd_set_arg_item(cmdq_peek(), "item", obj);
		cmd_set_arg_number(cmdq_peek(), "quantity", amt);
	}

	/* Update the display */
	ctx->flags |= STORE_GOLD_CHANGE;

	object_delete(NULL, NULL, &dummy);

	/* Not kicked out */
	return true;
}


/**
 * Examine an item in a store
 */
static void store_examine(struct store_context *ctx, int item)
{
	struct object *obj;
	char header[120];
	textblock *tb;
	region area = { 0, 0, 0, 0 };
	uint32_t odesc_flags = ODESC_PREFIX | ODESC_FULL;

	if (item < 0) return;

	/* Get the actual object */
	obj = ctx->list[item];

	/* Items in the home get less description */
	if (store_is_home(ctx->store)) {
		odesc_flags |= ODESC_CAPITAL;
	} else {
		odesc_flags |= ODESC_STORE;
	}

	/* Hack -- no flush needed */
	msg_flag = false;

	/* Show full info in most stores, but normal info in player home */
	tb = object_info(obj, OINFO_NONE);
	object_desc(header, sizeof(header), obj, odesc_flags | ODESC_ZH_TW, player);

	textui_textblock_show(tb, area, header);
	textblock_free(tb);

	/* Hack -- Browse book, then prompt for a command */
	if (obj_can_browse(obj))
		textui_book_browse(obj);
}


static void store_menu_set_selections(struct menu *menu, bool knowledge_menu)
{
	if (knowledge_menu) {
		if (OPT(player, rogue_like_commands)) {
			/* These two can't intersect! */
			menu->cmd_keys = "?|Ieilx";
			menu->selections = "abcdfghmnopqrstuvwyzABCDEFGHJKLMNOPQRSTUVWXYZ";
		} else {
			/* These two can't intersect! */
			menu->cmd_keys = "?|Ieil";
			menu->selections = "abcdfghjkmnopqrstuvwxyzABCDEFGHJKLMNOPQRSTUVWXYZ";
		}
	} else {
		if (OPT(player, rogue_like_commands)) {
			/* These two can't intersect! */
			menu->cmd_keys = "\x04\x05\x10?={|}~CEIPTdegilpswx"; /* \x10 = ^p , \x04 = ^D, \x05 = ^E */
			menu->selections = "abcfmnoqrtuvyzABDFGHJKLMNOQRSUVWXYZ";
		} else {
			/* These two can't intersect! */
			menu->cmd_keys = "\x05\x010?={|}~CEIbdegiklpstwx"; /* \x05 = ^E, \x10 = ^p */
			menu->selections = "acfhjmnoqruvyzABDFGHJKLMNOPQRSTUVWXYZ";
		}
	}
}

static void store_menu_recalc(struct menu *m)
{
	struct store_context *ctx = menu_priv(m);
	menu_setpriv(m, ctx->store->stock_num, ctx);
}

/**
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands in the stores
 * which are not allowed in the dungeon, and we must disable some commands
 * which are allowed in the dungeon but not in the stores, to prevent chaos.
 */
static bool store_process_command_key(struct keypress kp)
{
	int cmd = 0;

	/* Hack -- no flush needed */
	prt("", 0, 0);
	msg_flag = false;

	/* Process the keycode */
	switch (kp.code) {
		case 'T': /* roguelike */
		case 't': cmd = CMD_TAKEOFF; break;

		case KTRL('D'): /* roguelike */
		case 'k': textui_cmd_ignore(); break;

		case 'P': /* roguelike */
		case 'b': textui_spell_browse(); break;

		case '~': textui_browse_knowledge(); break;
		case 'I': textui_obj_examine(); break;
		case 'w': cmd = CMD_WIELD; break;
		case '{': cmd = CMD_INSCRIBE; break;
		case '}': cmd = CMD_UNINSCRIBE; break;

		case 'e': do_cmd_equip(); break;
		case 'i': do_cmd_inven(); break;
		case '|': do_cmd_quiver(); break;
		case KTRL('E'): toggle_inven_equip(); break;
		case 'C': do_cmd_change_name(); break;
		case KTRL('P'): do_cmd_messages(); break;
		case ')': do_cmd_save_screen(); break;

		default: return false;
	}

	if (cmd)
		cmdq_push_repeat(cmd, 0);

	return true;
}

/**
 * Select an item from the store's stock, and return the stock index
 */
static int store_get_stock(struct menu *m, int oid)
{
	ui_event e;
	int no_act = m->flags & MN_NO_ACTION;

	/* Set a flag to make sure that we get the selection or escape
	 * without running the menu handler */
	m->flags |= MN_NO_ACTION;
	e = menu_select(m, 0, true);
	if (!no_act) {
		m->flags &= ~MN_NO_ACTION;
	}

	if (e.type == EVT_SELECT) {
		return m->cursor;
	} else if (e.type == EVT_ESCAPE) {
		return -1;
	}

	/* if we do not have a new selection, just return the original item */
	return oid;
}

/** Enum for context menu entries */
enum {
	ACT_INSPECT_INVEN,
	ACT_SELL,
	ACT_EXAMINE,
	ACT_BUY,
	ACT_BUY_ONE,
	ACT_EXIT
};

/* pick the context menu options appropiate for a store */
static int context_menu_store(struct store_context *ctx, const int oid, int mx, int my)
{
	bool home = store_is_home(ctx->store);

	struct menu *m = menu_dynamic_new();

	int selected;
	char *labels = string_make(lower_case);
	m->selections = labels;

	menu_dynamic_add_label(m, "Inspect inventory", 'I', ACT_INSPECT_INVEN, labels);
	if (!ctx->inspect_only) {
		menu_dynamic_add_label(m, home ? "Stash" : "Sell", 'd',
			ACT_SELL, labels);
	}
	menu_dynamic_add_label(m, "Exit", '`', ACT_EXIT, labels);

	/* Hack -- no flush needed */
	msg_flag = false;
	screen_save();

	menu_dynamic_calc_location(m, mx, my);
	region_erase_bordered(&m->boundary);

	prt("(Enter to select, ESC) Command:", 0, 0);
	selected = menu_dynamic_select(m);

	menu_dynamic_free(m);
	string_free(labels);

	screen_load();

	switch (selected) {
		case ACT_SELL:
			store_sell(ctx);
			break;
		case ACT_INSPECT_INVEN:
			textui_obj_examine();
			break;
		case ACT_EXIT:
			return false;
	}

	return true;
}

/* Make 'g' a synonym of 'p' for an item's context menu in the store. */
static bool handle_g_context_store_item(struct menu *menu,
		const ui_event *event, int oid)
{
	if (event->type == EVT_KBRD && event->key.code == 'g') {
		ui_event mod_event, out_event;

		mod_event = *event;
		mod_event.key.code = 'p';
		return menu_handle_keypress(menu, &mod_event, &out_event);
	}
	return false;
}

/* pick the context menu options appropiate for an item available in a store */
static bool context_menu_store_item(struct store_context *ctx, const int oid, int mx, int my)
{
	bool home = store_is_home(ctx->store);

	struct menu *m = menu_dynamic_new();
	struct object *obj = ctx->list[oid];
	menu_iter mod_iter;
	int selected;
	char *labels;
	char header[120];

	object_desc(header, sizeof(header), obj,
		ODESC_PREFIX | ODESC_FULL | ((home) ? 0 : ODESC_STORE) | ODESC_ZH_TW, player);

	labels = string_make(lower_case);
	m->selections = labels;

	menu_dynamic_add_label(m, "Examine", (OPT(player, rogue_like_commands))
		? 'x' : 'l', ACT_EXAMINE, labels);
	if (!ctx->inspect_only) {
		menu_dynamic_add_label(m, home ? "Take" : "Buy", 'p',
			ACT_BUY, labels);
		if (obj->number > 1) {
			menu_dynamic_add_label(m, home ? "Take one" : "Buy one",
				'o', ACT_BUY_ONE, labels);
		}
		/*
		 * This a bit of a hack so 'g' acts like 'p' (as it does when
		 * there isn't a selected item). Has to be done after all the
		 * labels have been added to avoid assertion failures.
		 */
		mod_iter = *m->row_funcs;
		mod_iter.row_handler = handle_g_context_store_item;
		m->row_funcs = &mod_iter;
		m->switch_keys = "g";
	}

	/* Hack -- no flush needed */
	msg_flag = false;
	screen_save();

	menu_dynamic_calc_location(m, mx, my);
	region_erase_bordered(&m->boundary);

	prt(format("(Enter to select, ESC) Command for %s:", header), 0, 0);
	selected = menu_dynamic_select(m);

	menu_dynamic_free(m);
	string_free(labels);

	screen_load();

	switch (selected) {
		case ACT_EXAMINE:
			store_examine(ctx, oid);
			return false;
		case ACT_BUY:
			return store_purchase(ctx, oid, false);
		case ACT_BUY_ONE:
			return store_purchase(ctx, oid, true);
	}
	return false;
}

/**
 * Handle store menu input
 */
static bool store_menu_handle(struct menu *m, const ui_event *event, int oid)
{
	bool processed = true;
	struct store_context *ctx = menu_priv(m);
	struct store *store = ctx->store;
	
	if (event->type == EVT_SELECT) {
		/* Hack -- there's no mouse event coordinates to use for */
		/* menu_store_item, so fake one as if mouse clicked on letter */
		bool purchased = context_menu_store_item(ctx, oid, 1, m->active.row + oid);
		ctx->flags |= (STORE_FRAME_CHANGE | STORE_GOLD_CHANGE);

		/* Let the game handle any core commands (equipping, etc) */
		cmdq_pop(CTX_STORE);

		/* Notice and handle stuff */
		notice_stuff(player);
		handle_stuff(player);

		if (purchased) {
			/* Display the store */
			store_display_recalc(ctx);
			store_menu_recalc(m);
			store_redraw(ctx);
		}

		return true;
	} else if (event->type == EVT_MOUSE) {
		if (event->mouse.button == 2) {
			/* exit the store? what already does this? menu_handle_mouse
			 * so exit this so that menu_handle_mouse will be called */
			return false;
		} else if (event->mouse.button == 1) {
			bool action = false;
			if ((event->mouse.y == 0) || (event->mouse.y == 1)) {
				/* show the store context menu */
				if (context_menu_store(ctx, oid, event->mouse.x, event->mouse.y) == false)
					return false;

				action = true;
			} else if ((oid >= 0) && (event->mouse.y == m->active.row + oid)) {
				/* if press is on a list item, so store item context */
				context_menu_store_item(ctx, oid, event->mouse.x,
										event->mouse.y);
				action = true;
			}

			if (action) {
				ctx->flags |= (STORE_FRAME_CHANGE | STORE_GOLD_CHANGE);

				/* Let the game handle any core commands (equipping, etc) */
				cmdq_pop(CTX_STORE);

				/* Notice and handle stuff */
				notice_stuff(player);
				handle_stuff(player);

				/* Display the store */
				store_display_recalc(ctx);
				store_menu_recalc(m);
				store_redraw(ctx);

				return true;
			}
		}
	} else if (event->type == EVT_KBRD) {
		switch (event->key.code) {
			case 's':
			case 'd': store_sell(ctx); break;

			case 'p':
			case 'g':
				/* use the old way of purchasing items */
				msg_flag = false;
				if (!store_is_home(store)) {
					prt("Purchase which item? (ESC to cancel, Enter to select)",
						0, 0);
				} else {
					prt("Get which item? (Esc to cancel, Enter to select)",
						0, 0);
				}
				oid = store_get_stock(m, oid);
				prt("", 0, 0);
				if (oid >= 0) {
					store_purchase(ctx, oid, false);
				}
				break;
			case 'l':
			case 'x':
				/* use the old way of examining items */
				msg_flag = false;
				prt("Examine which item? (ESC to cancel, Enter to select)",
					0, 0);
				oid = store_get_stock(m, oid);
				prt("", 0, 0);
				if (oid >= 0) {
					store_examine(ctx, oid);
				}
				break;

			case '?': {
				/* Toggle help */
				if (ctx->flags & STORE_SHOW_HELP)
					ctx->flags &= ~(STORE_SHOW_HELP);
				else
					ctx->flags |= STORE_SHOW_HELP;

				/* Redisplay */
				ctx->flags |= STORE_INIT_CHANGE;

				store_display_recalc(ctx);
				store_redraw(ctx);

				break;
			}

			case '=': {
				do_cmd_options();
				store_menu_set_selections(m, false);
				break;
			}

			default:
				processed = store_process_command_key(event->key);
		}

		/* Let the game handle any core commands (equipping, etc) */
		cmdq_pop(CTX_STORE);

		if (processed) {
			event_signal(EVENT_INVENTORY);
			event_signal(EVENT_EQUIPMENT);
		}

		/* Notice and handle stuff */
		notice_stuff(player);
		handle_stuff(player);

		return processed;
	}

	return false;
}

static region store_menu_region = { 1, 4, -1, -2 };
static const menu_iter store_menu =
{
	NULL,
	NULL,
	store_display_entry,
	store_menu_handle,
	NULL
};

/**
 * Init the store menu
 */
static void store_menu_init(struct store_context *ctx, struct store *store, bool inspect_only)
{
	struct menu *menu = &ctx->menu;

	ctx->store = store;
	ctx->flags = STORE_INIT_CHANGE;
	ctx->inspect_only = inspect_only;
	ctx->list = mem_zalloc(sizeof(struct object *) * z_info->store_inven_max);

	store_stock_list(ctx->store, ctx->list, z_info->store_inven_max);

	/* Init the menu structure */
	menu_init(menu, MN_SKIN_SCROLL, &store_menu);
	menu_setpriv(menu, 0, ctx);

	/* Calculate the positions of things and draw */
	menu_layout(menu, &store_menu_region);
	store_menu_set_selections(menu, inspect_only);
	store_display_recalc(ctx);
	store_menu_recalc(menu);
	store_redraw(ctx);
}

/**
 * Display contents of a store from knowledge menu
 *
 * The only allowed actions are 'I' to inspect an item
 */
void textui_store_knowledge(struct store *store)
{
	struct store_context ctx;

	screen_save();
	clear_from(0);

	if (!store) {
		prt("You have no home!", 15, 0);
		anykey();
		screen_load();
		return;
	}

	store_menu_init(&ctx, store, true);
	menu_select(&ctx.menu, 0, false);

	/* Flush messages XXX XXX XXX */
	event_signal(EVENT_MESSAGE_FLUSH);

	screen_load();

	mem_free(ctx.list);
}


/**
 * Handle stock change.
 */
static void refresh_stock(game_event_type type, game_event_data *unused, void *user)
{
	struct store_context *ctx = user;
	struct menu *menu = &ctx->menu;

	store_stock_list(ctx->store, ctx->list, z_info->store_inven_max);

	/* Display the store */
	store_display_recalc(ctx);
	store_menu_recalc(menu);
	store_redraw(ctx);
}

/**
 * Enter a store.
 */
void enter_store(game_event_type type, game_event_data *data, void *user)
{
	struct store *store = store_at(cave, player->grid);

	/* Check that we're on a store */
	if (!store) {
		msg("You see no store here.");
		return;
	}

	sound(store_is_home(store) ? MSG_STORE_HOME : MSG_STORE_ENTER);

	/* Shut down the normal game view */
	event_signal(EVENT_LEAVE_WORLD);
}

/**
 * Interact with a store.
 */
void use_store(game_event_type type, game_event_data *data, void *user)
{
	struct store *store = store_at(cave, player->grid);
	struct store_context ctx;

	/* Check that we're on a store */
	if (!store) return;

	/*** Display ***/

	/* Save current screen (ie. dungeon) */
	screen_save();
	msg_flag = false;

	/* Get a array version of the store stock, register handler for changes */
	event_add_handler(EVENT_STORECHANGED, refresh_stock, &ctx);
	store_menu_init(&ctx, store, false);

	/* Say a friendly hello. */
	if (!store_is_home(store))
		prt_welcome(store->owner);

	/* Shopping */
	menu_select(&ctx.menu, 0, false);

	/* Shopping's done */
	event_remove_handler(EVENT_STORECHANGED, refresh_stock, &ctx);
	msg_flag = false;
	mem_free(ctx.list);

	/* Take a turn */
	player->upkeep->energy_use = z_info->move_energy;

	/* Flush messages */
	event_signal(EVENT_MESSAGE_FLUSH);

	/* Load the screen */
	screen_load();
}

void leave_store(game_event_type type, game_event_data *data, void *user)
{
	/* Disable repeats */
	cmd_disable_repeat();

	sound(MSG_STORE_LEAVE);

	/* Switch back to the normal game view. */
	event_signal(EVENT_ENTER_WORLD);

	/* Update the visuals */
	player->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw entire screen */
	player->upkeep->redraw |= (PR_BASIC | PR_EXTRA);

	/* Redraw map */
	player->upkeep->redraw |= (PR_MAP);
}
