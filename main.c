#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SGL.h"

static double background_x = 0.0;

int main( int argc, char* argv[] )
{
	(void) argc;
	(void) argv;

	time_t begin = time(NULL);
	
	Init_Video("Test game", 1280, 720, 0);
	Init_sound();
	
	Load_Image(0, "assets/background.png");
	Load_Image(1, "assets/numbers.png");
	
	Load_Text_Font(0, "assets/font.ttf", SMALL_FONT);
	
    while(zero_toexit_program) 
    {
		if (Controller_Input.buttons[CROSS_BUTTON] == PRESSED) zero_toexit_program = 0;
		if (Controller_Input.dpad[UP_DPAD] == RELEASED) zero_toexit_program = 0;
		
		//Move_Position_X(&background_x, 0.1);
		
		Put_image_top_left(0, background_x, 0, 255);
		
		Put_sprite_top_left(1, 0.5, 0.5, 32, 32, 0, 9, 1, 0.25, 255);
		
		Draw_Text(0, 0.1, 0.72, "Random character : ", 255, 255, 255);
		Draw_Text(0, 0.1, 0.8, "I love love ! Fuck you all boys ~~ !", 255, 255, 255);
		Draw_Text(0, 0.1, 0.88, "YEAH DAYTONA !", 255, 255, 255);
		
		Sync_video();
		
		//if (background_x > 1.0f) zero_toexit_program = 0;
    }
    

	time_t end = time(NULL);

	// calculate elapsed time by finding difference (end - begin)
	printf("Time elpased is %ld seconds\n", (end - begin));

	Unload_SFX();
	Close_Video();

    return 0;
}

