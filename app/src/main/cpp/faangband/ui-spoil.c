/**
 * \file ui-spoil.c
 * \brief Create menu for spoiler file generation
 *
 * Copyright (c) 2021 Eric Branlund
 * Copyright (c) 1997 Ben Harrison, and others
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
#include "ui-input.h"
#include "ui-menu.h"
#include "ui-output.h"
#include "ui-spoil.h"
#include "../angband/zh-tw-ui.h"


static void spoiler_menu_act(const char *title, int row)
{
	if (row == 0) {
		cmdq_push(CMD_SPOIL_OBJ);
	} else if (row == 1) {
		cmdq_push(CMD_SPOIL_ARTIFACT);
	} else if (row == 2) {
		cmdq_push(CMD_SPOIL_MON_BRIEF);
	} else if (row == 3) {
		cmdq_push(CMD_SPOIL_MON);
	} else {
		assert(0);
	}
	cmdq_execute((player->is_dead) ? CTX_DEATH : CTX_GAME);

	event_signal(EVENT_MESSAGE_FLUSH);
}


static struct menu *spoil_menu = NULL;

/* The action table is kept alive because menu_new_action() stores its pointer.
 * The displayed labels change with the language, but command IDs, callbacks
 * and spoiler filenames remain the game's original data. */
static menu_action spoil_actions[] =
{
	{ 0, 0, "Brief Object Info (obj-desc.spo)", spoiler_menu_act },
	{ 0, 0, "Brief Artifact Info (artifact.spo)", spoiler_menu_act },
	{ 0, 0, "Brief Monster Info (mon-desc.spo)", spoiler_menu_act },
	{ 0, 0, "Full Monster Info (mon-info.spo)", spoiler_menu_act },
};

static void fa_spoiler_set_labels(bool zh_tw)
{
	spoil_actions[0].name = zh_tw ?
		"物品摘要 (obj-desc.spo)" : "Brief Object Info (obj-desc.spo)";
	spoil_actions[1].name = zh_tw ?
		"神器摘要 (artifact.spo)" : "Brief Artifact Info (artifact.spo)";
	spoil_actions[2].name = zh_tw ?
		"怪物摘要 (mon-desc.spo)" : "Brief Monster Info (mon-desc.spo)";
	spoil_actions[3].name = zh_tw ?
		"完整怪物資訊 (mon-info.spo)" : "Full Monster Info (mon-info.spo)";
	if (spoil_menu)
		spoil_menu->title = zh_tw ? "產生遊戲資料檔" : "Create spoilers";
}


/**
 * Display menu for generating spoiler files.
 */
void do_cmd_spoilers(void)
{
	if (!Term || Term->wid <= 0 || Term->hgt <= 0)
		return;
	if (!spoil_menu) {
		spoil_menu = menu_new_action(spoil_actions,
			N_ELEMENTS(spoil_actions));
		spoil_menu->selections = all_letters_nohjkl;
	}

	/* Refresh cached menu labels on every visit, including a language
	 * switch without restarting the game. */
	fa_spoiler_set_labels(angdroid_zh_tw_enabled());
	screen_save();
	clear_from(0);
	menu_layout(spoil_menu, &SCREEN_REGION);
	menu_select(spoil_menu, 0, false);
	if (Term) screen_load();
}
