#ifndef TEXT_VN_H
#define TEXT_VN_H

#include <stdint.h>
#include "SGL_defines.h"
#include "array.h"

struct characters
{
	int character_to_display_number;
	int character_to_display_number_noloop_once;
	int character_end_frame;
	int character_end_frame_noloop_once;
	double time_delay_seconds;
	double width;
	double height;
	double x;
	double y;
	double alpha;
};

extern char text_line[3][MAX_TEXT_LENGH_SCRIPT];
extern uint32_t text_progress_value;

extern int32_t current_background_id;
extern double previous_current_background_id_alpha;
extern int32_t previous_current_background_id;

extern int32_t special_action_trigger;
extern int32_t special_action_trigger_choice;
extern int32_t move_choice_position[2];
extern int32_t characters_on_screen_number;

extern int32_t music_to_play;
extern int32_t previous_music_to_play;
extern int32_t music_trigger;

extern int32_t sound_to_play;
extern int32_t sound_trigger;

extern int32_t voice_to_play;
extern int32_t voice_trigger;

extern struct characters charac[2], previous_charac[2];

/* Functions */
extern uint8_t Load_Text_Files(void);
extern void Text_Progress(uint32_t val);
extern void Music_Slot_Play(int32_t a);
extern void Voice_Slot_Play(int32_t a);

#endif
