#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "SGL.h"
#include "array.h"

static char text_line[3][256];
static uint32_t text_progress_value = 0;

static int32_t current_background_id = 0;

static int32_t previous_current_background_id = 0;
static double previous_current_background_id_alpha = 255.0;

static int32_t special_action_trigger = 0;
static int32_t special_action_trigger_choice = 0;
static int32_t move_choice_position[2] = {0, 0};

static int32_t characters_on_screen_number = 0;
static int32_t previous_characters_on_screen_number = 0;

static int32_t music_to_play = 0;
static int32_t previous_music_to_play = 0;
static int32_t music_trigger = 0;

static int32_t sound_to_play = 0;
static int32_t sound_trigger = 0;

static int32_t voice_to_play = 0;
static int32_t voice_trigger = 0;

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
static struct characters charac[2], previous_charac[2];

static void Text_Progress(uint32_t val)
{
	char * pch;
	unsigned long last_occurence = 0, text_occurance_u[2] = {0, 0};
	char temp[3][256];
	
	/* Used to detect the number of | */
	char *temp2[24];
	char temp_line[256];
	uint8_t i, number_of_text_lines;
	double tmp_var;
	
	/* Find the occurances of the last | and \n */
	pch = strrchr(story_text[val],'|');
	last_occurence = (unsigned long)(pch-story_text[val])+1;
	
	pch = strchr(story_text[val],'\n');
	if (pch) text_occurance_u[0] = (unsigned long)(pch-story_text[val])+1;
	
	pch = strrchr(story_text[val],'\n');
	if (pch) text_occurance_u[1] = (unsigned long)(pch-story_text[val])+1;
	
	// We need to make a copy because it won't work otherwise
	snprintf(temp_line, sizeof(temp_line), "%s", story_text[val]);
	
	/* This will be useful for our script format. We get the position of each | in the array. */
	temp2[0] = strtok(temp_line,"|"); // Find the first |
	//printf("temp2[i] %s\n", temp2[0]);
	for(i=1;i<23;i++)
	{
		temp2[i] = strtok(NULL,"|"); // Find the subsequent |
		//printf("temp2[i] %s\n", temp2[i]);
	}
	
	if (val != 0)
	{
		previous_characters_on_screen_number = characters_on_screen_number;
		memcpy((void*)previous_charac, charac, sizeof(charac));
	}
	
	characters_on_screen_number = 0;
	previous_current_background_id_alpha = 255.0;
	
	previous_current_background_id = current_background_id;
	
	/*	For our script format, we use our custom one separated by |.
		strtoimax is C99 compliant and used to convert back from string to integer.
		See array.h for more details. (the variable names should also be descriptive enough)
	*/
	
	current_background_id = (int)strtoimax(temp2[0], NULL, 10);
	special_action_trigger = (int)strtoimax(temp2[1], NULL, 10);

	for(i=0;i<2;i++)
	{
		charac[i].character_to_display_number = (int)strtoimax(temp2[2+(i*7)], NULL, 10);
		if (charac[i].character_to_display_number >= 0)
		{
			characters_on_screen_number++;
		}
		else
		{
			if (previous_charac[i].character_to_display_number >= 0 && val != 0)
			{
				charac[i].alpha = 254;
			}
		}
		
		charac[i].character_to_display_number_noloop_once = (int)strtoimax(temp2[3+(i*7)], NULL, 10);
		charac[i].character_end_frame = (int)strtoimax(temp2[4+(i*7)], NULL, 10);
		charac[i].character_end_frame_noloop_once = (int)strtoimax(temp2[5+(i*7)], NULL, 10);
		
		if (strtoimax(temp2[6+(i*7)], NULL, 10) > 0) charac[i].time_delay_seconds = (double)(strtoimax(temp2[6+(i*7)], NULL, 10))/100;
		if (strtoimax(temp2[7+(i*7)], NULL, 10) > 0) charac[i].width = (double)(strtoimax(temp2[7+(i*7)], NULL, 10))/ 100;
		if (strtoimax(temp2[8+(i*7)], NULL, 10) > 0) charac[i].height = (double)(strtoimax(temp2[8+(i*7)], NULL, 10))/ 100;
		
		charac[i].alpha = 255;
	}

	for(i=0;i<characters_on_screen_number;i++)
	{
		tmp_var = 1.0/(double)(characters_on_screen_number+1);
	
		charac[i].x = (tmp_var - charac[i].width/2) + (i * tmp_var);

		charac[i].y = charac[i].height/2 - (0.25);
	}
	
	if (characters_on_screen_number == 1 && previous_characters_on_screen_number == 0)
	{
		for(i=0;i<2;i++)
		{
			if (charac[i].character_to_display_number >= 0)
			{
				previous_charac[i].x = charac[i].x;
			}
		}
	}
	
	/* If we go from 1 or several characters to zero, assuming we are changing the entire scene. */
	if (characters_on_screen_number == 0 && previous_characters_on_screen_number > 0)
	{
		for(i=0;i<2;i++)
		{
			charac[i].alpha = 254;
		}
	}
	
	special_action_trigger_choice = (int)strtoimax(temp2[16], NULL, 10);
	move_choice_position[0] = (int)strtoimax(temp2[17], NULL, 10);
	move_choice_position[1] = (int)strtoimax(temp2[18], NULL, 10);
	
	previous_music_to_play = music_to_play;
	music_to_play = (int32_t)strtoimax(temp2[19], NULL, 10);
	sound_to_play = (int32_t)strtoimax(temp2[20], NULL, 10);
	voice_to_play = (int32_t)strtoimax(temp2[21], NULL, 10);
	
	music_trigger = 0;
	sound_trigger = 0;
	voice_trigger = 0;
	
	/* Clean up variables before doing anything to them */
	for(i=0;i<3;i++)
	{
		memset(text_line[i], 0, 256);
		memset(temp[i], 0, 256);
	}
	
	/* Following lines of code uses the position we extracted from the string and copy the strings accordingly. */
	
	/* This means that there's only a single line of text.*/
	if (text_occurance_u[0] == 0)
	{
		strncpy(temp[0], story_text[val] + (last_occurence), (strlen(story_text[val])) - (last_occurence));
		number_of_text_lines = 1;
	}
	/* Only 2 lines of text */
	else if (text_occurance_u[1] == text_occurance_u[0])
	{
		strncpy(temp[0], story_text[val] + (last_occurence), (text_occurance_u[0]-1) - (last_occurence));
		strncpy(temp[1], story_text[val] + (text_occurance_u[0]), (strlen(story_text[val])) - (text_occurance_u[0]));
		number_of_text_lines = 2;
	}
	/* All 3 of them */
	else
	{
		strncpy(temp[0], story_text[val] + (last_occurence), (text_occurance_u[0]-1) - (last_occurence));
		strncpy(temp[1], story_text[val] + (text_occurance_u[0]),(text_occurance_u[1]-1) - (text_occurance_u[0]));
		strncpy(temp[2], story_text[val] + (text_occurance_u[1]), (strlen(story_text[val])) - (text_occurance_u[1]));
		number_of_text_lines = 3;
	}
	
	for(i=0;i<number_of_text_lines;i++)
		snprintf(text_line[i], 256, "%s", temp[i]);
		
	Unload_Voice();
}

static void Music_Slot_Play(int32_t a)
{
	const char music_table[4][256] =
	{
		{"assets/music.opus"},
		{"assets/music.opus"},
		{"assets/music.opus"},
		{"assets/music.opus"}
	};
	Load_Music(music_table[a]);
	Play_Music(1);
}

static void Voice_Slot_Play(int32_t a)
{
	const char vc_table[4][256] =
	{
		{"assets/voice_1.flac"},
		{"assets/voice_2.flac"},
		{"assets/voice_3.flac"},
		{"assets/voice_4.flac"}
	};
	Load_Voice(vc_table[a]);
	Play_Voice();
}

int main( int argc, char* argv[] )
{
	uint32_t i;
	uint8_t choice_input = 0;
	(void) argc;
	(void) argv;

	Init_Video("Test game", 1920, 1080, 0);
	Init_sound();
	
	Load_Background(0, "assets/background.png");
	Load_Background(1, "assets/background2.png");
	
	Load_Sprite(0, "assets/sprites.png");
	Load_Sprite(1, "assets/sprites.png");
	
	Load_Picture(0, "assets/messagebox.png");
	
	Load_Text_Font(0, "assets/font.ttf", SMALL_FONT);
	
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
			Play_SFX((int32_t)sound_to_play);
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

	Clean_Music();
	Unload_SFX();
	Stop_Voice();
	Close_Video();

    return 0;
}

