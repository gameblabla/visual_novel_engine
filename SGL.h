#ifndef SIMPLE_GAME_LIBRARY
#define SIMPLE_GAME_LIBRARY

#include <stdint.h>

/* Variables, functions, arrays */

#define BIG_FONT 1
#define SMALL_FONT 0

extern uint32_t zero_toexit_program;

/* Video */
extern void Init_Video(const char* title, uint_fast32_t width, uint_fast32_t height, uint_fast32_t screen_mode);
extern uint_fast8_t Load_Image(uint_fast32_t a, const char* directory);
extern void Put_image_top_left(uint_fast32_t a, double top_left_x, double top_left_y);
extern void Put_sprite_top_left(uint_fast32_t a, double top_left_x, double top_left_y, uint_fast32_t w, uint_fast32_t h, uint_fast32_t f);
extern void Sync_video();
extern void Close_Video();

/* Text */
extern uint_fast8_t Load_Text_Font(uint_fast32_t a, const char* directory, uint32_t big);
extern void Draw_Text(uint_fast32_t a, double top_left_x, double top_left_y, const char* text_to_write, uint8_t red, uint8_t green, uint8_t blue);
extern void Close_Font(uint_fast32_t a);
extern void Close_All_Fonts();

/* Movement */

extern void Move_Position_X(double* x, double xVel);
extern void Move_Position_Y(double* y, double yVel);

/* Input */

extern void Controller_update();

/* Sound playback */

extern void Init_sound();
extern void Clean_Music();
extern void Load_Music(const char* directory);
extern void Play_Music(uint_fast32_t loop);
extern void Load_SFX(uint_fast32_t i, const char* directory);
extern void Play_SFX(uint_fast32_t i);
extern void Unload_SFX();

#endif
