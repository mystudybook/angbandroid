/**
 * \file obj-desc.c
 * \brief Create object name descriptions
 *
 * Copyright (c) 1997 - 2007 Angband contributors
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
#include "obj-chest.h"
#include "obj-desc.h"
#include "obj-gear.h"
#include "obj-ignore.h"
#include "obj-knowledge.h"
#include "obj-tval.h"
#include "obj-util.h"
#include "droid.h"
#include "zh-tw-items.h"
#include "zh-tw-ego-names.h"
#include "zh-tw-artifact-names.h"
#include "zh-tw-flavor-names.h"
#include "zh-tw-scroll-title.h"
#include "zh-tw-flavored-object.h"
#include "zh-tw-item-grammar.h"
#include "zh-tw-item-details.h"

/**
 * Puts the object base kind's name into buf.
 */
void object_base_name(char *buf, size_t max, int tval, bool plural)
{
	struct object_base *kb = &kb_info[tval];
	size_t end = 0;

	if (kb->name && kb->name[0]) 
		(void) obj_desc_name_format(buf, max, end, kb->name, NULL, plural);
}


/**
 * Puts a very stripped-down version of an object's name into buf.
 * If easy_know is true, then the IDed names are used, otherwise
 * flavours, scroll names, etc will be used.
 *
 * Just truncates if the buffer isn't big enough.
 */
void object_kind_name(char *buf, size_t max, const struct object_kind *kind,
					  bool easy_know)
{
	/* If not aware, the plain flavour (e.g. Copper) will do. */
	if (!easy_know && !kind->aware && kind->flavor) {
		/* Unknown item kinds remain unknown; only the appearance changes. */
		const char *flavor = kind->flavor->text;
		const char *localized = angdroid_zh_tw_enabled()
			? angdroid_zh_tw_flavor_name(flavor) : NULL;
		/* Generated scroll titles are not source dictionary entries.
		 * Only wrap the ORIGINAL text in Chinese quotation marks. */
		if (kind->tval == TV_SCROLL && angdroid_zh_tw_enabled() &&
				angdroid_zh_tw_scroll_title(buf, max, flavor, false))
			return;
		my_strcpy(buf, localized ? localized : flavor, max);
	}

	/* Use the localized DISPLAY name only for a known kind. Keep
	 * object_desc(), unidentified flavors and the canonical grammar
	 * template untouched for gameplay and save compatibility. */
	else {
		const char *translated = angdroid_zh_tw_enabled() ?
			angdroid_zh_tw_item_kind_name(kind->name) : NULL;
		if (translated)
			my_strcpy(buf, translated, max);
		else
			(void) obj_desc_name_format(buf, max, 0, kind->name, NULL, false);
	}
}


/**
 * A modifier string, put where '#' goes in the basename below.  The weird
 * games played with book names are to allow the non-essential part of the
 * name to be abbreviated when there is not much room to display.
 */
static const char *obj_desc_get_modstr(const struct object_kind *kind)
{
	if (tval_can_have_flavor_k(kind))
		return kind->flavor ? kind->flavor->text : "";

	if (tval_is_book_k(kind))
		return kind->name;

	return "";
}

/**
 * An object's basic name - a generic name for flavored objects (with the
 * actual name added later depending on awareness, the name from object.txt
 * for almost everything else, and a bit extra for books. 
 */
static const char *obj_desc_get_basename(const struct object *obj, bool aware,
		bool terse, uint32_t mode, const struct player *p)
{
	bool show_flavor = !terse && obj->kind->flavor;

	if (mode & ODESC_STORE)
		show_flavor = false;
	if (aware && p && !OPT(p, show_flavors)) show_flavor = false;

	/* Artifacts are special */
	if (obj->artifact && (aware || object_is_known_artifact(obj) || terse ||
						  !obj->kind->flavor))
		return obj->kind->name;

	/* Analyze the object */
	switch (obj->tval)
	{
		case TV_FLASK:
		case TV_CHEST:
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_LIGHT:
		case TV_FOOD:
			return obj->kind->name;

		case TV_AMULET:
			return (show_flavor ? "& # Amulet~" : "& Amulet~");

		case TV_RING:
			return (show_flavor ? "& # Ring~" : "& Ring~");

		case TV_STAFF:
			return (show_flavor ? "& # Sta|ff|ves|" : "& Sta|ff|ves|");

		case TV_WAND:
			return (show_flavor ? "& # Wand~" : "& Wand~");

		case TV_ROD:
			return (show_flavor ? "& # Rod~" : "& Rod~");

		case TV_POTION:
			return (show_flavor ? "& # Potion~" : "& Potion~");

		case TV_SCROLL:
			return (show_flavor ? "& Scroll~ titled #" : "& Scroll~");

		case TV_MAGIC_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Book~ of Magic Spells #";

		case TV_PRAYER_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Holy Book~ of Prayers #";

		case TV_NATURE_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Book~ of Nature Magics #";

		case TV_SHADOW_BOOK:
			if (terse)
				return "& Tome~ #";
			else
				return "& Necromantic Tome~ #";

		case TV_OTHER_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Book of Mysteries~ #";

		case TV_MUSHROOM:
			return (show_flavor ? "& # Mushroom~" : "& Mushroom~");
	}

	return "(nothing)";
}


/**
 * Start to description, indicating number/uniqueness (a, the, no more, 7, etc)
 */
static size_t obj_desc_name_prefix(char *buf, size_t max, size_t end,
		const struct object *obj, const char *basename,
		const char *modstr, bool terse, uint16_t number)
{
	if (number == 0) {
		strnfcat(buf, max, &end, "no more ");
	} else if (number > 1) {
		strnfcat(buf, max, &end, "%u ", number);
	} else if (object_is_known_artifact(obj)) {
		strnfcat(buf, max, &end, "the ");
	} else if (*basename == '&') {
		bool an = false;
		const char *lookahead = basename + 1;

		while (*lookahead == ' ') lookahead++;

		if (*lookahead == '#') {
			if (modstr && is_a_vowel(*modstr))
				an = true;
		} else if (is_a_vowel(*lookahead)) {
			an = true;
		}

		if (!terse) {
			if (an)
				strnfcat(buf, max, &end, "an ");
			else
				strnfcat(buf, max, &end, "a ");			
		}
	}

	return end;
}



/**
 * Formats 'fmt' into 'buf', with the following formatting characters:
 *
 * '~' at the end of a word (e.g. "fridge~") will pluralise
 *
 * '|x|y|' will be output as 'x' if singular or 'y' if plural
 *    (e.g. "kni|fe|ves|")
 *
 * '#' will be replaced with 'modstr' (which may contain the pluralising
 * formats given above).
 */
size_t obj_desc_name_format(char *buf, size_t max, size_t end,
		const char *fmt, const char *modstr, bool pluralise)
{
	/* Copy the string */
	while (*fmt) {
		/* Skip */
		if (*fmt == '&') {
			while (*fmt == ' ' || *fmt == '&')
				fmt++;
			continue;
		} else if (*fmt == '~') {
			/* Pluralizer (regular English plurals) */
			char prev = *(fmt - 1);

			if (!pluralise)	{
				fmt++;
				continue;
			}

			/* e.g. cutlass-e-s, torch-e-s, box-e-s */
			if (prev == 's' || prev == 'h' || prev == 'x')
				strnfcat(buf, max, &end, "es");
			else
				strnfcat(buf, max, &end, "s");
		} else if (*fmt == '|') {
			/* Special plurals 
			* e.g. kni|fe|ves|
			*          ^  ^  ^ */
			const char *singular = fmt + 1;
			const char *plural   = strchr(singular, '|');
			const char *endmark  = NULL;

			if (plural) {
				plural++;
				endmark = strchr(plural, '|');
			}

			if (!singular || !plural || !endmark) return end;

			if (!pluralise)
				strnfcat(buf, max, &end, "%.*s",
					(int) (plural - singular) - 1,
					singular);
			else
				strnfcat(buf, max, &end, "%.*s",
					(int) (endmark - plural), plural);

			fmt = endmark;
		} else if (*fmt == '#') {
			/* Add modstr, with pluralisation if relevant */
			end = obj_desc_name_format(buf, max, end, modstr, NULL,	pluralise);
		}

		else
			buf[end++] = *fmt;

		fmt++;
	}

	buf[end] = 0;

	return end;
}


/**
 * Format object obj's name into 'buf'.
 */
static size_t obj_desc_name(char *buf, size_t max, size_t end,
		const struct object *obj, bool prefix, uint32_t mode,
		bool terse, const struct player *p)
{
	bool store = mode & ODESC_STORE ? true : false;
	bool spoil = mode & ODESC_SPOIL ? true : false;
	uint16_t number = (mode & ODESC_ALTNUM) ?
		(mode & 0xFFFF0000) >> 16 : obj->number;
	
	/* Actual name for flavoured objects if aware, or in store, or spoiled */
	bool aware = object_flavor_is_aware(obj) || store || spoil;
	/* Pluralize if (not forced singular) and
	 * (not a known/visible artifact) and
	 * (not one in stack or forced plural) */
	bool plural = !(mode & ODESC_SINGULAR) &&
		!obj->artifact &&
		(number != 1 || (mode & ODESC_PLURAL));
	const char *basename = obj_desc_get_basename(obj, aware, terse,
		mode, p);
	const char *modstr = obj_desc_get_modstr(obj->kind);
	const char *localized_kind = NULL;
	const char *localized_flavor = NULL;
	char scroll_display[96];
	char flavored_display[256];
	bool localized_scroll = false;
	bool localized_flavored = false;
	bool show_flavor = !terse && obj->kind->flavor;

	/* UI-only display translation. Never expose an unidentified kind,
	 * change the canonical grammar, or affect save/character exports. */
	if (store || (aware && p && !OPT(p, show_flavors)))
		show_flavor = false;
	/* Do not reveal the kind: translate only the existing visible flavor. */
	if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled() &&
			show_flavor && obj->kind->flavor) {
		localized_flavor =
			angdroid_zh_tw_flavor_name(obj->kind->flavor->text);
		if (localized_flavor) modstr = localized_flavor;
	}
	if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled() && aware &&
			!show_flavor && obj->kind->name)
		localized_kind = angdroid_zh_tw_item_kind_name(obj->kind->name);

	/* The title is random Latin-like text, not a translatable word. Do
	 * not regenerate it, change the flavor pointer, or disclose the kind.
	 * Keep special ego/artifact objects on the original formatting path. */
	if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled() &&
			obj->tval == TV_SCROLL && show_flavor &&
			!obj->artifact && !obj->ego && obj->kind->flavor)
		localized_scroll = angdroid_zh_tw_scroll_title(
			scroll_display, sizeof(scroll_display),
			obj->kind->flavor->text, true);


	/* Only names with a translated visible appearance can use the
	 * Chinese noun grammar. Unknown effects stay hidden until aware. */
	if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled() &&
			show_flavor && localized_flavor && !obj->artifact &&
			!obj->ego && obj->tval != TV_SCROLL) {
		const char *visible_effect = NULL;
		if (aware) {
			visible_effect = angdroid_zh_tw_item_kind_name(
				obj->kind->name);
		}
		if (!aware || visible_effect)
			localized_flavored = angdroid_zh_tw_flavored_object(
				flavored_display, sizeof(flavored_display),
				tval_find_name(obj->tval), localized_flavor,
				visible_effect, prefix, number);
	}
	if (localized_scroll) {
		if (prefix && number == 0)
			strnfcat(buf, max, &end, "沒有剩餘的");
		else if (prefix && number > 1)
			strnfcat(buf, max, &end, "%u 張", number);
		strnfcat(buf, max, &end, "%s", scroll_display);
	} else if (localized_flavored) {
		strnfcat(buf, max, &end, "%s", flavored_display);
	} else if (localized_kind) {
		char known_display[256];
		char named_effect[160];
		const char *display_kind = localized_kind;
		/* An identified flavored kind uses the effect as its source name.
		 * Complete it with the Chinese noun, but only after the existing
		 * aware/show_flavor checks have selected localized_kind. */
		if (obj->kind->flavor &&
				angdroid_zh_tw_identified_flavor_name(named_effect,
					sizeof(named_effect), tval_find_name(obj->tval),
					localized_kind))
			display_kind = named_effect;
		/* Only the user-facing Chinese text uses type-specific classifiers.
		 * Copy whole UTF-8 codepoints on narrow terminals. */
		if (angdroid_zh_tw_known_item_name(known_display,
				sizeof(known_display), tval_find_name(obj->tval),
				display_kind, prefix, number)) {
			angdroid_zh_tw_item_append_utf8(buf, max, &end,
				known_display);
		} else {
			/* Safe backward fallback for unexpectedly long source labels. */
			if (prefix && number == 0)
				strnfcat(buf, max, &end, "沒有剩餘的");
			else if (prefix && number > 1)
				strnfcat(buf, max, &end, "%u 件", number);
			strnfcat(buf, max, &end, "%s", localized_kind);
		}
	} else {
		/* Preserve English pluralization and unidentified flavor text. */
		if (prefix)
			end = obj_desc_name_prefix(buf, max, end, obj, basename,
				modstr, terse, number);
		end = obj_desc_name_format(buf, max, end, basename, modstr, plural);
	}

	/* Append extra names of various kinds */
	if (object_is_known_artifact(obj)) {
		/* Only known artifacts in explicitly translated UI descriptions.
		 * Canonical artifact names and unflagged exports stay untouched. */
		const char *artifact_name = obj->artifact->name;
		const char *translated_suffix = NULL;
		bool chinese_suffix_done = false;
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()) {
			const char *localized = angdroid_zh_tw_artifact_name(artifact_name);
			if (localized) artifact_name = localized;
			translated_suffix = localized;
		}
		if (localized_kind && translated_suffix) {
			char suffix_display[192];
			if (angdroid_zh_tw_item_suffix(suffix_display,
					sizeof(suffix_display), translated_suffix)) {
				angdroid_zh_tw_item_append_utf8(buf, max, &end,
					suffix_display);
				chinese_suffix_done = true;
			}
		}
		if (!chinese_suffix_done)
			strnfcat(buf, max, &end, " %s", artifact_name);
	}
	else if ((obj->known->ego && !(mode & ODESC_NOEGO)) || (obj->ego && store)) {
		/* Only translate a revealed ego suffix for opted-in screen calls.
		 * Unknown ego text, English mode and unflagged exports stay intact. */
		const char *ego_name = obj->ego->name;
		const char *translated_suffix = NULL;
		bool chinese_suffix_done = false;
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()) {
			const char *localized = angdroid_zh_tw_ego_name(ego_name);
			if (localized) ego_name = localized;
			translated_suffix = localized;
		}
		if (localized_kind && translated_suffix) {
			char suffix_display[192];
			if (angdroid_zh_tw_item_suffix(suffix_display,
					sizeof(suffix_display), translated_suffix)) {
				angdroid_zh_tw_item_append_utf8(buf, max, &end,
					suffix_display);
				chinese_suffix_done = true;
			}
		}
		if (!chinese_suffix_done)
			strnfcat(buf, max, &end, " %s", ego_name);
	}
	else if (localized_scroll && aware && !obj->artifact) {
		/* Only an identified scroll may reveal its actual effect. */
		const char *known = angdroid_zh_tw_item_kind_name(obj->kind->name);
		strnfcat(buf, max, &end, "（%s）",
			known ? known : obj->kind->name);
	}
	else if (!localized_kind && aware && !obj->artifact &&
			 !localized_flavored &&
			 (obj->kind->flavor || obj->kind->tval == TV_SCROLL)) {
		if (terse)
			strnfcat(buf, max, &end, " '%s'", obj->kind->name);
		else
			strnfcat(buf, max, &end, " of %s", obj->kind->name);
	}

	return end;
}

/* Only called in the opted-in Chinese screen path. Same engine values and
 * identification gates are enforced by obj_desc_combat(). */
static size_t obj_desc_combat_zh_tw(char *buf, size_t max, size_t end,
        enum angdroid_zh_tw_combat_field field, int first, int second)
{
    char detail[96];
    if (angdroid_zh_tw_item_combat_text(detail, sizeof(detail),
            field, first, second))
        angdroid_zh_tw_item_append_whole(buf, max, &end, detail);
    return end;
}

/**
 * Is obj armor?
 */
static bool obj_desc_show_armor(const struct object *obj,
		const struct player *p)
{
	return (!p || p->obj_k->ac) && (obj->ac || tval_is_armor(obj));
}

/**
 * Special descriptions for types of chest traps
 */
static size_t obj_desc_chest(const struct object *obj, char *buf, size_t max,
							 size_t end)
{
	if (!tval_is_chest(obj)) return end;

	/* The chest is unopened, but we know nothing about its trap/lock */
	if (obj->pval && !obj->known->pval) return end;

	/* Describe the traps */
	strnfcat(buf, max, &end, " (%s)", chest_trap_name(obj));

	return end;
}

/**
 * Describe combat properties of an item - damage dice, to-hit, to-dam, armor
 * class, missile multipler
 */
static size_t obj_desc_combat(const struct object *obj, char *buf, size_t max, 
		size_t end, uint32_t mode, const struct player *p)
{
	bool spoil = mode & ODESC_SPOIL ? true : false;
	int to_h, to_d, to_a;
	bool zh_tw = (mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled();

	/* Display damage dice if they are known */
	if (kf_has(obj->kind->kind_flags, KF_SHOW_DICE) &&
		(!p || (p->obj_k->dd && p->obj_k->ds))) {
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_DICE, obj->dd, obj->ds);
		else
			strnfcat(buf, max, &end, " (%dd%d)", obj->dd, obj->ds);
	}

	/* Display shooting power as part of the multiplier */
	if (kf_has(obj->kind->kind_flags, KF_SHOW_MULT)) {
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_MULTIPLIER,
				obj->pval + obj->modifiers[OBJ_MOD_MIGHT], 0);
		else
			strnfcat(buf, max, &end, " (x%d)",
				obj->pval + obj->modifiers[OBJ_MOD_MIGHT]);
	}

	/* No more if the object hasn't been assessed */
	if (!((obj->notice & OBJ_NOTICE_ASSESSED) || spoil)) return end;

	to_h = object_to_hit(obj);
	to_d = object_to_dam(obj);
	to_a = object_to_ac(obj);

	/* Show weapon bonuses if we know of any */
	if ((!p || (p->obj_k->to_h && p->obj_k->to_d))
			&& (tval_is_weapon(obj) || to_d
			|| (to_h && !tval_is_body_armor(obj))
			|| ((!object_has_standard_to_h(obj)
			|| obj->to_h != to_h)
			&& !obj->artifact && !obj->ego))) {
		/* In general show full combat bonuses */
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_ATTACK_PAIR, to_h, to_d);
		else
			strnfcat(buf, max, &end, " (%+d,%+d)", to_h, to_d);
	} else if (obj->to_h < 0 && object_has_standard_to_h(obj)) {
		/* Special treatment for body armor with only a to-hit penalty */
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_HIT, obj->to_h, 0);
		else
			strnfcat(buf, max, &end, " (%+d)", obj->to_h);
	} else if (to_d != 0 && (!p || p->obj_k->to_d)) {
		/* To-dam rune known only */
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_DAMAGE, to_d, 0);
		else
			strnfcat(buf, max, &end, " (%+d)", to_d);
	} else if (to_h != 0 && (!p || p->obj_k->to_h)) {
		/* To-hit rune known only */
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_HIT, to_h, 0);
		else
			strnfcat(buf, max, &end, " (%+d)", to_h);
	}

	/* Show armor bonuses */
	if (!p || p->obj_k->to_a) {
		if (obj_desc_show_armor(obj, p)) {
			if (zh_tw)
				end = obj_desc_combat_zh_tw(buf, max, end,
					ANGDROID_ITEM_ARMOR_PAIR, obj->ac, to_a);
			else
				strnfcat(buf, max, &end, " [%d,%+d]", obj->ac, to_a);
		} else if (to_a) {
			if (zh_tw)
				end = obj_desc_combat_zh_tw(buf, max, end,
					ANGDROID_ITEM_ARMOR_BONUS, to_a, 0);
			else
				strnfcat(buf, max, &end, " [%+d]", to_a);
		}
	} else if (obj_desc_show_armor(obj, p)) {
		if (zh_tw)
			end = obj_desc_combat_zh_tw(buf, max, end,
				ANGDROID_ITEM_ARMOR, obj->ac, 0);
		else
			strnfcat(buf, max, &end, " [%d]", obj->ac);
	}

	return end;
}

/**
 * Describe remaining light for refuellable lights
 */
static size_t obj_desc_light(const struct object *obj, char *buf, size_t max,
							 size_t end, uint32_t mode)
{
	/* Fuelled light sources get number of remaining turns appended */
	if (tval_is_light(obj) && !of_has(obj->flags, OF_NO_FUEL)) {
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
			strnfcat(buf, max, &end, "（剩餘 %d 回合）", obj->timeout);
		else
			strnfcat(buf, max, &end, " (%d turns)", obj->timeout);
	}

	return end;
}

/**
 * Describe numerical modifiers to stats and other player qualities which
 * allow numerical bonuses - speed, stealth, etc
 */
static size_t obj_desc_mods(const struct object *obj, char *buf, size_t max,
							size_t end, uint32_t mode)
{
	int i, j, num_mods = 0;
	int mods[OBJ_MOD_MAX] = { 0 };

	/* Run through possible modifiers and store distinct ones */
	for (i = 0; i < OBJ_MOD_MAX; i++) {
		/* Check for known non-zero mods */
		if (obj->modifiers[i] != 0) {
			/* If no mods stored yet, store and move on */
			if (!num_mods) {
				mods[num_mods++] = obj->modifiers[i];
				continue;
			}

			/* Run through the existing mods, quit on duplicates */
			for (j = 0; j < num_mods; j++)
				if (mods[j] == obj->modifiers[i]) break;

			/* Add another mod if needed */
			if (j == num_mods)
				mods[num_mods++] = obj->modifiers[i];
		}
	}

	if (!num_mods) return end;

	if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()) {
		char localized[192];
		if (angdroid_zh_tw_item_modifiers_text(localized,
				sizeof(localized), mods, (size_t)num_mods)) {
			angdroid_zh_tw_item_append_whole(buf, max, &end, localized);
			return end;
		}
	}

	/* Print the modifiers */
	strnfcat(buf, max, &end, " <");
	for (j = 0; j < num_mods; j++) {
		if (j) strnfcat(buf, max, &end, ", ");
		strnfcat(buf, max, &end, "%+d", mods[j]);
	}
	strnfcat(buf, max, &end, ">");

	return end;
}

/**
 * Describe charges or charging status for re-usable items with magic effects
 */
static size_t obj_desc_charges(const struct object *obj, char *buf, size_t max,
		size_t end, uint32_t mode)
{
	bool aware = object_flavor_is_aware(obj) || (mode & ODESC_STORE);

	/* Wands and staffs have charges, others may be charging */
	if (aware && tval_can_have_charges(obj)) {
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
			strnfcat(buf, max, &end, "（剩餘 %d 次充能）", obj->pval);
		else
			strnfcat(buf, max, &end, " (%d charge%s)", obj->pval,
				PLURAL(obj->pval));
	} else if (obj->timeout > 0) {
		if (tval_is_rod(obj) && obj->number > 1) {
			if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
				strnfcat(buf, max, &end, "（%d 個充能中）", number_charging(obj));
			else
				strnfcat(buf, max, &end, " (%d charging)", number_charging(obj));
		} else if (tval_is_rod(obj) || obj->activation || obj->effect) {
			if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
				strnfcat(buf, max, &end, "（充能中）");
			else
				strnfcat(buf, max, &end, " (charging)");
		}
	}

	return end;
}

/**
 * Add player-defined inscriptions or game-defined descriptions
 */
static size_t obj_desc_inscrip(const struct object *obj, char *buf,
		size_t max, size_t end, const struct player *p, uint32_t mode)
{
	const char *u[6] = { 0, 0, 0, 0, 0, 0 };
	int n = 0;

	/* Get inscription */
	if (obj->note)
		u[n++] = quark_str(obj->note);

	/* Use special inscription, if any */
	if (!object_flavor_is_aware(obj)) {
		if (tval_can_have_charges(obj) && (obj->pval == 0))
			u[n++] = "empty";
		if (object_flavor_was_tried(obj))
			u[n++] = "tried";
	}

	/* Note curses */
	if (obj->known->curses)
		u[n++] = "cursed";

	/* Note ignore */
	if (p && ignore_item_ok(p, obj))
		u[n++] = "ignore";

	/* Note unknown properties */
	if (!object_runes_known(obj) && (obj->known->notice & OBJ_NOTICE_ASSESSED))
		u[n++] = "??";

	if (n) {
		int i;
		for (i = 0; i < n; i++) {
			if (i == 0)
				strnfcat(buf, max, &end, " {");
			if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled() &&
					!(i == 0 && obj->note)) {
				const char *label = u[i];
				if (streq(label, "empty")) label = "已耗盡";
				else if (streq(label, "tried")) label = "已試用";
				else if (streq(label, "cursed")) label = "受詛咒";
				else if (streq(label, "ignore")) label = "忽略";
				strnfcat(buf, max, &end, "%s", label);
			} else {
				/* User-written inscriptions are never translated. */
				strnfcat(buf, max, &end, "%s", u[i]);
			}
			if (i < n - 1)
				strnfcat(buf, max, &end, ", ");
		}

		strnfcat(buf, max, &end, "}");
	}

	return end;
}


/**
 * Add "unseen" to the end of unaware items in stores,
 * and "??" to not fully known unflavored items 
 */
static size_t obj_desc_aware(const struct object *obj, char *buf, size_t max,
							 size_t end, uint32_t mode)
{
	if (!object_flavor_is_aware(obj)) {
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
			strnfcat(buf, max, &end, " {未見過}");
		else
			strnfcat(buf, max, &end, " {unseen}");
	} else if (!object_runes_known(obj)) {
		strnfcat(buf, max, &end, " {??}");
	} else if (obj->known->curses) {
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
			strnfcat(buf, max, &end, " {受詛咒}");
		else
			strnfcat(buf, max, &end, " {cursed}");
	}

	return end;
}


/**
 * Describes item `obj` into buffer `buf` of size `max`.
 *
 * \param buf is the buffer for the description.  Must have space for at least
 * max bytes.
 * \param max is the size of the buffer, in bytes.
 * \param obj is the object to describe.
 * \param mode must be a bitwise-or of zero or one more of the following:
 * ODESC_PREFIX prepends a 'the', 'a' or number
 * ODESC_BASE results in a base description.
 * ODESC_COMBAT will add to-hit, to-dam and AC info.
 * ODESC_EXTRA will add pval/charge/inscription/ignore info.
 * ODESC_PLURAL will pluralise regardless of the number in the stack.
 * ODESC_STORE turns off ignore markers, for in-store display.
 * ODESC_SPOIL treats the object as fully identified.
 * ODESC_CAPITAL capitalises the object name.
 * ODESC_TERSE causes a terse name to be used.
 * ODESC_NOEGO omits ego names.
 * ODESC_ALTNUM causes the high 16 bits of mode to be used as the number
 * of objects instead of using obj->number.  Note that using ODESC_ALTNUM
 * is not fully compatible with ODESC_EXTRA:  the display of number of rods
 * charging does not account for the alternate number.
 * \param p is the player whose knowledge is factored into the description.
 * If p is NULL, the description is for an omniscient observer.
 *
 * \returns The number of bytes used of the buffer.
 */
size_t object_desc(char *buf, size_t max, const struct object *obj,
		uint32_t mode, const struct player *p)
{
	bool prefix = mode & ODESC_PREFIX ? true : false;
	bool spoil = mode & ODESC_SPOIL ? true : false;
	bool terse = mode & ODESC_TERSE ? true : false;

	size_t end = 0;

	/* Simple description for null item */
	if (!obj || !obj->known) {
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
			return strnfmt(buf, max, "（空）");
		return strnfmt(buf, max, "(nothing)");
	}

	/* Unknown itema and cash get straightforward descriptions */
	if (obj->known && obj->kind != obj->known->kind) {
		if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled())
			return strnfmt(buf, max, "未知物品");
		if (prefix)
			return strnfmt(buf, max, "an unknown item");
		return strnfmt(buf, max, "unknown item");
	}

	if (tval_is_money(obj))
		return strnfmt(buf, max, "%d gold pieces worth of %s%s",
				obj->pval, obj->kind->name,
				ignore_item_ok(p, obj) ? " {ignore}" : "");

	/* Egos and kinds whose name we know are seen */
	if (obj->known->ego && !spoil)
		obj->ego->everseen = true;

	if (object_flavor_is_aware(obj) && !spoil)
		obj->kind->everseen = true;

	/** Construct the name **/

	/* Copy the base name to the buffer */
	end = obj_desc_name(buf, max, end, obj, prefix, mode, terse, p);

	/* Combat properties */
	if (mode & ODESC_COMBAT) {
		if (tval_is_chest(obj))
			end = obj_desc_chest(obj, buf, max, end);
		else if (tval_is_light(obj))
			end = obj_desc_light(obj, buf, max, end, mode);

		end = obj_desc_combat(obj->known, buf, max, end, mode, p);
	}

	/* Modifiers, charges, flavour details, inscriptions */
	if (mode & ODESC_EXTRA) {
		end = obj_desc_mods(obj->known, buf, max, end, mode);

		end = obj_desc_charges(obj, buf, max, end, mode);

		if (mode & ODESC_STORE)
			end = obj_desc_aware(obj, buf, max, end, mode);
		else
			end = obj_desc_inscrip(obj, buf, max, end, p, mode);
	}

	return end;
}
