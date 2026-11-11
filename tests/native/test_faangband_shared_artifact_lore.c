/* C99 regression for all 38 canonical FA/Ang shared artifact story variants. */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-artifact-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-faangband-artifact-variants.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

int main(void)
{
    static const struct artifact_source_pair { const char *name, *fa, *standard; } cases[] = {
        { "'Angrist'",
          "Forged from meteoric iron by Telchar, greatest of Dwarven smiths, and used by Beren to gouge a Silmaril out of Morgoth's crown, this long chopping dagger slices through ordinary metal as easily as its name, \"Iron Cleaver\", suggests.",
          "Forged by Telchar, greatest of Dwarven smiths, and used by Beren to gouge a Silmaril out of Morgoth's crown, this long chopping dagger slices through ordinary metal as easily as its name, \"Iron Cleaver\", suggests." },
        { "'Narthanc'",
          "A fiery dagger finely balanced for deadly throws.",
          "Instead of a blade, a flame seems to emerge from this dagger's hilt." },
        { "'Nimthanc'",
          "A finely-balanced dagger that numbs with deadly cold.",
          "From the hilt of this dagger springs not so much a blade as an icicle." },
        { "'Dethanc'",
          "A dagger covered in sparks and finely balanced for deadly throws.",
          "In this dagger is bound the very force of lightning." },
        { "'Anguirel'",
          "Forged of black galvorn by the Dark-Elven smith Eöl, this blade has the living lightning trapped inside.",
          "This black blade was forged of meteoric iron by Eöl the Dark Elf.  It is imbued with the malice of its maker; as dangerous to its wielder as it is deadly to his foes." },
        { "'Glamdring'",
          "The fiery, shining blade of Turgon.  Mate to Orcrist, it earned its sobriquet \"Beater\" from dying orcs who dared behold hidden Gondolin.",
          "This fiery, shining blade, mate to Orcrist, earned its sobriquet \"Beater\" from dying orcs who dared to behold hidden Gondolin." },
        { "'Gurthang'",
          "Runes dark and deadly stand stark against the naked blade of Túrin's awesome black weapon, and you feel a wrathful power of slaying and rending as you slowly approach.",
          "A giant sword once wielded by mighty Túrin, and an even greater dragonbane; for what worm could stand against the blade that bathed in Glaurung's blood?" },
        { "'Firestar'",
          "A famed battle-lord of old with a ruddy head, colored as embers are that can yet rise up in wrath.",
          "A famed battle-lord of old with a head as ruddy as embers that can yet rise up in wrath." },
        { "'Thunderfist'",
          "The long-lost weapon of Kzurin, Dwarven champion of ancient Belegost.  Runes of strength adorn its handle, and flames and sparks roar and crackle around its massive head.",
          "The long-lost weapon of Kzurin, Dwarven champion of ancient Belegost Runes of strength adorn its handle, and flames and sparks roar and crackle around its massive head." },
        { "'Taratol'",
          "A great ridged mace that surrounds you in a nimbus of living lightning; you remain utterly untouched, even as fat sparks crackle around your fingers and eyebrows.",
          "A great ridged mace that surrounds you with a nimbus of living lightning.  You remain utterly untouched, even as fat sparks crackle around your fingers and eyebrows." },
        { "'Eriril'",
          "The radiant golden staff of a Maia of legend, this wizard's companion grants keen sight and clear knowledge of many hidden things.",
          "The radiant golden staff of an Istar of legend, this wizard's companion grants keen sight and clear knowledge of many hidden things." },
        { "of Olórin",
          "A staff tall and sturdy, borne of old by Olórin, wisest of the Maiar. Rough-hewn runes that invoke the element of Earth stand bold upon its gnarled surface, and no creature who lives in the shadow of mountains can withstand the blows it deals.",
          "A staff tall and sturdy, borne of old by Gandalf.  Rough-hewn runes that invoke the element of Earth stand bold upon its gnarled surface, and no creature who lives in the shadow of mountains can withstand the blows it deals." },
        { "'Turmil'",
          "Wielded by the High Priest of Tirion, this great mace gleams coldly as though moonlit, and it can strike as mighty a blow spiritually as physically.",
          "Once wielded by the High Priest of Meneltarma, this great mace gleams coldly as though moonlit, and can strike equally mighty spiritual and physical blows." },
        { "'Aiglos'",
          "The mighty spear of Gil-galad, famed as \"Snow-point\" in the songs of Elves.",
          "The mighty spear of Gil-galad, famed as \"Snow-point\" in the songs of Elves.  Against it, all the foul corruptions of Sauron dashed in vain." },
        { "of Oromë",
          "The spear of wise Oromë the Vala, strong against giants of frost, and able to melt rock or flesh with ease.",
          "The thrusting spear of wise Oromë the Vala, strong against giants of frost, and able to pierce rock or flesh with ease." },
        { "of Wrath",
          "A massive triple-pronged spear, so great it normally requires two hands to wield, evoking the spirit of Ossë who with it pierced legions of evil and undead.",
          "A massive triple-pronged spear, so great it normally requires two hands to wield, evoking the spirit of Osse who with it pierced legions of evil and undead." },
        { "of Ulmo",
          "The fearsome weapon of the Vala Ulmo, Lord of Waters.  Mightiest of all the powers of good save Manwë himself, Ulmo laughs to scorn the dread powers of the undead, and is utterly in command of the Element of water.",
          "The awesome weapon of the Vala Ulmo, Lord of Waters.  Mightiest of all the powers of good save Manwë himself, Ulmo laughs to scorn the dread powers of the undead, and is utterly in command of the Element of water." },
        { "'Til-i-arc'",
          "Within this long two-handed spear dwell the spirits of frost giants and fire demons, who war forever, fettered in sorcerous binds.",
          "Within this long two-handed spear dwell the spirits of frost giants and fire demons, who war forever, fettered by sorcerous binds." },
        { "'Lotharang'",
          "A superbly crafted double-bladed axe once borne by a commander of the Rangers of Eriador.",
          "A superbly crafted double-bladed axe once borne by Ceruthain, commander of the Rangers of Arnor." },
        { "of Eönwë",
          "The axe of Eönwë, herald of Manwë Súlimo, strikes with the pure icy wrath of Taniquetil at the undead, disperses hosts of evil at a word, and grants Maia-like powers of body and mind.",
          "The axe of Eönwë, leader of the Hosts of the West in the War of Wrath, strikes with icy wrath at the undead, disperses hosts of evil at a word, and grants Maia-like powers of body and mind." },
        { "'Avavir'",
          "Binding elemental powers whose struggles turn this weapon red and purest white, this shining reaper bears within it a power of going forth and returning.",
          "Holding elemental powers whose struggles turn this weapon red and purest white, this shining reaper bears within it a power of going forth and returning." },
        { "of Eöl",
          "A small throwing javelin with a poisoned tip, crafted by the master of trickery in his darkened woods.",
          "The iron-shod gauntlets of the Dark Elven smith Eöl, who imbued them with magics he could channel in battle." },
        { "of Amrod",
          "A hunter's bow used for sport and war across the plains of East Beleriand.  It remembers the fury of the sons of Fëanor.",
          "One of a pair of hunting bows once belonging to Fëanor's two youngest sons.  The bows are like and yet unlike, much as the twin brothers were." },
        { "of Celegorm",
          "A cunning bow, crafted by the Noldor of old.  It remembers the wiles of the sons of Fëanor.",
          "This shield emblazoned with a multitude of creatures unseen for ages once protected Celegorm, Lord of Himlad; around it, a mystic balance lies, containing the conflicts of the elements." },
        { "'Belthronding'",
          "The great bow of Beleg Cúthalion, made of black yew and strung with elven hair that faintly shimmers a pale clear gold.",
          "The great bow of Beleg Cúthalion, the most famous archer of the Elves.  Its backbone of black yew holds strings of bear sinew, and of old, only Beleg himself could draw them." },
        { "'Dal-i-thalion'",
          "A pair of high-laced shoes, strong against the powers of corruption and withering, that grant the wearer extraordinary agility.",
          "A pair of high-laced shoes, bestowing extraordinary agility and proof against the powers of corruption and withering." },
        { "'Holhenneth'",
          "A famous cap of forged iron granting extraordinary powers of mind and awareness.",
          "A famous forged iron helm granting extraordinary powers of mind and awareness." },
        { "of Dor-Lómin",
          "The legendary dragon helm of Hador, crafted by Telchar of Nogrod.  It's dwarven visor allowed Túrin Turambar alone to face the fire of Glaurung, an image of whose head was set on its crest in defiance.",
          "The legendary Dragon-helm, forged by one of the legendary Dwarven smiths of the First Age.  The visage of mighty Glaurung forms its crest, and the mere sight of the helm, borne by Túrin Turambar, struck dread into the hearts of the servants of Morgoth." },
        { "of Celeborn",
          "A shimmering suit of true-silver, forged by dwarven smiths for Celeborn of the Teleri.  It gleams with purest white as you gaze upon it, and mighty are its powers to protect and banish.",
          "A shimmering suit of true-silver, forged long ago by dwarven smiths of legend.  It gleams with purest white as you gaze upon it, and mighty are its powers to protect and banish." },
        { "of Thingol",
          "A cloak of translucent, pearly grey, with glowing elven-runes to restore magic showing calm and clear as moonlight on still water.",
          "A translucent cloak of pearly grey with glowing, energized Elven runes showing calm and clear as moonlight on still water." },
        { "'Colannon'",
          "A crystal-blue cape of fine silk worn by a silent messenger of the forces of Law.",
          "A crystal-blue cape of fine silk worn by a silent messenger of the forces of Law.  Seldom will its wearer be diverted from his mission." },
        { "'Cammithrim'",
          "These gloves glow so brightly as to light the way for the wearer.",
          "These gloves glow so brightly as to light the way for the wearer and cast magical bolts with great frequency." },
        { "'Paurnimmen'",
          "A set of handgear which freezes your opponents.",
          "A set of handgear, freezing with unnatural cold." },
        { "'Pauraegen'",
          "A set of handgear with sparks that dance upon its knuckleguards.",
          "A set of handgear with sparks that crackle across its knuckleguards." },
        { "'Paurnen'",
          "A set of handgear able to infuse your attacks with acid.",
          "A set of gauntlets that give off a foul, acrid odour yet remain untarnished." },
        { "of Gil-galad",
          "The shield of Ereinion Gil-Galad son of Fingon, made to keep him from harm in times of trouble.",
          "The legendary shield of Ereinion Gil-galad, who fought his way to the gates of the Dark Tower, and with whom came light even to Gorgoroth." },
        { "of the Dwarves",
          "The Nauglamír; the carcanet of gold set with a multitude of shining gems of Valinor is but a setting.  The true power lies in the Silmaril of Fëanor at its centre, shining forth the light of the Two Trees of Valinor.  Its beauty and desirability are equalled only by its revealing power.",
          "The Nauglamír, a carcanet of gold set with a multitude of shining gems of Valinor.  The radiant Silmaril of Fëanor hangs in its midst as its crowning glory.  The sturdy spirits of Dwarvish craftsmen who labored long in mountain smithies lie within it still, and as gossamer it rests upon the bearer." },
        { "'Elessar'",
          "Crafted by Enerdhil of Gondolin, a jewel as green as leaves within which the clear light of the sun is imprisoned.  The hands of one who holds it bring to all that they touch healing from hurt.",
          "A green stone, imbued with the power of Elvendom and the light of the Sun.  Those who gaze through it see the aged and infirm as young again, and its wearer brings healing after victory in battle." },
    };
    size_t i;
    bool allow;
    const char *result;
    CHECK(ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_VARIANTS == 38);
    CHECK(sizeof(cases) / sizeof(cases[0]) == 38);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const struct artifact_source_pair *item = &cases[i];
        char changed[1024];
        const char *fa_story = item->fa;
        allow = true;
        result = angdroid_zh_tw_faangband_variant_artifact_lore(
            item->name, fa_story, &allow);
        CHECK(result != NULL);
        CHECK(strlen(result) > 12);
        CHECK(allow == false);
        CHECK(strcmp(result, fa_story) != 0);

        allow = false;
        result = angdroid_zh_tw_faangband_variant_artifact_lore(
            item->name, item->standard, &allow);
        CHECK(result == NULL);
        CHECK(allow == true);
        CHECK(angdroid_zh_tw_artifact_flavor(item->name) != NULL);

        /* Reject unknown or changed source instead of reusing standard lore. */
        CHECK(strlen(fa_story) + 2 < sizeof(changed));
        snprintf(changed, sizeof(changed), "%s", fa_story);
        changed[0] = changed[0] == 'X' ? 'Y' : 'X';
        allow = true;
        result = angdroid_zh_tw_faangband_variant_artifact_lore(
            item->name, changed, &allow);
        CHECK(result == NULL);
        CHECK(allow == false);

        /* A different name must never be matched to this artifact text. */
        allow = false;
        result = angdroid_zh_tw_faangband_variant_artifact_lore(
            "not a shared artifact", fa_story, &allow);
        CHECK(result == NULL);
        CHECK(allow == true);
    }
    /* Brand-new FAangband-only names use the other catalog as before. */
    allow = false;
    CHECK(angdroid_zh_tw_faangband_variant_artifact_lore(
        "of Gwindor", "Some story", &allow) == NULL && allow);
    allow = false;
    CHECK(angdroid_zh_tw_faangband_variant_artifact_lore(
        "'Sting'", "Some story", &allow) == NULL && allow);
    allow = true;
    CHECK(angdroid_zh_tw_faangband_variant_artifact_lore(
        NULL, "Some story", &allow) == NULL && !allow);
    allow = true;
    CHECK(angdroid_zh_tw_faangband_variant_artifact_lore(
        "'Angrist'", NULL, &allow) == NULL && !allow);
    CHECK(angdroid_zh_tw_faangband_variant_artifact_lore(
        NULL, NULL, NULL) == NULL);
    CHECK(strstr(angdroid_zh_tw_faangband_variant_artifact_lore(
        "'Anguirel'", cases[4].fa, &allow), "雷電") != NULL);
    CHECK(strstr(angdroid_zh_tw_faangband_variant_artifact_lore(
        "'Gurthang'", cases[6].fa, &allow), "符文") != NULL);
    CHECK(strstr(angdroid_zh_tw_faangband_variant_artifact_lore(
        "of Eöl", cases[21].fa, &allow), "投擲標槍") != NULL);
    printf("PASS: 38 exact FA/Ang variant artifacts (%lu assertions)\n", checks);
    return 0;
}
