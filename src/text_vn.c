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

char text_line[3][MAX_TEXT_LENGH_SCRIPT];

static int32_t previous_characters_on_screen_number = 0;
int32_t previous_current_background_id = 0;

uint32_t text_progress_value = 0;
int32_t current_background_id = 0;
double previous_current_background_id_alpha = 255.0;

int32_t special_action_trigger = 0;
int32_t special_action_trigger_choice = 0;
int32_t move_choice_position[2] = {0, 0};
int32_t characters_on_screen_number = 0;

int32_t music_to_play = 0;
int32_t previous_music_to_play = 0;
int32_t music_trigger = 0;

int32_t sound_to_play = 0;
int32_t sound_trigger = 0;

int32_t voice_to_play = 0;
int32_t voice_trigger = 0;

struct characters charac[2], previous_charac[2];

static uint_fast32_t Load_Assets_Date(const char* path, struct assets_data* ex)
{
	FILE* fp;
	uint_fast32_t lineNum = 0;
	size_t ln;
	char line[MAX_TEXT_LENGH_SCRIPT];
	
	fp = fopen(path, "r");
	if (!fp)
	{
		printf("File %s is missing !\n", path);
		return 0;
	}
	
	while (fgets(line, sizeof(line), fp))
	{
		char *arg = strchr(line, '\n');

		snprintf(ex->data_memory[lineNum], MAX_TEXT_LENGH_SCRIPT, "assets/%s", line);
		
		/* Strip out \n from the string. It should be removed for file paths. */
		ln = strlen(ex->data_memory[lineNum]) - 1;
		if (*ex->data_memory[lineNum] && ex->data_memory[lineNum][ln] == '\n') 
			ex->data_memory[lineNum][ln] = '\0';
		
		++lineNum;

		if (!arg) {
			continue;
		}

		*arg = '\0';
		arg++;
	}
	fclose(fp);
	
	return lineNum;
}

uint8_t Load_Text_Files(void)
{
	/* For loading the story text */
	char line[MAX_TEXT_LENGH_SCRIPT], newline[MAX_TEXT_LENGH_SCRIPT];
	FILE* fp;
	
	uint_fast32_t lineNum = 0;
	uint_fast32_t i, z = 0;
	uint_fast8_t count = 0;
	unsigned long a = 0;
	
	memset(line, 0, sizeof(line));
	
	fp = fopen("desc/story.txt", "r");
	if (!fp)
	{
		fprintf(stderr, "Important story file is missing !\n");
		return 0;
	}
	
	while (fgets(line, sizeof(line), fp))
	{
		char *arg = strchr(line, '\n');
		
		count = 0;
		a = 0;
		memset(newline, 0, sizeof(newline));

		for(i=0;i<sizeof(line);i++)
		{
			if (line[i] == '\\' && count < 2)
			{
				newline[a] = '\n';
				a++;
				i+=1;
				count++;
			}
			else
			{
				if (line[i] == 10 && count > 1)
				{
					a++;
					newline[a] = 0;
					i = sizeof(line);
					break;
				}
				else
				{
					newline[a] = line[i];
					a++;
				}
			}
		}
		snprintf(story_text[lineNum], MAX_TEXT_LENGH_SCRIPT, "%s", newline);

		++lineNum;

		if (!arg) {
			continue;
		}

		*arg = '\0';
		arg++;
	}
	fclose(fp);
	
	/* Hardcoded file names for external resources */
	const char hardcoded_filenames[7][128] =
	{
		{"desc/backgrounds.txt"},
		{"desc/sprites.txt"},
		{"desc/images.txt"},
		{"desc/sounds.txt"},
		{"desc/music.txt"},
		{"desc/voices.txt"},
		{"desc/fonts.txt"}
	};
	
	for(i=0;i<7;i++)
	{
		game_assets[i].data_entries = Load_Assets_Date(hardcoded_filenames[i], &game_assets[i]);
		switch(i)
		{
			case BACKGROUND_ID_ASSETS:
				for(z=0;z<game_assets[i].data_entries;z++)
				{
					if (!Load_Background(z, game_assets[i].data_memory[z])) return 0;
				}
			break;
			case SPRITES_ID_ASSETS:
				for(z=0;z<game_assets[i].data_entries;z++)
				{
					if (!Load_Sprite(z, game_assets[i].data_memory[z])) return 0;
				}
			break;
			case IMAGES_ID_ASSETS:
				for(z=0;z<game_assets[i].data_entries;z++)
				{
					if (!Load_Picture(z, game_assets[i].data_memory[z])) return 0;
				}
			break;
			case SOUNDS_ID_ASSETS:
				for(z=0;z<game_assets[i].data_entries;z++)
				{
					if (!Load_SFX(z, game_assets[i].data_memory[z])) return 0;
				}
			break;
			case FONT_ID_ASSETS:
				for(z=0;z<game_assets[i].data_entries;z++)
				{
					if (!Load_Text_Font(z, (const char*)game_assets[i].data_memory[z])) return 0;
				}
				
			break;
			default:
			break;
		}
	}
	
	if (!Load_Titlescreen("assets/titlescreen.png"))
	{
		return 0;
	}

	/* 1 means sucess */
	return 1;
}

void Text_Progress(uint32_t val)
{
	char * pch;
	unsigned long last_occurence = 0, text_occurance_u[2] = {0, 0};
	char temp[3][MAX_TEXT_LENGH_SCRIPT];
	
	/* Used to detect the number of | */
	char *temp2[24];
	char temp_line[MAX_TEXT_LENGH_SCRIPT];
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

	/* This takes care of centering the 2 characters according to the number of them on-screen and circumstances */
	for(i=0;i<characters_on_screen_number;i++)
	{
		tmp_var = 1.0/(double)(characters_on_screen_number+1);
	
		charac[i].x = (tmp_var - charac[i].width/2) + (i * tmp_var);

		charac[i].y = charac[i].height/2 - (0.25);
	}
	
	/* In case we go from 0 to 1 character... */
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
		memset(text_line[i], 0, MAX_TEXT_LENGH_SCRIPT);
		memset(temp[i], 0, MAX_TEXT_LENGH_SCRIPT);
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
	
	/* This parses the final result to the final text array */
	for(i=0;i<number_of_text_lines;i++)
		snprintf(text_line[i], MAX_TEXT_LENGH_SCRIPT, "%s", temp[i]);
		
	Unload_Voice();
}

void Music_Slot_Play(int32_t a)
{
	Load_Music(game_assets[MUSIC_ID_ASSETS].data_memory[a]);
	Play_Music(1);
}

void Voice_Slot_Play(int32_t a)
{
	Load_Voice(game_assets[VOICES_ID_ASSETS].data_memory[a]);
	Play_Voice();
}


