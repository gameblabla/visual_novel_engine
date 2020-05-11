#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>
#include "SGL_defines.h"

#define MAX_TEXT_ENTRIES_SCRIPT 5192
#define MAX_TEXT_LENGH_SCRIPT 512

#define BACKGROUND_ID_ASSETS 0
#define SPRITES_ID_ASSETS 1
#define IMAGES_ID_ASSETS 2
#define SOUNDS_ID_ASSETS 3
#define MUSIC_ID_ASSETS 4
#define VOICES_ID_ASSETS 5
#define FONT_ID_ASSETS 6

struct assets_data
{
	/* We use NUMBER_OF_IMAGES as our max limit */
	char data_memory[NUMBER_OF_IMAGES][256];
	uint_fast32_t data_entries;
};
extern struct assets_data game_assets[7];
extern char story_text[MAX_TEXT_ENTRIES_SCRIPT][MAX_TEXT_LENGH_SCRIPT];

#endif
