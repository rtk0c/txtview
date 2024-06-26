#include "conversions.h"

#include <cassert>

uint8_t HbScriptCompress(hb_script_t script) {
    switch (script) {
        case HB_SCRIPT_COMMON: return 0;
        case HB_SCRIPT_INHERITED: return 1;
        case HB_SCRIPT_UNKNOWN: return 2;
        case HB_SCRIPT_ARABIC: return 3;
        case HB_SCRIPT_ARMENIAN: return 4;
        case HB_SCRIPT_BENGALI: return 5;
        case HB_SCRIPT_CYRILLIC: return 6;
        case HB_SCRIPT_DEVANAGARI: return 7;
        case HB_SCRIPT_GEORGIAN: return 8;
        case HB_SCRIPT_GREEK: return 9;
        case HB_SCRIPT_GUJARATI: return 10;
        case HB_SCRIPT_GURMUKHI: return 11;
        case HB_SCRIPT_HANGUL: return 12;
        case HB_SCRIPT_HAN: return 13;
        case HB_SCRIPT_HEBREW: return 14;
        case HB_SCRIPT_HIRAGANA: return 15;
        case HB_SCRIPT_KANNADA: return 16;
        case HB_SCRIPT_KATAKANA: return 17;
        case HB_SCRIPT_LAO: return 18;
        case HB_SCRIPT_LATIN: return 19;
        case HB_SCRIPT_MALAYALAM: return 20;
        case HB_SCRIPT_ORIYA: return 21;
        case HB_SCRIPT_TAMIL: return 22;
        case HB_SCRIPT_TELUGU: return 23;
        case HB_SCRIPT_THAI: return 24;
        case HB_SCRIPT_TIBETAN: return 25;
        case HB_SCRIPT_BOPOMOFO: return 26;
        case HB_SCRIPT_BRAILLE: return 27;
        case HB_SCRIPT_CANADIAN_SYLLABICS: return 28;
        case HB_SCRIPT_CHEROKEE: return 29;
        case HB_SCRIPT_ETHIOPIC: return 30;
        case HB_SCRIPT_KHMER: return 31;
        case HB_SCRIPT_MONGOLIAN: return 32;
        case HB_SCRIPT_MYANMAR: return 33;
        case HB_SCRIPT_OGHAM: return 34;
        case HB_SCRIPT_RUNIC: return 35;
        case HB_SCRIPT_SINHALA: return 36;
        case HB_SCRIPT_SYRIAC: return 37;
        case HB_SCRIPT_THAANA: return 38;
        case HB_SCRIPT_YI: return 39;
        case HB_SCRIPT_DESERET: return 40;
        case HB_SCRIPT_GOTHIC: return 41;
        case HB_SCRIPT_OLD_ITALIC: return 42;
        case HB_SCRIPT_BUHID: return 43;
        case HB_SCRIPT_HANUNOO: return 44;
        case HB_SCRIPT_TAGALOG: return 45;
        case HB_SCRIPT_TAGBANWA: return 46;
        case HB_SCRIPT_CYPRIOT: return 47;
        case HB_SCRIPT_LIMBU: return 48;
        case HB_SCRIPT_LINEAR_B: return 49;
        case HB_SCRIPT_OSMANYA: return 50;
        case HB_SCRIPT_SHAVIAN: return 51;
        case HB_SCRIPT_TAI_LE: return 52;
        case HB_SCRIPT_UGARITIC: return 53;
        case HB_SCRIPT_BUGINESE: return 54;
        case HB_SCRIPT_COPTIC: return 55;
        case HB_SCRIPT_GLAGOLITIC: return 56;
        case HB_SCRIPT_KHAROSHTHI: return 57;
        case HB_SCRIPT_NEW_TAI_LUE: return 58;
        case HB_SCRIPT_OLD_PERSIAN: return 59;
        case HB_SCRIPT_SYLOTI_NAGRI: return 60;
        case HB_SCRIPT_TIFINAGH: return 61;
        case HB_SCRIPT_BALINESE: return 62;
        case HB_SCRIPT_CUNEIFORM: return 63;
        case HB_SCRIPT_NKO: return 64;
        case HB_SCRIPT_PHAGS_PA: return 65;
        case HB_SCRIPT_PHOENICIAN: return 66;
        case HB_SCRIPT_CARIAN: return 67;
        case HB_SCRIPT_CHAM: return 68;
        case HB_SCRIPT_KAYAH_LI: return 69;
        case HB_SCRIPT_LEPCHA: return 70;
        case HB_SCRIPT_LYCIAN: return 71;
        case HB_SCRIPT_LYDIAN: return 72;
        case HB_SCRIPT_OL_CHIKI: return 73;
        case HB_SCRIPT_REJANG: return 74;
        case HB_SCRIPT_SAURASHTRA: return 75;
        case HB_SCRIPT_SUNDANESE: return 76;
        case HB_SCRIPT_VAI: return 77;
        case HB_SCRIPT_AVESTAN: return 78;
        case HB_SCRIPT_BAMUM: return 79;
        case HB_SCRIPT_EGYPTIAN_HIEROGLYPHS: return 80;
        case HB_SCRIPT_IMPERIAL_ARAMAIC: return 81;
        case HB_SCRIPT_INSCRIPTIONAL_PAHLAVI: return 82;
        case HB_SCRIPT_INSCRIPTIONAL_PARTHIAN: return 83;
        case HB_SCRIPT_JAVANESE: return 84;
        case HB_SCRIPT_KAITHI: return 85;
        case HB_SCRIPT_LISU: return 86;
        case HB_SCRIPT_MEETEI_MAYEK: return 87;
        case HB_SCRIPT_OLD_SOUTH_ARABIAN: return 88;
        case HB_SCRIPT_OLD_TURKIC: return 89;
        case HB_SCRIPT_SAMARITAN: return 90;
        case HB_SCRIPT_TAI_THAM: return 91;
        case HB_SCRIPT_TAI_VIET: return 92;
        case HB_SCRIPT_BATAK: return 93;
        case HB_SCRIPT_BRAHMI: return 94;
        case HB_SCRIPT_MANDAIC: return 95;
        case HB_SCRIPT_CHAKMA: return 96;
        case HB_SCRIPT_MEROITIC_CURSIVE: return 97;
        case HB_SCRIPT_MEROITIC_HIEROGLYPHS: return 98;
        case HB_SCRIPT_MIAO: return 99;
        case HB_SCRIPT_SHARADA: return 100;
        case HB_SCRIPT_SORA_SOMPENG: return 101;
        case HB_SCRIPT_TAKRI: return 102;
        case HB_SCRIPT_BASSA_VAH: return 103;
        case HB_SCRIPT_CAUCASIAN_ALBANIAN: return 104;
        case HB_SCRIPT_DUPLOYAN: return 105;
        case HB_SCRIPT_ELBASAN: return 106;
        case HB_SCRIPT_GRANTHA: return 107;
        case HB_SCRIPT_KHOJKI: return 108;
        case HB_SCRIPT_KHUDAWADI: return 109;
        case HB_SCRIPT_LINEAR_A: return 110;
        case HB_SCRIPT_MAHAJANI: return 111;
        case HB_SCRIPT_MANICHAEAN: return 112;
        case HB_SCRIPT_MENDE_KIKAKUI: return 113;
        case HB_SCRIPT_MODI: return 114;
        case HB_SCRIPT_MRO: return 115;
        case HB_SCRIPT_NABATAEAN: return 116;
        case HB_SCRIPT_OLD_NORTH_ARABIAN: return 117;
        case HB_SCRIPT_OLD_PERMIC: return 118;
        case HB_SCRIPT_PAHAWH_HMONG: return 119;
        case HB_SCRIPT_PALMYRENE: return 120;
        case HB_SCRIPT_PAU_CIN_HAU: return 121;
        case HB_SCRIPT_PSALTER_PAHLAVI: return 122;
        case HB_SCRIPT_SIDDHAM: return 123;
        case HB_SCRIPT_TIRHUTA: return 124;
        case HB_SCRIPT_WARANG_CITI: return 125;
        case HB_SCRIPT_AHOM: return 126;
        case HB_SCRIPT_ANATOLIAN_HIEROGLYPHS: return 127;
        case HB_SCRIPT_HATRAN: return 128;
        case HB_SCRIPT_MULTANI: return 129;
        case HB_SCRIPT_OLD_HUNGARIAN: return 130;
        case HB_SCRIPT_SIGNWRITING: return 131;
        case HB_SCRIPT_ADLAM: return 132;
        case HB_SCRIPT_BHAIKSUKI: return 133;
        case HB_SCRIPT_MARCHEN: return 134;
        case HB_SCRIPT_OSAGE: return 135;
        case HB_SCRIPT_TANGUT: return 136;
        case HB_SCRIPT_NEWA: return 137;
        case HB_SCRIPT_MASARAM_GONDI: return 138;
        case HB_SCRIPT_NUSHU: return 139;
        case HB_SCRIPT_SOYOMBO: return 140;
        case HB_SCRIPT_ZANABAZAR_SQUARE: return 141;
        case HB_SCRIPT_DOGRA: return 142;
        case HB_SCRIPT_GUNJALA_GONDI: return 143;
        case HB_SCRIPT_HANIFI_ROHINGYA: return 144;
        case HB_SCRIPT_MAKASAR: return 145;
        case HB_SCRIPT_MEDEFAIDRIN: return 146;
        case HB_SCRIPT_OLD_SOGDIAN: return 147;
        case HB_SCRIPT_SOGDIAN: return 148;
        case HB_SCRIPT_ELYMAIC: return 149;
        case HB_SCRIPT_NANDINAGARI: return 150;
        case HB_SCRIPT_NYIAKENG_PUACHUE_HMONG: return 151;
        case HB_SCRIPT_WANCHO: return 152;
        case HB_SCRIPT_CHORASMIAN: return 153;
        case HB_SCRIPT_DIVES_AKURU: return 154;
        case HB_SCRIPT_KHITAN_SMALL_SCRIPT: return 155;
        case HB_SCRIPT_YEZIDI: return 156;
        case HB_SCRIPT_CYPRO_MINOAN: return 157;
        case HB_SCRIPT_OLD_UYGHUR: return 158;
        case HB_SCRIPT_TANGSA: return 159;
        case HB_SCRIPT_TOTO: return 160;
        case HB_SCRIPT_VITHKUQI: return 161;
        case HB_SCRIPT_MATH: return 162;
        case HB_SCRIPT_KAWI: return 163;
        case HB_SCRIPT_NAG_MUNDARI: return 164;
        case HB_SCRIPT_INVALID: return 165;

        case _HB_SCRIPT_MAX_VALUE: break;
    }
    assert(false && "HbScriptCompress(): invalid enum value");
    return 0xFF;
}

static const hb_script_t kHbScriptUncompressLut[] = {
    HB_SCRIPT_COMMON, HB_SCRIPT_INHERITED, HB_SCRIPT_UNKNOWN, HB_SCRIPT_ARABIC, HB_SCRIPT_ARMENIAN, HB_SCRIPT_BENGALI, HB_SCRIPT_CYRILLIC,
    HB_SCRIPT_DEVANAGARI, HB_SCRIPT_GEORGIAN, HB_SCRIPT_GREEK, HB_SCRIPT_GUJARATI, HB_SCRIPT_GURMUKHI, HB_SCRIPT_HANGUL, HB_SCRIPT_HAN,
    HB_SCRIPT_HEBREW, HB_SCRIPT_HIRAGANA, HB_SCRIPT_KANNADA, HB_SCRIPT_KATAKANA, HB_SCRIPT_LAO, HB_SCRIPT_LATIN, HB_SCRIPT_MALAYALAM,
    HB_SCRIPT_ORIYA, HB_SCRIPT_TAMIL, HB_SCRIPT_TELUGU, HB_SCRIPT_THAI, HB_SCRIPT_TIBETAN, HB_SCRIPT_BOPOMOFO, HB_SCRIPT_BRAILLE,
    HB_SCRIPT_CANADIAN_SYLLABICS, HB_SCRIPT_CHEROKEE, HB_SCRIPT_ETHIOPIC, HB_SCRIPT_KHMER, HB_SCRIPT_MONGOLIAN, HB_SCRIPT_MYANMAR,
    HB_SCRIPT_OGHAM, HB_SCRIPT_RUNIC, HB_SCRIPT_SINHALA, HB_SCRIPT_SYRIAC, HB_SCRIPT_THAANA, HB_SCRIPT_YI, HB_SCRIPT_DESERET,
    HB_SCRIPT_GOTHIC, HB_SCRIPT_OLD_ITALIC, HB_SCRIPT_BUHID, HB_SCRIPT_HANUNOO, HB_SCRIPT_TAGALOG, HB_SCRIPT_TAGBANWA, HB_SCRIPT_CYPRIOT,
    HB_SCRIPT_LIMBU, HB_SCRIPT_LINEAR_B, HB_SCRIPT_OSMANYA, HB_SCRIPT_SHAVIAN, HB_SCRIPT_TAI_LE, HB_SCRIPT_UGARITIC, HB_SCRIPT_BUGINESE,
    HB_SCRIPT_COPTIC, HB_SCRIPT_GLAGOLITIC, HB_SCRIPT_KHAROSHTHI, HB_SCRIPT_NEW_TAI_LUE, HB_SCRIPT_OLD_PERSIAN, HB_SCRIPT_SYLOTI_NAGRI,
    HB_SCRIPT_TIFINAGH, HB_SCRIPT_BALINESE, HB_SCRIPT_CUNEIFORM, HB_SCRIPT_NKO, HB_SCRIPT_PHAGS_PA, HB_SCRIPT_PHOENICIAN, HB_SCRIPT_CARIAN,
    HB_SCRIPT_CHAM, HB_SCRIPT_KAYAH_LI, HB_SCRIPT_LEPCHA, HB_SCRIPT_LYCIAN, HB_SCRIPT_LYDIAN, HB_SCRIPT_OL_CHIKI, HB_SCRIPT_REJANG,
    HB_SCRIPT_SAURASHTRA, HB_SCRIPT_SUNDANESE, HB_SCRIPT_VAI, HB_SCRIPT_AVESTAN, HB_SCRIPT_BAMUM, HB_SCRIPT_EGYPTIAN_HIEROGLYPHS,
    HB_SCRIPT_IMPERIAL_ARAMAIC, HB_SCRIPT_INSCRIPTIONAL_PAHLAVI, HB_SCRIPT_INSCRIPTIONAL_PARTHIAN, HB_SCRIPT_JAVANESE, HB_SCRIPT_KAITHI,
    HB_SCRIPT_LISU, HB_SCRIPT_MEETEI_MAYEK, HB_SCRIPT_OLD_SOUTH_ARABIAN, HB_SCRIPT_OLD_TURKIC, HB_SCRIPT_SAMARITAN, HB_SCRIPT_TAI_THAM,
    HB_SCRIPT_TAI_VIET, HB_SCRIPT_BATAK, HB_SCRIPT_BRAHMI, HB_SCRIPT_MANDAIC, HB_SCRIPT_CHAKMA, HB_SCRIPT_MEROITIC_CURSIVE, HB_SCRIPT_MEROITIC_HIEROGLYPHS,
    HB_SCRIPT_MIAO, HB_SCRIPT_SHARADA, HB_SCRIPT_SORA_SOMPENG, HB_SCRIPT_TAKRI, HB_SCRIPT_BASSA_VAH, HB_SCRIPT_CAUCASIAN_ALBANIAN,
    HB_SCRIPT_DUPLOYAN, HB_SCRIPT_ELBASAN, HB_SCRIPT_GRANTHA, HB_SCRIPT_KHOJKI, HB_SCRIPT_KHUDAWADI, HB_SCRIPT_LINEAR_A, HB_SCRIPT_MAHAJANI,
    HB_SCRIPT_MANICHAEAN, HB_SCRIPT_MENDE_KIKAKUI, HB_SCRIPT_MODI, HB_SCRIPT_MRO, HB_SCRIPT_NABATAEAN, HB_SCRIPT_OLD_NORTH_ARABIAN,
    HB_SCRIPT_OLD_PERMIC, HB_SCRIPT_PAHAWH_HMONG, HB_SCRIPT_PALMYRENE, HB_SCRIPT_PAU_CIN_HAU, HB_SCRIPT_PSALTER_PAHLAVI, HB_SCRIPT_SIDDHAM,
    HB_SCRIPT_TIRHUTA, HB_SCRIPT_WARANG_CITI, HB_SCRIPT_AHOM, HB_SCRIPT_ANATOLIAN_HIEROGLYPHS, HB_SCRIPT_HATRAN, HB_SCRIPT_MULTANI,
    HB_SCRIPT_OLD_HUNGARIAN, HB_SCRIPT_SIGNWRITING, HB_SCRIPT_ADLAM, HB_SCRIPT_BHAIKSUKI, HB_SCRIPT_MARCHEN, HB_SCRIPT_OSAGE,
    HB_SCRIPT_TANGUT, HB_SCRIPT_NEWA, HB_SCRIPT_MASARAM_GONDI, HB_SCRIPT_NUSHU, HB_SCRIPT_SOYOMBO, HB_SCRIPT_ZANABAZAR_SQUARE,
    HB_SCRIPT_DOGRA, HB_SCRIPT_GUNJALA_GONDI, HB_SCRIPT_HANIFI_ROHINGYA, HB_SCRIPT_MAKASAR, HB_SCRIPT_MEDEFAIDRIN, HB_SCRIPT_OLD_SOGDIAN,
    HB_SCRIPT_SOGDIAN, HB_SCRIPT_ELYMAIC, HB_SCRIPT_NANDINAGARI, HB_SCRIPT_NYIAKENG_PUACHUE_HMONG, HB_SCRIPT_WANCHO, HB_SCRIPT_CHORASMIAN,
    HB_SCRIPT_DIVES_AKURU, HB_SCRIPT_KHITAN_SMALL_SCRIPT, HB_SCRIPT_YEZIDI, HB_SCRIPT_CYPRO_MINOAN, HB_SCRIPT_OLD_UYGHUR, HB_SCRIPT_TANGSA,
    HB_SCRIPT_TOTO, HB_SCRIPT_VITHKUQI, HB_SCRIPT_MATH, HB_SCRIPT_KAWI, HB_SCRIPT_NAG_MUNDARI, HB_SCRIPT_INVALID
};

hb_script_t HbScriptUncompress(uint8_t script) {
    if (script <= 165 /*compressed index for HB_SCRIPT_INVALID*/)
        return kHbScriptUncompressLut[script];
    else
        return HB_SCRIPT_INVALID;
}

uint8_t HbDirectionCompress(hb_direction_t dir) {
    assert(dir >= 0 && dir <= 0b111);
    return static_cast<uint8_t>(dir);
}

hb_direction_t HbDirectionUncompress(uint8_t dir) {
    return static_cast<hb_direction_t>(dir);
}
