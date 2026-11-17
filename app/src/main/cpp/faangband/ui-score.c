/**
 * \file ui-score.c
 * \brief Highscore display for Angband
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
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
#include "buildid.h"
#include "game-world.h"
#include "score.h"
#include "ui-input.h"
#include "ui-output.h"
#include "ui-score.h"
#include "ui-term.h"
#include "../angband/zh-tw-ui.h"
#include "../angband/menu-text-fit.h"

/* High scores store canonical data in English. Only clip and localize the
 * terminal presentation, without changing highscore records or game state. */
static void fa_score_put_clipped(uint8_t attr, const char *text, int row,
								int col)
{
	char visible[256];
	if (!Term || !text || row < 0 || row >= Term->hgt ||
		col < 0 || col >= Term->wid) return;
	angdroid_menu_copy_columns(visible, sizeof(visible), text,
		(size_t)(Term->wid - col));
	if (visible[0]) c_put_str(attr, visible, row, col);
}

/**
 * Display a page of scores
 */
/* One score needs three text rows starting at row 2, plus a footer row.
 * Never let fixed five-entry pages draw over the footer on short screens. */
static int fa_score_page_entries(int terminal_height)
{
	if (terminal_height < 6) return 0;
	return MIN(5, (terminal_height - 2) / 4);
}

static void display_score_page(const struct high_score scores[], int start,
							   int count, int highlight, int page_entries)
{
	int n;

	/* Dump 5 entries */
	for (n = 0; start < count && n < page_entries; start++, n++) {
		const struct high_score *score = &scores[start];
		uint8_t attr;
		int clev, mlev, cdun, mdun;
		const char *user, *gold, *when, *aged;
		struct player_class *c;
		struct player_race *r;
		char out_val[160];
		char tmp_val[160];
		/* Preserve original indentation at 80 columns, reclaim room on
		 * narrow mobile terminals so the details remain legible. */
		int details_col = Term && Term->wid < 60 ? 0 : 15;

		/* Hack -- indicate death in yellow */
		attr = (start == highlight) ? COLOUR_L_GREEN : COLOUR_WHITE;

		c = player_id2class(atoi(score->p_c));
		r = player_id2race(atoi(score->p_r));

		/* Extract the level info */
		clev = atoi(score->cur_lev);
		mlev = atoi(score->max_lev);
		cdun = atoi(score->cur_dun);
		mdun = atoi(score->max_dun);

		/* Hack -- extract the gold and such */
		for (user = score->uid; isspace((unsigned char)*user); user++)
			/* loop */;
		for (when = score->day; isspace((unsigned char)*when); when++)
			/* loop */;
		for (gold = score->gold; isspace((unsigned char)*gold); gold++)
			/* loop */;
		for (aged = score->turns; isspace((unsigned char)*aged); aged++)
			/* loop */;

		/* Display only: never change the stored race/class/player IDs. */
		if (angdroid_zh_tw_enabled())
			strnfmt(out_val, sizeof(out_val),
				"%3d.%9s  %s（%s %s），等級 %d",
				start + 1, score->pts, score->who,
				r ? r->name : "<none>", c ? c->name : "<none>",
				clev);
		else
			strnfmt(out_val, sizeof(out_val),
				"%3d.%9s  %s the %s %s, level %d",
				start + 1, score->pts, score->who,
				r ? r->name : "<none>", c ? c->name : "<none>",
				clev);

		/* Append a "maximum level" */
		if (mlev > clev)
			my_strcat(out_val, angdroid_zh_tw_enabled() ?
				format("（最高 %d）", mlev) : format(" (Max %d)", mlev),
				sizeof(out_val));

		/* Dump the first line */
		fa_score_put_clipped(attr, out_val, n * 4 + 2, 0);


		/* Died where? */
		if (!cdun) {
			if (angdroid_zh_tw_enabled())
				strnfmt(out_val, sizeof(out_val), "在城鎮死於 %s",
					score->how);
			else
				strnfmt(out_val, sizeof(out_val), "Killed by %s in the town",
					score->how);
		} else {
			if (angdroid_zh_tw_enabled())
				strnfmt(out_val, sizeof(out_val),
					"在地下城第 %d 層死於 %s", cdun, score->how);
			else
				strnfmt(out_val, sizeof(out_val),
					"Killed by %s on dungeon level %d", score->how, cdun);
		}

		/* Append a "maximum level" */
		if (mdun > cdun)
			my_strcat(out_val, angdroid_zh_tw_enabled() ?
				format("（最高 %d）", mdun) : format(" (Max %d)", mdun),
				sizeof(out_val));

		/* Dump the info */
		fa_score_put_clipped(attr, out_val, n * 4 + 3, details_col);


		/* Clean up standard encoded form of "when" */
		if ((*when == '@') && strlen(when) == 9) {
			strnfmt(tmp_val, sizeof(tmp_val), "%.4s-%.2s-%.2s", when + 1,
					when + 5, when + 7);
			when = tmp_val;
		}

		/* And still another line of info */
		if (angdroid_zh_tw_enabled())
			strnfmt(out_val, sizeof(out_val),
				"（玩家 %s，日期 %s，金幣 %s，回合 %s）",
				user, when, gold, aged);
		else
			strnfmt(out_val, sizeof(out_val),
				"(User %s, Date %s, Gold %s, Turn %s).",
				user, when, gold, aged);
		fa_score_put_clipped(attr, out_val, n * 4 + 4, details_col);
	}
}

/**
 * Display the scores in a given range.
 */
static void display_scores_aux(const struct high_score scores[], int from,
							   int to, int highlight, bool allow_scrolling)
{
	struct keypress ch;
	int k, count;
	int page_entries;

	if (!Term || Term->wid <= 0 || Term->hgt <= 0) return;

	/* Assume we will show the first 10 */
	if (from < 0) from = 0;
	if (to < 0) to = allow_scrolling ? 5 : 10;
	if (to > MAX_HISCORES) to = MAX_HISCORES;

	/* Hack -- Count the high scores */
	for (count = 0; count < MAX_HISCORES; count++)
		if (!scores[count].what[0])
			break;

	/* Forget about the last entries */
	if ((count > to) && !allow_scrolling) count = to;

	/*
	 * Move only as many records as the terminal can show. Unless scrolling
	 * is allowed, move forward and stop once the end is reached.
	 */
	k = from;
	while (1) {
		/* Recompute after rotations: the available rows can change while
		 * the user is selecting the next score page. */
		if (!Term || Term->wid <= 0 || Term->hgt <= 0) return;
		page_entries = fa_score_page_entries(Term->hgt);
		if (page_entries == 0) {
			Term_clear();
			fa_score_put_clipped(COLOUR_WHITE,
				angdroid_zh_tw_enabled() ? "畫面高度不足，按任意鍵返回。" :
				"Screen too short; press any key to return.", 0, 0);
			(void)inkey();
			return;
		}

		/* Clear screen */
		Term_clear();

		/* Title */
		if (k > 0) {
			if (angdroid_zh_tw_enabled())
				fa_score_put_clipped(COLOUR_WHITE,
					format("%s 名人榜（第 %d 名起）", VERSION_NAME, k + 1),
					0, 0);
			else
				fa_score_put_clipped(COLOUR_WHITE,
					format("%s Hall of Fame (from position %d)",
						VERSION_NAME, k + 1), 0, Term->wid >= 80 ? 21 : 0);
		} else {
			if (angdroid_zh_tw_enabled())
				fa_score_put_clipped(COLOUR_WHITE,
					format("%s 名人榜", VERSION_NAME), 0, 0);
			else
				fa_score_put_clipped(COLOUR_WHITE,
					format("%s Hall of Fame", VERSION_NAME),
					0, Term->wid >= 80 ? 30 : 0);
		}

		display_score_page(scores, k, count, highlight, page_entries);

		/* Keep the footer visible on shorter/narrower terminals. */
		{
			int footer_row = MIN(23, Term->hgt - 1);
			int footer_col = Term->wid >= 80 ?
				(allow_scrolling ? 6 : 9) : 0;
			if (allow_scrolling)
				fa_score_put_clipped(COLOUR_WHITE,
					angdroid_zh_tw_enabled() ?
					"[ESC 離開，上方向鍵上頁，其他按鍵下頁]" :
					"[Press ESC to exit, up for prior page, any other key for next page.]",
					footer_row, footer_col);
			else
				fa_score_put_clipped(COLOUR_WHITE,
					angdroid_zh_tw_enabled() ?
					"[ESC 離開，其他按鍵翻至下一頁]" :
					"[Press ESC to exit, any other key to page forward till done.]",
					footer_row, footer_col);
			ch = inkey();
			if (Term && footer_row >= 0 && footer_row < Term->hgt)
				prt("", footer_row, 0);
		}

		if (ch.code == ESCAPE) {
			break;
		} else if (ch.code == ARROW_UP && allow_scrolling) {
			if (k == 0) {
				k = count > 0 ?
					((count - 1) / page_entries) * page_entries : 0;
			} else if (k < page_entries) {
				k = 0;
			} else {
				k -= page_entries;
			}
		} else {
			k += page_entries;
			if (k >= count) {
				if (allow_scrolling) {
					k = 0;
				} else {
					break;
				}
			}
		}
	}

	return;
}

/**
 * Predict the players location, and display it.
 */
void predict_score(bool allow_scrolling)
{
	int j;
	struct high_score the_score;
	struct high_score scores[MAX_HISCORES];


	/* Read scores, place current score */
	highscore_read(scores, N_ELEMENTS(scores));
	build_score(&the_score, player, "nobody (yet!)", NULL);

	if (player->is_dead)
		j = highscore_where(&the_score, scores, N_ELEMENTS(scores));
	else
		j = highscore_add(&the_score, scores, N_ELEMENTS(scores));

	/* Top fifteen scores if on the top ten, otherwise ten surrounding */
	if (j < 10) {
		display_scores_aux(scores, 0, 15, j, allow_scrolling);
	} else {
		display_scores_aux(scores, j - 2, j + 7, j, allow_scrolling);
	}
}


/**
 * Show scores.
 */
void show_scores(void)
{
	screen_save();

	/* Display the scores */
	if (character_generated) {
		predict_score(true);
	} else {
		/* Currently unused, but leaving in in case we re-implement looking
		 * at the scores without loading a character */
		struct high_score scores[MAX_HISCORES];
		highscore_read(scores, N_ELEMENTS(scores));
		display_scores_aux(scores, 0, MAX_HISCORES, -1, true);
	}

	screen_load();

	/* Hack - Flush it */
	Term_fresh();
}

