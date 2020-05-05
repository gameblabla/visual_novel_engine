#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include "SGL.h"
#include "array.h"

static char text_line[3][256];
static uint32_t text_progress_value = 0;

//{"1|0|0|0|0|0|0|0|0|0|0|The year is 2008.µI was at home watching some TV shows.µYou know, the usual boring stuff..."},

static int32_t current_background_id = 0;
static int32_t special_action_trigger = 0;
static int32_t special_action_trigger_choice = 0;
static int32_t move_choice_position[2] = {0, 0};

static int32_t characters_on_screen_number = 0;

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
};
static struct characters charac[2];

static void Text_Progress(uint32_t val)
{
	char * pch;
	unsigned long last_occurence = 0, text_occurance_u[2] = {0, 0};
	char temp[3][256];
	
	/* Used to detect the number of | */
	char *temp2[20];
	char temp_line[256];
	uint8_t i, number_of_text_lines;
	
	/* Find the occurances of the last | and \n */
	pch = strrchr(story_text[val],'|');
	last_occurence = (unsigned long)(pch-story_text[val])+1;
	
	pch = strchr(story_text[val],'\n');
	if (pch) text_occurance_u[0] = (unsigned long)(pch-story_text[val])+1;
	
	pch = strrchr(story_text[val],'\n');
	if (pch) text_occurance_u[1] = (unsigned long)(pch-story_text[val])+1;
	
	// We need to make a copy because it won't work otherwise
	strcpy(temp_line, story_text[val]);
	
	temp2[0] = strtok(temp_line,"|"); // Find the first |
	//printf("temp2[i] %s\n", temp2[0]);
	for(i=1;i<19;i++)
	{
		temp2[i] = strtok(NULL,"|"); // Find the subsequent |
		//printf("temp2[i] %s\n", temp2[i]);
	}
	
	characters_on_screen_number = 0;
	
	current_background_id = (int)strtoimax(temp2[0], NULL, 10);
	special_action_trigger = (int)strtoimax(temp2[1], NULL, 10);
	
	for(i=0;i<2;i++)
	{
		charac[i].character_to_display_number = (int)strtoimax(temp2[2+(i*7)], NULL, 10);
		if (charac[i].character_to_display_number >= 0) characters_on_screen_number++;
		
		charac[i].character_to_display_number_noloop_once = (int)strtoimax(temp2[3+(i*7)], NULL, 10);
		charac[i].character_end_frame = (int)strtoimax(temp2[4+(i*7)], NULL, 10);
		charac[i].character_end_frame_noloop_once = (int)strtoimax(temp2[5+(i*7)], NULL, 10);
		
		if (strtoimax(temp2[6+(i*7)], NULL, 10) > 0) charac[i].time_delay_seconds = (double)(strtoimax(temp2[6+(i*7)], NULL, 10))/100;
		if (strtoimax(temp2[7+(i*7)], NULL, 10) > 0) charac[i].width = (double)(strtoimax(temp2[7+(i*7)], NULL, 10))/ 100;
		if (strtoimax(temp2[8+(i*7)], NULL, 10) > 0)  charac[i].height = (double)(strtoimax(temp2[8+(i*7)], NULL, 10))/ 100;
	}
	
	special_action_trigger_choice = (int)strtoimax(temp2[15], NULL, 10);
	move_choice_position[0] = (int)strtoimax(temp2[16], NULL, 10);
	move_choice_position[1] = (int)strtoimax(temp2[17], NULL, 10);
	
	/* Clean up variables before doing anything to them */
	for(i=0;i<3;i++)
	{
		memset(text_line[i], 0, 256);
		memset(temp[i], 0, 256);
	}
	
	/* Following lines of code uses the position we extracted from the string 
	 * and copy the strings accordingly.
	 * */
	
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
}

int main( int argc, char* argv[] )
{
	uint32_t i;
	(void) argc;
	(void) argv;

	Init_Video("Test game", 1920, 1080, 0);
	Init_sound();
	
	Load_Background(0, "assets/background.png");
	Load_Sprite(0, "assets/numbers.png");
	Load_Picture(0, "assets/messagebox.png");
	
	Load_Text_Font(0, "assets/font.ttf", SMALL_FONT);
	
	Text_Progress(text_progress_value);
	
    while(zero_toexit_program) 
    {
		Put_background(0);

		for(i=0;i<2;i++)
		{
			if (charac[i].character_to_display_number >= 0)
			{
				// Center the character on-screen
				charac[i].x = (1.0/(double)(characters_on_screen_number+1)) - charac[i].width/2;
				
				Put_sprite_top_left(charac[i].character_to_display_number, charac[i].x, 0.2125, 
				
				Return_Sprite_Width(charac[i].character_to_display_number), charac[i].character_end_frame ? Return_Sprite_Height(charac[i].character_to_display_number)/charac[i].character_end_frame : Return_Sprite_Height(charac[i].character_to_display_number), 
				
				// Start and End_frame
				0,
				charac[i].character_end_frame,
				// Loop
				1, 
				// Time it takes until the last frame
				charac[i].time_delay_seconds,
				// Alpha
				255,
				// Relative width & height (Not sprite itself, but how it will be stretched/displayed)
				charac[i].width, charac[i].height);
			}
		
			Put_image_top_left(0, 0.0, 1.0-0.33, 128, 1.0, 0.33);
		}
		
		Draw_Text(0, 0.1, 0.72, text_line[0], 255, 255, 255);
		Draw_Text(0, 0.1, 0.8, text_line[1], 255, 255, 255);
		Draw_Text(0, 0.1, 0.88, text_line[2], 255, 255, 255);
		
		if (Controller_Input.buttons[CROSS_BUTTON] == PRESSED)
		{
			text_progress_value += 1;
			Text_Progress(text_progress_value);
		}
		
		Sync_video();
    }

	Unload_SFX();
	Close_Video();

    return 0;
}

