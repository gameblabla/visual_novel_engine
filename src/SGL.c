#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <sys/time.h>
#include <sys/types.h>

#include "SGL.h"
#include "SGL_defines.h"

static int current_internal_resolution_width = 0;
static int current_internal_resolution_height = 0;

static SDL_Window * window;
static SDL_Renderer * renderer;

static SDL_Texture * titlescreen_data;

struct image_structure
{
	SDL_Texture * texture[NUMBER_OF_IMAGES];
	int width[NUMBER_OF_IMAGES];
	int height[NUMBER_OF_IMAGES];
	double current_frame[NUMBER_OF_IMAGES];
	
	int width_frame[NUMBER_OF_IMAGES];
};
static struct image_structure img_struct[3];

static Mix_Music* music;
static Mix_Chunk* sfx_id[MAX_SFX];
static Mix_Chunk* voice_sfx;

static SDL_GameController *pad[MAX_JOYSTICK];
static SDL_Joystick *joy[MAX_JOYSTICK];
static SDL_JoystickID instanceID[MAX_JOYSTICK];

uint32_t zero_toexit_program = 1;

/* Used to lock the FPS not via Vsync but internally */
static uint32_t lockfps_game = 0;

/* FPS calculation */
static long Timer_Read(void) 
{
	/* Timing. */
	struct timeval tval;
  	gettimeofday(&tval, 0);
	return (((tval.tv_sec*1000000) + (tval.tv_usec)));
}
static long lastTick = 0, newTick;
static uint32_t frames_FPS_calculation = 0;

/* Used to detect and lock the framerate */
static uint32_t Get_Refresh_rate = 0;
static double FPS_sleep;

/* Input */

struct player_input Controller_Input;

/* Text FONTs */
static TTF_Font * font[NUMBER_OF_FONTS];

static void msleep(double milisec)
{
	#ifdef POSIX
		struct timespec req={0};
		time_t sec=(uint32_t)(milisec/1000);

		milisec=milisec-(sec*1000);
		req.tv_sec=sec;
		req.tv_nsec=milisec*1000000L;

		while(nanosleep(&req,&req)==-1)
		continue;
	#else
		SDL_Delay((Uint32)milisec);
	#endif
}

static uint32_t SDLGetWindowRefreshRate(SDL_Window *Window)
{
    SDL_DisplayMode Mode;
    int DisplayIndex = SDL_GetWindowDisplayIndex(Window);
    
    // If we can't find the refresh rate, we'll return this:
    uint32_t DefaultRefreshRate = 60;
    
    if (SDL_GetDesktopDisplayMode(DisplayIndex, &Mode) != 0)
    {
        return DefaultRefreshRate;
    }
    if (Mode.refresh_rate == 0)
    {
        return DefaultRefreshRate;
    }
    return (uint32_t)Mode.refresh_rate;
}

void Init_Video(const char* title, int width, int height, uint_fast32_t screen_mode, uint_fast32_t sync_mode)
{
	SDL_DisplayMode Mode;
	int DisplayIndex;
	uint32_t i;
	int vsync_flag = 0;
	
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|SDL_INIT_AUDIO);
	
	switch(screen_mode)
	{
		case 0:
			/* Set up a window that is resizable. It is set to the native resolution by default. */
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		break;
		case 1:
			/* Set an empty window. GetDesktopDisplayMode requires it */
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, 0);
			DisplayIndex = SDL_GetWindowDisplayIndex(window);
			SDL_GetDesktopDisplayMode(DisplayIndex, &Mode);
			
			/* We then set the borderless window to the desktop's resolution. 
			(It won't be configurable because well, that's the whole point of Borderless mode right ?) */
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Mode.w, Mode.h, SDL_WINDOW_BORDERLESS);
		break;
		case 2:
			/* Fullscreen desktop mode. For some reasons Windows 10 hates this.
			 * SDL_WINDOW_FULLSCREEN_DESKTOP was prompting a Windows 10 error so maybe detecting the
			 * resolution and then forcing fullscreen with the detected resolution will fix that.
			 * We need a better solution than that but SDL does not handle it gracefully...
			 * There seems to be no way of setting a width/height to a fullscreen resolution.
			 *  */
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, 0);
			DisplayIndex = SDL_GetWindowDisplayIndex(window);
			SDL_GetDesktopDisplayMode(DisplayIndex, &Mode);
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Mode.w, Mode.h, SDL_WINDOW_FULLSCREEN);
		break;
	}
	
	switch(sync_mode)
	{
		default:
			// No Vsync but with FPS limit
			lockfps_game = 1;
			vsync_flag = 0;
		break;
		case 1:
			// Vsync, no FPS lock
			lockfps_game = 0;
			vsync_flag = 0;
		break;
		case 2:
			// Vsync with FPS lock
			lockfps_game = 0;
			vsync_flag = 0;
		break;
		case 3:
			// No Vsync but Freesync
			/* Make sure to have proper drivers, especially on Linux.
			 * Note that you may have to force disable vsync if you want force adaptive sync
			 * accross everything :
			 * export adaptive_sync=true
			 * export vblank_mode=false
			 * 
			 * You can put this either under your .drirc file or export them as variables.
			 * 
			 * On Windows, this should work out of the box since Windows 7 as long as the drivers support it.
			 * Note that you will also need to run the game fullscren to take advantage of Gsync or freesync, like on Linux.
			 * On Nvidia, it's possible to also enable gsync for windowed mode as well, although i'm not sure if its recommended.
			 * 
			 * Of course since we don't have vsync, we should use our own FPS lock instead.
			 */
			lockfps_game = 1;
			vsync_flag = 0;
			
			if (screen_mode != 2)
			{
				printf("Warning : Windowed Freesync/Gsync might work on Windows but not on Linux !!!\n");
			}
			
			if (!SDL_GL_SetSwapInterval(-1))
			{
				printf("Freesync not supported or enabled. Renabling vsync since freesync isn't available.\n");
				vsync_flag = SDL_RENDERER_PRESENTVSYNC;
			}
		break;
		case 4:
			// Vsync + Freesync (Usually breaks and there should be no reason to use this)
			lockfps_game = 0;
			vsync_flag = SDL_RENDERER_PRESENTVSYNC;
			
			if (screen_mode != 2)
			{
				printf("Warning : Windowed Freesync/Gsync might work on Windows but not on Linux !!!\n");
			}
			
			if (!SDL_GL_SetSwapInterval(-1))
			{
				printf("Freesync not supported or enabled. Renabling Lock FPS again for that reason.\n");
				lockfps_game = 1;
			}
		break;
		
	}
	
	Get_Refresh_rate = SDLGetWindowRefreshRate(window);
	FPS_sleep = 1000.0 / Get_Refresh_rate;
	printf("Refresh rate is %d Hz\n", Get_Refresh_rate);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | vsync_flag);
	SDL_RenderSetLogicalSize(renderer, width, height);
	
	/* Clear everything on screen */
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	current_internal_resolution_width = width;
	current_internal_resolution_height = height;
	
	TTF_Init();
	
	/* Make sure uninitialized values are set to 0 */
	
	for(i=0;i<MAX_BUTTONS;i++)
	{
		Controller_Input.buttons[i] = 0;
		Controller_Input.timer_buttons[i] = 0;
	}
	
	for(i=0;i<4;i++)
		Controller_Input.dpad[i] = 0;
}

uint_fast8_t Load_Text_Font(uint_fast32_t a, const char* directory)
{
	char * pch;
	int font_size;
	uint32_t big = 0;
	unsigned long lengh_image = 0;
	unsigned long character_after = 0;
	
	if (!directory) return 0;
	
	/* Oh boy, i believe i have the vapors !
	 * We use double $$ in a sucession as to know 
	 * whenever our text file should be tagged as big or not.
	 * If this turns out to be a bad idea, then i might revert back to a descriptor.
	 * */
	pch = strrchr(directory,'$');
	if (pch != NULL)
	{
			lengh_image = strlen(directory);
			character_after = (unsigned long)(pch-(directory)+1)+1;
			if (!(character_after > lengh_image))
			{
				if (directory[character_after] == '$')
				{
					big = 1;
				}
			}
	}
	
	/* Hack for Ultra & Wider aspect ratios */
	/* TODO : Properly fix this according to the current aspect ratio instead of fixed magic numbers */
	if (current_internal_resolution_height * 2 < current_internal_resolution_width)
	{
		font_size = big ? current_internal_resolution_width / 30 : current_internal_resolution_width / 60;
	}
	else
	{
		font_size = big ? current_internal_resolution_width / 10 : current_internal_resolution_width / 30;
	}

	font[a] = TTF_OpenFont(directory, font_size);

	if (!font[a]) return 0;
	
	return 1;
}

uint_fast8_t Load_Titlescreen(const char* directory)
{
	SDL_Surface* tmp;
	
	if (!directory) return 0;
	
	if (titlescreen_data != NULL)
	{
		SDL_DestroyTexture(titlescreen_data);
	}
	
	tmp = IMG_Load(directory);
	if (!tmp) return 0;

	SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 255, 0, 255));
	SDL_SetSurfaceRLE(tmp, 1);
	
	titlescreen_data = SDL_CreateTextureFromSurface(renderer, tmp);
	SDL_FreeSurface(tmp);
	
	return 1;
}

uint_fast8_t Load_Image(uint_fast32_t p, uint_fast32_t a, const char* directory)
{
	SDL_Surface* tmp;
	
	if (!directory) return 0;
	
	if (img_struct[p].texture[a] != NULL)
	{
		SDL_DestroyTexture(img_struct[p].texture[a]);
	}
	
	tmp = IMG_Load(directory);
	if (!tmp) return 0;
	
	img_struct[p].width[a] = tmp->w;
	img_struct[p].height[a] = tmp->h;
	img_struct[p].current_frame[a] = 0;

	SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 255, 0, 255));
	SDL_SetSurfaceRLE(tmp, 1);
	
	img_struct[p].texture[a] = SDL_CreateTextureFromSurface(renderer, tmp);
	SDL_FreeSurface(tmp);
	
	return 1;
}


void Draw_Text(uint_fast32_t a, double top_left_x, double top_left_y, const char* text_to_write, uint8_t red, uint8_t green, uint8_t blue)
{
	SDL_Color color = { red, green, blue, 255 };
	SDL_Surface * surface = TTF_RenderText_Solid(font[a], text_to_write, color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	
	SDL_Rect position;
	
	SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
	position.x = (int) (current_internal_resolution_width * top_left_x);
	position.y = (int) (current_internal_resolution_height * top_left_y);
	
	SDL_RenderCopy(renderer, texture, NULL, &position);
	
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
} 

void Close_Font(uint_fast32_t a)
{
	TTF_CloseFont(font[a]);
}

void Close_All_Fonts(void)
{
	uint_fast32_t a;
	for(a=0;a<NUMBER_OF_FONTS;a++)
		TTF_CloseFont(font[a]);
}

void Put_titlescreen(void)
{
	SDL_Rect position;
	position.x = 0;
	position.y = 0;
	
	position.w = current_internal_resolution_width;
	position.h = current_internal_resolution_height;
	SDL_RenderCopy(renderer, titlescreen_data, NULL, &position);
}

void Put_background(int a)
{
	// -1 is pitch black. Since we are always clearing the screen black, ignore this function.
	if (a < 0) return;
	
	SDL_Rect position;
	position.x = 0;
	position.y = 0;
	
	position.w = current_internal_resolution_width;
	position.h = current_internal_resolution_height;

	SDL_SetTextureAlphaMod( img_struct[BACKGROUND_IMG].texture[a], 255 );
	SDL_RenderCopy(renderer, img_struct[BACKGROUND_IMG].texture[a], NULL, &position);
}

void Put_background_override(int a, uint8_t alpha)
{
	if (alpha < 1) return;
	SDL_Rect position;
	position.x = 0;
	position.y = 0;
	
	position.w = current_internal_resolution_width;
	position.h = current_internal_resolution_height;

	SDL_SetTextureAlphaMod( img_struct[BACKGROUND_IMG].texture[a], alpha );
	SDL_RenderCopy(renderer, img_struct[BACKGROUND_IMG].texture[a], NULL, &position);
}

void Put_image_top_left(int a, double top_left_x, double top_left_y, uint8_t alpha, double width, double height)
{
	if (alpha < 1) return;
	
	SDL_Rect position;
	position.x = (int) (current_internal_resolution_width * top_left_x);
	position.y = (int) (current_internal_resolution_height * top_left_y);
	position.w = (int) (current_internal_resolution_width * width);
	position.h = (int) (current_internal_resolution_height * height);
	
	SDL_SetTextureAlphaMod( img_struct[STATIC_IMG].texture[a], alpha );
	SDL_RenderCopy(renderer, img_struct[STATIC_IMG].texture[a], NULL, &position);
}

void Put_sprite_top_left(int a, double top_left_x, double top_left_y, int w_frame, int h_frame, int frame_start, int frame_end, int loop, double seconds, uint8_t alpha, double width, double height)
{
	if (alpha < 1) return;

	SDL_Rect position;
	SDL_Rect frame;
	
	position.x = (int) (current_internal_resolution_width * top_left_x);
	position.y = (int) (current_internal_resolution_height * top_left_y);
	position.w = (int) (current_internal_resolution_width * width);
	position.h = (int) (current_internal_resolution_height * height);
	
	if (!(img_struct[SPRITE_IMG].current_frame[a] >= (double)frame_end && loop == 0))
	{
		if (img_struct[SPRITE_IMG].current_frame[a] < frame_end)
		{
			/* 
			 * This function is executed every frame of the display refresh rate hence the division by 1.
			 * ( (Get_Refresh_rate * seconds) / (double)(frame_end-frame_start)) is the number of frame required to update each frame of the sprite
			*/
			img_struct[SPRITE_IMG].current_frame[a] += 1 / ( (Get_Refresh_rate * seconds) / (double)(frame_end-frame_start));
		}
		
		if ((img_struct[SPRITE_IMG].current_frame[a] >= (double)frame_end && loop == 1) || (img_struct[SPRITE_IMG].current_frame[a] < 0.0 && img_struct[SPRITE_IMG].current_frame[a] == 0.0))
		{
			img_struct[SPRITE_IMG].current_frame[a] = (double)frame_start;
		}
	}

	frame.x = 0;
	frame.y = (int)(img_struct[SPRITE_IMG].current_frame[a])*(h_frame);
	frame.w = w_frame;
	frame.h = h_frame;
	
	SDL_SetTextureAlphaMod( img_struct[SPRITE_IMG].texture[a], alpha );
	SDL_RenderCopy(renderer, img_struct[SPRITE_IMG].texture[a], &frame, &position);
}

void Reset_Sprite_Frame_Counter(int a)
{
	img_struct[SPRITE_IMG].current_frame[a] = 0;
}

int Return_Sprite_Width(int a)
{
	return img_struct[SPRITE_IMG].width[a];
}

int Return_Sprite_Height(int a)
{
	return img_struct[SPRITE_IMG].height[a];
}

double Return_Alpha_tick(double seconds)
{
	return 1 / ( (Get_Refresh_rate * seconds) / 255);
}

static uint_fast8_t Set_DPAD(uint_fast8_t value)
{
	switch(value)
	{
		case 0:
			return PRESSED;
		case 2:
			return RELEASED;
		default:
			return UNPRESSED;
	}
}

static void Controller_update(void)
{
	int i;
	SDL_Event event;

	/* Buttons detected as Released must be reset to 0 */
	for(i=0;i<MAX_BUTTONS;i++)
	{
		if (Controller_Input.buttons[i] == 3) Controller_Input.buttons[i] = 0;
	}
	
	/* Due to the nature of the dpad*/
	for(i=0;i<4;i++)
	{
		if (Controller_Input.dpad[i] == 3) Controller_Input.dpad[i] = 0;
	}
	
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				zero_toexit_program = 0;
				break;
			}
			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode)
				{
					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_W:
					Controller_Input.dpad[0] = Set_DPAD(Controller_Input.dpad[0]);
					break;
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_S:
					Controller_Input.dpad[1] = Set_DPAD(Controller_Input.dpad[1]);
					break;
					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_A:
					Controller_Input.dpad[2] = Set_DPAD(Controller_Input.dpad[2]);
					break;
					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_D:
					Controller_Input.dpad[3] = Set_DPAD(Controller_Input.dpad[3]);
					break;
					case SDL_SCANCODE_SPACE:
					case SDL_SCANCODE_KP_SPACE:
					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_KP_ENTER:
						if (Controller_Input.buttons[CROSS_BUTTON] != HELD)
						Controller_Input.buttons[CROSS_BUTTON] = PRESSED;
					break;
					case SDL_SCANCODE_ESCAPE:
						zero_toexit_program = 0;
					break;
					default:
					break;
				}
			break;

			case SDL_KEYUP:
				switch(event.key.keysym.scancode)
				{
					case SDL_SCANCODE_UP:
					case SDL_SCANCODE_W:
					Controller_Input.dpad[0] = RELEASED;
					break;
					case SDL_SCANCODE_DOWN:
					case SDL_SCANCODE_S:
					Controller_Input.dpad[1] = RELEASED;
					break;
					case SDL_SCANCODE_LEFT:
					case SDL_SCANCODE_A:
					Controller_Input.dpad[2] = RELEASED;
					break;
					case SDL_SCANCODE_RIGHT:
					case SDL_SCANCODE_D:
					Controller_Input.dpad[3] = RELEASED;
					break;
					case SDL_SCANCODE_SPACE:
					case SDL_SCANCODE_KP_SPACE:
					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_KP_ENTER:
						Controller_Input.buttons[CROSS_BUTTON] = RELEASED;
					break;
					default:
					break;
				}
			break;

			case SDL_CONTROLLERAXISMOTION:
			{
				//printf("Axis id %d, %d\n", event.caxis.axis, event.caxis.value);
				/* TODO : Improve DPAD emulation code */
				if (event.caxis.axis == 0)
				{
					if (event.caxis.value < -DEADZONE)
					{
						if (Controller_Input.dpad[UP_DPAD] != HELD)
						{
							Controller_Input.dpad[UP_DPAD] = PRESSED;
							if (Controller_Input.dpad[DOWN_DPAD] == HELD) Controller_Input.dpad[DOWN_DPAD] = RELEASED;
							else Controller_Input.dpad[DOWN_DPAD] = UNPRESSED;
						}
					}
					else if (event.caxis.value > DEADZONE)
					{
						if (Controller_Input.dpad[DOWN_DPAD] != HELD)
						{
							if (Controller_Input.dpad[UP_DPAD] == HELD) Controller_Input.dpad[UP_DPAD] = RELEASED;
							else Controller_Input.dpad[UP_DPAD] = UNPRESSED;
							Controller_Input.dpad[DOWN_DPAD] = PRESSED;
						}
					}
					else
					{
						if (Controller_Input.dpad[DOWN_DPAD] == 2) Controller_Input.dpad[DOWN_DPAD] = RELEASED;
						else Controller_Input.dpad[DOWN_DPAD] = UNPRESSED;
						if (Controller_Input.dpad[UP_DPAD] == 2) Controller_Input.dpad[UP_DPAD] = RELEASED;
						else Controller_Input.dpad[UP_DPAD] = UNPRESSED;
					}
				}
				else if (event.caxis.axis == 1)
				{
					if (event.caxis.value < -DEADZONE)
					{
						if (Controller_Input.dpad[LEFT_DPAD] != HELD)
						{
							Controller_Input.dpad[LEFT_DPAD] = PRESSED;
							if (Controller_Input.dpad[RIGHT_DPAD] == HELD) Controller_Input.dpad[LEFT_DPAD] = RELEASED;
							else Controller_Input.dpad[RIGHT_DPAD] = UNPRESSED;
						}
					}
					else if (event.caxis.value > DEADZONE)
					{
						if (Controller_Input.dpad[RIGHT_DPAD] != HELD)
						{
							if (Controller_Input.dpad[LEFT_DPAD] == HELD) Controller_Input.dpad[LEFT_DPAD] = RELEASED;
							else Controller_Input.dpad[LEFT_DPAD] = UNPRESSED;
							Controller_Input.dpad[RIGHT_DPAD] = PRESSED;
						}
					}
					else
					{
						if (Controller_Input.dpad[RIGHT_DPAD] == HELD) Controller_Input.dpad[RIGHT_DPAD] = RELEASED;
						else Controller_Input.dpad[RIGHT_DPAD] = UNPRESSED;
						if (Controller_Input.dpad[LEFT_DPAD] == HELD) Controller_Input.dpad[LEFT_DPAD] = RELEASED;
						else Controller_Input.dpad[LEFT_DPAD] = UNPRESSED;
					}
				}
				break;
			}
			case SDL_CONTROLLERBUTTONDOWN:
				//printf("event.cbutton.button %d\n", event.cbutton.button);
				if (Controller_Input.buttons[event.cbutton.button] != HELD)
				Controller_Input.buttons[event.cbutton.button] = PRESSED;
			break;
			case SDL_CONTROLLERBUTTONUP:
				Controller_Input.buttons[event.cbutton.button] = RELEASED;
			break;
                
			case SDL_CONTROLLERDEVICEADDED:
			for(i=0;i<MAX_JOYSTICK;i++)
			{
				if ( SDL_IsGameController(i)) 
				{
					pad[i] = SDL_GameControllerOpen(i);
					if (pad[i]) 
					{
						joy[i] = SDL_GameControllerGetJoystick( pad[i] );
						instanceID[i] = SDL_JoystickInstanceID( joy[i] );
					}
				}
			}
			break;

		}
	}	

	for(i=0;i<MAX_BUTTONS;i++)
	{
		if (Controller_Input.buttons[i] == 1)
		{
			Controller_Input.timer_buttons[i]++;
			if (Controller_Input.timer_buttons[i] > 1)
			{
				Controller_Input.buttons[i] = 2;
				Controller_Input.timer_buttons[i] = 0;
			}
		}
	}
	
	for(i=0;i<4;i++)
	{
		if (Controller_Input.dpad[i] == 1)
		{
			Controller_Input.timer_dpad[i]++;
			if (Controller_Input.timer_dpad[i] > 1)
			{
				Controller_Input.dpad[i] = 2;
				Controller_Input.timer_dpad[i] = 0;
			}
		}
	}
	
}

void Sync_video(void)
{
	uint32_t start = 0;
	if (lockfps_game) start = SDL_GetTicks();
	SDL_RenderPresent(renderer);
	/* This is used to sleep and lock the FPS */
	if (lockfps_game)
	{
		if(FPS_sleep > SDL_GetTicks()-start) msleep(FPS_sleep-(SDL_GetTicks()-start));
	}
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	frames_FPS_calculation++;
	newTick = Timer_Read();
	if ( ((newTick) - (lastTick)) > 1000000) 
	{
		#ifdef LOG_FPS
		printf("FPS %d\n", frames_FPS_calculation);
		#endif
		frames_FPS_calculation = 0;
		lastTick = newTick;
	}
	
	Controller_update();
}

void Move_Position_X(double* x, double xVel)
{
	*x += xVel * ( 1.0/Get_Refresh_rate );
}

void Move_Position_Y(double* y, double yVel)
{
	*y += yVel * ( 1.0/Get_Refresh_rate );
}

void Close_Video(void)
{
	uint32_t a;
	for (a=0;a<NUMBER_OF_IMAGES;a++) 
	{
		if (img_struct[BACKGROUND_IMG].texture[a]) SDL_DestroyTexture(img_struct[BACKGROUND_IMG].texture[a]);
		if (img_struct[SPRITE_IMG].texture[a]) SDL_DestroyTexture(img_struct[SPRITE_IMG].texture[a]);
		if (img_struct[STATIC_IMG].texture[a]) SDL_DestroyTexture(img_struct[STATIC_IMG].texture[a]);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Init_sound(void)
{
	Mix_OpenAudio(48000,MIX_DEFAULT_FORMAT,2,2048);
	Mix_AllocateChannels(32);
}
		
void Clean_Music(void)
{
	if (music)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}
}

void Load_Music(const char* directory)
{
	music = Mix_LoadMUS(directory);
}

void Play_Music(uint_fast32_t loop)
{
	if (loop == 1) Mix_PlayMusic(music, -1);
	else Mix_PlayMusic(music, 0);
}

uint_fast32_t Load_SFX(uint_fast32_t i, const char* directory)
{
	if (sfx_id[i])
	{
		Mix_FreeChunk(sfx_id[i]);
		sfx_id[i] = NULL;
	}
	sfx_id[i] = Mix_LoadWAV(directory);
	if (!sfx_id[i]) return 0;
	return 1;
}

void Play_SFX(uint_fast32_t i)
{
	Mix_PlayChannel(-1, sfx_id[i], 0) ;
}

void Unload_SFX(void)
{
	uint_fast32_t i;

	for (i=0;i<MAX_SFX;i++) 
	{
		if (sfx_id[i])
		{
			Mix_FreeChunk(sfx_id[i]);
			sfx_id[i] = NULL;
		}
	}
}

void Load_Voice(const char* directory)
{
	if (voice_sfx)
	{
		Mix_FreeChunk(voice_sfx);
		voice_sfx = NULL;
	}
	voice_sfx = Mix_LoadWAV(directory);
}

void Play_Voice(void)
{
	/* Use the last channel for voices */
	Mix_PlayChannel(15, voice_sfx, 0);
}

void Stop_Voice(void)
{
	if (voice_sfx) Mix_HaltChannel(15);
}

void Unload_Voice(void)
{
	if (voice_sfx)
	{
		Mix_FreeChunk(voice_sfx);
		voice_sfx = NULL;
	}
}
