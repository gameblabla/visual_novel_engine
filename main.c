#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "SGL.h"
#include "array.h"

static double background_x = 0.0;

static char text_line[3][256];
static uint32_t text_progress_value = 0;

//{"1|0|0|0|0|0|0|0|0|0|0|The year is 2008.µI was at home watching some TV shows.µYou know, the usual boring stuff..."},

static void Text_Progress(uint32_t val)
{
	char * pch;
	unsigned long last_occurence = 0, text_occurance_u[2] = {0, 0};
	char temp[3][256];
	uint8_t i, number_of_characters;
	
	pch = strrchr(story_text[val],'|');
	last_occurence = (unsigned long)(pch-story_text[val])+1;
	
	pch = strchr(story_text[val],'\n');
	if (pch) text_occurance_u[0] = (unsigned long)(pch-story_text[val])+1;
	
	pch = strrchr(story_text[val],'\n');
	if (pch) text_occurance_u[1] = (unsigned long)(pch-story_text[val])+1;
	
	/* Clean up variables before doing anything to them */
	for(i=0;i<3;i++)
	{
		memset(text_line[i], 0, 256);
		memset(temp[i], 0, 256);
	}
	
	/* This means that there's only a single line of text.*/
	if (text_occurance_u[0] == 0)
	{
		strncpy(temp[0], story_text[val] + (last_occurence), (strlen(story_text[val])) - (last_occurence));
		number_of_characters = 1;
	}
	/* Only 2 lines of text */
	else if (text_occurance_u[1] == text_occurance_u[0])
	{
		strncpy(temp[0], story_text[val] + (last_occurence), (text_occurance_u[0]-1) - (last_occurence));
		strncpy(temp[1], story_text[val] + (text_occurance_u[0]), (strlen(story_text[val])) - (text_occurance_u[0]));
		number_of_characters = 2;
	}
	/* All 3 of them */
	else
	{
		strncpy(temp[0], story_text[val] + (last_occurence), (text_occurance_u[0]-1) - (last_occurence));
		strncpy(temp[1], story_text[val] + (text_occurance_u[0]),(text_occurance_u[1]-1) - (text_occurance_u[0]));
		strncpy(temp[2], story_text[val] + (text_occurance_u[1]), (strlen(story_text[val])) - (text_occurance_u[1]));
		number_of_characters = 3;
	}
	
	for(i=0;i<number_of_characters;i++)
		snprintf(text_line[i], 256, "%s", temp[i]);
}

int main( int argc, char* argv[] )
{
	(void) argc;
	(void) argv;

	time_t begin = time(NULL);
	
	Init_Video("Test game", 1280, 720, 0);
	Init_sound();
	
	Load_Image(0, "assets/background.png");
	Load_Image(100, "assets/numbers.png");
	
	Load_Text_Font(0, "assets/font.ttf", SMALL_FONT);
	
	Text_Progress(text_progress_value);
	
    while(zero_toexit_program) 
    {
		Put_image_top_left(0, background_x, 0, 255);
		
		Put_sprite_top_left(100, 0.5, 0.5, 32, 32, 0, 9, 1, 9.0, 255);
		
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
    

	time_t end = time(NULL);

	// calculate elapsed time by finding difference (end - begin)
	printf("Time elpased is %ld seconds\n", (end - begin));

	Unload_SFX();
	Close_Video();

    return 0;
}

