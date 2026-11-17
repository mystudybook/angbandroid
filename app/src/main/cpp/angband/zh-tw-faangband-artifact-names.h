/* Additional FAangband-only artifact display names from gamedata/artifact.txt.
 * Original game records, flags, save data and English names stay unchanged.
 * This catalog only contains the names absent from standard Angband.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES_H
#define ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES 41

static inline const char *angdroid_zh_tw_faangband_artifact_name(
    const char *english)
{
    static const struct { const char *english, *traditional; }
        names[ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES] = {
        { "of Gwindor", "格溫多之物" },
        { "'Nimphelos'", "「寧菲洛斯」" },
        { "of Avallónë", "阿瓦隆尼之物" },
        { "of Idril", "伊綴爾之物" },
        { "of Egalmoth", "埃加爾莫斯之物" },
        { "'Narsil'", "「納西爾」" },
        { "of Manwë", "曼威之物" },
        { "of Rog", "羅格之物" },
        { "of Boldog", "博爾多格之物" },
        { "of the Marches", "邊境之物" },
        { "'Dramborleg'", "「德蘭博爾勒格」" },
        { "of Bregor", "布瑞戈爾之物" },
        { "of Hithlum", "希斯隆之物" },
        { "of Mablung", "馬布隆之物" },
        { "of Vinyamar", "維尼亞瑪之物" },
        { "of Ecthelion", "埃克塞里昂之物" },
        { "of Thuringwethil", "圖林微希爾之物" },
        { "of Nargothrond", "納國斯隆德之物" },
        { "of the Hidden Kingdom", "隱秘王國之物" },
        { "(Dwarven) of the Mormegil", "矮人鍛製的墨米吉爾之物" },
        { "of Nevrast", "內芙拉斯特之物" },
        { "of Valinor", "維林諾之物" },
        { "of Glorfindel", "葛羅芬戴爾之物" },
        { "of Maeglin", "梅格林之物" },
        { "of the Elements", "元素之物" },
        { "of Khazad-dum", "卡扎督姆之物" },
        { "of Haleth", "哈烈絲之物" },
        { "of the Swan", "天鵝之物" },
        { "'Ilkorin'", "「伊爾科林」" },
        { "of Beguiling", "魅惑之物" },
        { "of Unmaking", "湮滅之物" },
        { "of Ossë", "歐希之物" },
        { "of Starfire", "星焰之物" },
        { "of Winds", "疾風之物" },
        { "of Holding", "禁錮之物" },
        { "of the Kelvar", "萬獸之物" },
        { "of Delving", "掘地之物" },
        { "'Mantle of Shadow'", "「暗影披風」" },
        { "of Air", "長空之物" },
        { "of Portals", "傳送門之物" },
        { "of Ungoliant", "昂哥立安之物" },
    };
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES; ++i)
        if (strcmp(english, names[i].english) == 0)
            return names[i].traditional;
    return NULL;
}

#endif /* ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_NAMES_H */
