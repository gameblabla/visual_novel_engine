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

#define NUMBER_OF_IMAGES 1024
#define MAX_JOYSTICK 16
#define MAX_SFX 128
#define NUMBER_OF_FONTS 64

static uint32_t current_internal_resolution_width = 0;
static uint32_t current_internal_resolution_height = 0;

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture_library_memory[NUMBER_OF_IMAGES];
uint_fast32_t sprites_w_[NUMBER_OF_IMAGES];
uint_fast32_t sprites_h_[NUMBER_OF_IMAGES];

Mix_Music* music;
Mix_Chunk* sfx_id[MAX_SFX];

SDL_GameController *pad[MAX_JOYSTICK];
SDL_Joystick *joy[MAX_JOYSTICK];
uint32_t instanceID[MAX_JOYSTICK];

uint32_t zero_toexit_program = 1;

/* Used to lock the FPS not via Vsync but internally */
uint32_t lockfps_game = 0;

/* FPS calculation */
static uint32_t Timer_Read(void) 
{
	/* Timing. */
	struct timeval tval;
  	gettimeofday(&tval, 0);
	return (((tval.tv_sec*1000000) + (tval.tv_usec)));
}
static long lastTick = 0, newTick;
static uint32_t frames_FPS_calculation = 0;

/* Used to detect and lock the framerate */
uint32_t Get_Refresh_rate = 0;
double FPS_sleep;

/* Text FONTs */
TTF_Font * font[NUMBER_OF_FONTS];


void msleep(uint32_t milisec)
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
		SDL_Delay(milisec);
	#endif
}

uint32_t SDLGetWindowRefreshRate(SDL_Window *Window)
{
    SDL_DisplayMode Mode;
    uint32_t DisplayIndex = SDL_GetWindowDisplayIndex(Window);
    
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
    return Mode.refresh_rate;
}

void Init_Video(const char* title, uint_fast32_t width, uint_fast32_t height, uint_fast32_t screen_mode)
{
	uint32_t real_window_mode;
	switch(screen_mode)
	{
		case 0:
			real_window_mode = SDL_WINDOW_SHOWN;
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, width, height, real_window_mode);
		break;
		case 1:
			real_window_mode = SDL_WINDOW_BORDERLESS;
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 0, 0, real_window_mode);
		break;
		case 2:
			real_window_mode = SDL_WINDOW_FULLSCREEN;
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 0, 0, real_window_mode);
		break;
	}
	
	Get_Refresh_rate = SDLGetWindowRefreshRate(window);
	FPS_sleep = 1000.0 / Get_Refresh_rate;
	printf("Get_Refresh_rate %d\n", Get_Refresh_rate);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(renderer, width, height);
	
	/* Clear everything on screen */
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	current_internal_resolution_width = width;
	current_internal_resolution_height = height;
	
	TTF_Init();
}

uint_fast8_t Load_Text_Font(uint_fast32_t a, const char* directory, uint32_t big)
{
	uint32_t font_size;
	
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
}

uint_fast8_t Load_Image(uint_fast32_t a, const char* directory)
{
	SDL_Surface* tmp;
	
	if (texture_library_memory[a] != NULL)
	{
		SDL_DestroyTexture(texture_library_memory[a]);
	}
	
	tmp = IMG_Load(directory);
	if (!tmp) return 1;
	
	sprites_w_[a] = tmp->w;
	sprites_h_[a] = tmp->h;

	SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 255, 0, 255));
	SDL_SetSurfaceRLE(tmp, 1);
	
	texture_library_memory[a] = SDL_CreateTextureFromSurface(renderer, tmp);
	SDL_FreeSurface(tmp);
	
	return 0;
}


void Draw_Text(uint_fast32_t a, double top_left_x, double top_left_y, const char* text_to_write, uint8_t red, uint8_t green, uint8_t blue)
{
	SDL_Color color = { red, green, blue };
	SDL_Surface * surface = TTF_RenderText_Solid(font[a], text_to_write, color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	
	SDL_Rect position;
	
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
	position.x = current_internal_resolution_width * top_left_x;
	position.y = current_internal_resolution_height * top_left_y;
	
	SDL_RenderCopy(renderer, texture, NULL, &position);
	
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
} 

void Close_Font(uint_fast32_t a)
{
	TTF_CloseFont(font[a]);
}

void Close_All_Fonts()
{
	uint_fast32_t a;
	for(a=0;a<NUMBER_OF_FONTS;a++)
		TTF_CloseFont(font[a]);
}


void Put_image_top_left(uint_fast32_t a, double top_left_x, double top_left_y)
{
	SDL_Rect position;
	position.x = current_internal_resolution_width * top_left_x;
	position.y = current_internal_resolution_height * top_left_y;
	
	position.w = sprites_w_[a];
	position.h = sprites_h_[a];
	SDL_RenderCopy(renderer, texture_library_memory[a], NULL, &position);
}

void Put_sprite_top_left(uint_fast32_t a, double top_left_x, double top_left_y, uint_fast32_t w, uint_fast32_t h, uint_fast32_t f)
{
	SDL_Rect position;
	position.x = current_internal_resolution_width * top_left_x;
	position.y = current_internal_resolution_height * top_left_y;
	position.w = w;
	position.h = h;
	
	SDL_Rect frame;
	frame.x = f*w;
	frame.y = 0;
	frame.w = w;
	frame.h = h;
	
	SDL_RenderCopy(renderer, texture_library_memory[a], &frame, &position);
}

void Sync_video()
{
	uint32_t start;
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
}

void Move_Position_X(double* x, double xVel)
{
	*x += xVel * ( 1.0f/Get_Refresh_rate );
}

void Move_Position_Y(double* y, double yVel)
{
	*y += yVel * ( 1.0f/Get_Refresh_rate );
}

void Close_Video()
{
	uint32_t i;
	for (i=0;i<NUMBER_OF_IMAGES;i++) 
	{
		if (texture_library_memory[i]) SDL_DestroyTexture(texture_library_memory[i]);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Controller_update()
{
	uint32_t i;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				zero_toexit_program = 0;
				break;
			}
			case SDL_CONTROLLERAXISMOTION:
			{
				printf("Axis %d\n", event.caxis);
				break;
			}
			case SDL_CONTROLLERBUTTONDOWN:
				printf("sdlEvent.cbutton Down %d\n", event.button);
			break;
			case SDL_CONTROLLERBUTTONUP:
				printf("sdlEvent.cbutton Up %d\n", event.button);
			break;
                
			case SDL_CONTROLLERDEVICEADDED:
			for(i=0;i<MAX_JOYSTICK;i++)
			{
				if ( SDL_IsGameController(i)) 
				{
					pad[i] = SDL_GameControllerOpen(i);
					printf("LOL\n");
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
}

void Init_sound()
{
	Mix_OpenAudio(48000,MIX_DEFAULT_FORMAT,2,2048);
	Mix_AllocateChannels(32);
}
		
void Clean_Music()
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

void Load_SFX(uint_fast32_t i, const char* directory)
{
	if (sfx_id[i])
	{
		Mix_FreeChunk(sfx_id[i]);
		sfx_id[i] = NULL;
	}
	sfx_id[i] = Mix_LoadWAV(directory);
}

void Play_SFX(uint_fast32_t i)
{
	Mix_PlayChannel(-1, sfx_id[i], 0) ;
}

void Unload_SFX()
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