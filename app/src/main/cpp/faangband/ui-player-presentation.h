/* Presentation-only FAangband character sheet stat captions and units.
 * Pure lookup tables keep the rendering stable across English/zh-TW modes. */
#ifndef ANGDROID_FA_PLAYER_PRESENTATION_H
#define ANGDROID_FA_PLAYER_PRESENTATION_H

enum angdroid_fa_sheet_format {
    ANGDROID_FA_SHEET_BLOWS = 0,
    ANGDROID_FA_SHEET_SHOTS,
    ANGDROID_FA_SHEET_BURDEN,
    ANGDROID_FA_SHEET_OVERWEIGHT,
    ANGDROID_FA_SHEET_INFRA
};

static inline const char *angdroid_fa_stat_heading(int index, int zh_tw)
{
    static const char * const en[] = {
        "  Self", " RB", " CB", " EB", "  Best"
    };
    static const char * const zh[] = {
        "  本值", "種族", "職業", "裝備", "  最佳"
    };
    if (index < 0 || index >= 5) return "";
    return zh_tw ? zh[index] : en[index];
}

static inline const char *angdroid_fa_sheet_unit_format(
    enum angdroid_fa_sheet_format category, int zh_tw)
{
    static const char * const en[] = {
        "%d.%d/turn", "%d.%d/turn", "%.1f lb",
        "%d.%d lb", "%d ft"
    };
    static const char * const zh[] = {
        "%d.%d/回合", "%d.%d/回合", "%.1f 磅",
        "%d.%d 磅", "%d 呎"
    };
    if (category < ANGDROID_FA_SHEET_BLOWS ||
        category > ANGDROID_FA_SHEET_INFRA) return "";
    return zh_tw ? zh[category] : en[category];
}

#endif /* ANGDROID_FA_PLAYER_PRESENTATION_H */
