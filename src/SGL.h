#ifndef SIMPLE_GAME_LIBRARY
#define SIMPLE_GAME_LIBRARY

#include <stdint.h>

/* Variables, functions, arrays */

#define BIG_FONT 1
#define SMALL_FONT 0

extern uint32_t zero_toexit_program;

#define MAX_BUTTONS 128
struct player_input
{
	uint_fast8_t dpad[4];
	/* Don't use timer_buttons in your code !*/
	uint_fast8_t timer_dpad[4];
	
	uint_fast8_t buttons[MAX_BUTTONS];
	/* Don't use timer_buttons in your code !*/
	uint_fast8_t timer_buttons[MAX_BUTTONS];
};
extern struct player_input Controller_Input;

// .dpad[X] where X is...
#define UP_DPAD 0
#define DOWN_DPAD 1
#define LEFT_DPAD 2
#define RIGHT_DPAD 3

// Controller_Input.buttons[X] where X is...

#define CROSS_BUTTON 0
#define CIRCLE_BUTTON 1
#define SQUARE_BUTTON 2
#define TRIANGLE_BUTTON 3

#define SELECT_BUTTON 4
#define START_BUTTON 6

#define L3_BUTTON 7
#define R3_BUTTON 8

#define L1_BUTTON 9
#define R1_BUTTON 10

// State of buttons
#define UNPRESSED 0
#define PRESSED 1
#define HELD 2
#define RELEASED 3


/* Macro for functions */

#define Load_Background(a, b) Load_Image(0, a, b)
#define Load_Sprite(a, b) Load_Image(1, a, b)
#define Load_Picture(a, b) Load_Image(2, a, b)

/* Video */
extern void Init_Video(const char* title, int width, int height, uint_fast32_t screen_mode);

/* WARNING : Use the macros instead of this function !!! */
extern uint_fast8_t Load_Image(uint_fast32_t p, uint_fast32_t a, const char* directory);

extern void Put_background(int a);
extern void Put_background_override(int a, uint8_t alpha);

extern void Put_image_top_left(int a, double top_left_x, double top_left_y, uint8_t alpha, double width, double height);
extern void Put_sprite_top_left(int a, double top_left_x, double top_left_y, int w_frame, int h_frame, int frame_start, int frame_end, int loop, double seconds, uint8_t alpha, double width, double height);

/* Use in case you set the loop to 0 and want to reset it back to frame zero */
extern void Reset_Sprite_Frame_Counter(int a);

extern int Return_Sprite_Width(int a);
extern int Return_Sprite_Height(int a);

extern double Return_Alpha_tick(double seconds);

extern void Sync_video(void);
extern void Close_Video(void);

/* Text */
extern uint_fast8_t Load_Text_Font(uint_fast32_t a, const char* directory);
extern void Draw_Text(uint_fast32_t a, double top_left_x, double top_left_y, const char* text_to_write, uint8_t red, uint8_t green, uint8_t blue);
extern void Close_Font(uint_fast32_t a);
extern void Close_All_Fonts(void);

/* Movement */

extern void Move_Position_X(double* x, double xVel);
extern void Move_Position_Y(double* y, double yVel);


/* Sound playback */

extern void Init_sound(void);
extern void Clean_Music(void);
extern void Load_Music(const char* directory);
extern void Play_Music(uint_fast32_t loop);
extern uint_fast32_t Load_SFX(uint_fast32_t i, const char* directory);
extern void Play_SFX(uint_fast32_t i);
extern void Unload_SFX(void);

extern void Load_Voice(const char* directory);
extern void Play_Voice(void);
extern void Unload_Voice(void);
extern void Stop_Voice(void);

#endif
