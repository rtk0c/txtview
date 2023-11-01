#pragma once

#include <harfbuzz/hb.h>

// 8 bits
uint8_t HbScriptCompress(hb_script_t);
hb_script_t HbScriptUncompress(uint8_t);

// 3 bits
uint8_t HbDirectionCompress(hb_direction_t);
hb_direction_t HbDirectionUncompress(uint8_t);
