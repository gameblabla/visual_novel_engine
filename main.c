#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SGL.h"

double background_x = 0.0;

int main( int argc, char* argv[] )
{
	clock_t start_t, end_t, total_t;
	
	time_t begin = time(NULL);
	
	Init_Video("Test game", 1280, 720, 0);
	Load_Image(0, "assets/background.png");
	
	Load_Text_Font(0, "assets/font.ttf", SMALL_FONT);
	
    while(zero_toexit_program) 
    {
		Controller_update();
		
		//Move_Position_X(&background_x, 0.1);
		
		Put_image_top_left(0, background_x, 0);
		
		Draw_Text(0, 0.1, 0.72, "Random character : ", 255, 255, 255);
		Draw_Text(0, 0.1, 0.8, "I love love ! Fuck you all boys ~~ !", 255, 255, 255);
		Draw_Text(0, 0.1, 0.88, "YEAH DAYTONA !", 255, 255, 255);
		
		Sync_video();
		
		//if (background_x > 1.0f) zero_toexit_program = 0;
    }
    

	time_t end = time(NULL);

	// calculate elapsed time by finding difference (end - begin)
	printf("Time elpased is %d seconds\n", (end - begin));

	Unload_SFX();
	Close_Video();

    return 0;
}

