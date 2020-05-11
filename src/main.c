#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "SGL.h"
#include "text_vn.h"
#include "array.h"

static void Exit_Game(void)
{
	Clean_Music();
	Unload_SFX();
	Stop_Voice();
	Close_Video();
}

int main( int argc, char* argv[] )
{
	uint32_t i;
	uint32_t err;
	uint8_t choice_input = 0;
	(void) argc;
	(void) argv;

	Init_Video("Test game", 1280, 720, 0);
	Init_sound();
	
	err = Load_Text_Files();
	if (!err)
	{
		fprintf(stderr, "ERROR : Failed to load text files, missing text files !\nCheck your text files !\n");
		Exit_Game();
		return 1;
	}
	Text_Progress(text_progress_value);
	
    while(zero_toexit_program) 
    {
		/* Music, Sound effects and Voices. */
		if (previous_music_to_play != music_to_play && previous_music_to_play > -1 && music_trigger == 0)
		{
			Music_Slot_Play(music_to_play);
			music_trigger = 1;
		}
		
		if (sound_trigger == 0 && sound_to_play > -1)
		{
			Play_SFX((uint_fast32_t)sound_to_play);
			sound_trigger = 1;
		}
		
		if (voice_trigger == 0 && voice_to_play > -1)
		{
			Voice_Slot_Play(voice_to_play);
			voice_trigger = 1;
		}
		
		/* Displaying graphics */
		
		Put_background(current_background_id);
		/* Background fading */
		if (previous_current_background_id_alpha > 0.5 && previous_current_background_id != current_background_id)
		{
			Put_background_override(previous_current_background_id, (uint8_t)previous_current_background_id_alpha);
			previous_current_background_id_alpha -= Return_Alpha_tick(0.5);
		}

		for(i=0;i<2;i++)
		{
			/* The character disappears if alpha is set to 254. */
			if (charac[i].alpha < 255 && charac[i].alpha > 0)
			{
				charac[i].alpha -= Return_Alpha_tick(0.5);
				
				Put_sprite_top_left(previous_charac[i].character_to_display_number, previous_charac[i].x, previous_charac[i].y, 
					
				Return_Sprite_Width(previous_charac[i].character_to_display_number), previous_charac[i].character_end_frame ? Return_Sprite_Height(previous_charac[i].character_to_display_number)/previous_charac[i].character_end_frame : Return_Sprite_Height(previous_charac[i].character_to_display_number), 
					
				// Start and End_frame
				0,
				previous_charac[i].character_end_frame,
				// Loop
				1, 
				// Time it takes until the last frame
				previous_charac[i].time_delay_seconds,
				// Alpha
				(uint8_t)charac[i].alpha,
				// Relative width & height (Not sprite itself, but how it will be stretched/displayed)
				previous_charac[i].width, previous_charac[i].height);
				
			}
			/* This takes care of moving the character according to changes being made */
			else if ((previous_charac[i].x > charac[i].x || previous_charac[i].x < charac[i].x) && charac[i].character_to_display_number >= 0)
			{
				
				/* Move the Sprite if it's not at the exact location set */
				if (previous_charac[i].x > charac[i].x) Move_Position_X(&previous_charac[i].x, -1.5);
				else if (previous_charac[i].x < charac[i].x) Move_Position_X(&previous_charac[i].x, 1.5);
				
				/* Force position if the difference between the two positions is small */
				if (fabs(previous_charac[i].x - charac[i].x) < 0.01)
				{
					previous_charac[i].x = charac[i].x;
				}
				
				Put_sprite_top_left(charac[i].character_to_display_number, previous_charac[i].x, charac[i].y, 
					
				Return_Sprite_Width(charac[i].character_to_display_number), charac[i].character_end_frame ? Return_Sprite_Height(charac[i].character_to_display_number)/charac[i].character_end_frame : Return_Sprite_Height(charac[i].character_to_display_number), 
					
				// Start and End_frame
				0,
				charac[i].character_end_frame,
				// Loop
				1, 
				// Time it takes until the last frame
				charac[i].time_delay_seconds,
				// Alpha
				(uint8_t)charac[i].alpha,
				// Relative width & height (Not sprite itself, but how it will be stretched/displayed)
				charac[i].width, charac[i].height);
			}
			else
			{
				Put_sprite_top_left(charac[i].character_to_display_number, charac[i].x, charac[i].y, 
					
				Return_Sprite_Width(charac[i].character_to_display_number), charac[i].character_end_frame ? Return_Sprite_Height(charac[i].character_to_display_number)/charac[i].character_end_frame : Return_Sprite_Height(charac[i].character_to_display_number), 
					
				// Start and End_frame
				0,
				charac[i].character_end_frame,
				// Loop
				1, 
				// Time it takes until the last frame
				charac[i].time_delay_seconds,
				// Alpha
				(uint8_t)charac[i].alpha,
				// Relative width & height (Not sprite itself, but how it will be stretched/displayed)
				charac[i].width, charac[i].height);
			}
		}
		
		/* Draw messagebox */
		Put_image_top_left(0, 0.0, 1.0-0.33, 128, 1.0, 0.34);
		
		/* Draw lines of text */
		Draw_Text(0, 0.1, 0.72, text_line[0], 255, 255, 255);
		Draw_Text(0, 0.1, 0.8, text_line[1], 255, 255, 255);
		Draw_Text(0, 0.1, 0.88, text_line[2], 255, 255, 255);
		
		/* Input, pretty basic */
		
		switch(special_action_trigger_choice)
		{
			/* Linear Choice */
			default:
				if (Controller_Input.buttons[CROSS_BUTTON] == PRESSED)
				{
					text_progress_value += 1;
					Text_Progress(text_progress_value);
				}
			break;
			/* Possible branching path : 2 choices */
			case 1:
				switch(choice_input)
				{
					case 0:
						Draw_Text(0, 0.02, 0.8, "=>", 255, 255, 255);
					break;
					case 1:
						Draw_Text(0, 0.02, 0.88, "=>", 255, 255, 255);
					break;
				}
				
				if (Controller_Input.dpad[UP_DPAD] == PRESSED) choice_input = 0;
				else if (Controller_Input.dpad[DOWN_DPAD] == PRESSED) choice_input = 1;
				
				if (Controller_Input.buttons[CROSS_BUTTON] == PRESSED)
				{
					special_action_trigger_choice = 0;
					// Text_progress_value needs to be reset to whatever the user chose
					text_progress_value = (uint32_t)move_choice_position[choice_input];
					Text_Progress((uint32_t)move_choice_position[choice_input]);
				}
			break;
		}

		/* Vsync, Post-Processing */
		Sync_video();
    }

	Exit_Game();

    return 0;
}

