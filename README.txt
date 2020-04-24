Small VN engine for Visual novel games
=======================================

It is also somewhat flexible for static screen games like some Arcadey or Visual Novel games.

I wouldn't recommend this for a game that requires a scrolling level though as it's not suitable and flexible enough for that.
(You will get breakage especially if multiple resolutions or aspect ratios are involved)

Done
====

- High refresh rate support (use the Move functions to move images)
- Multiple resolutions and aspect ratios (ultra wide resolutions not properly supported though)
- Able to draw pictures, a sprite from a spritesheet and some text on-screen
- Uses SDL2 meaning it's portable to other devices with an SDL2 port available.

TODO
====

- Input (I think i'll just not allow users to type in their name and pretend the keyboard is a controller)
- Change font used for final version (it's proprietary and its license is restrictive)
- Properly support Ultra-wide resolutions (still works for now but we need a better solution)
- SDL 1.2 backend for OpenDingux devices. (the abstraction works good enough for this)
- PS3 port with NoRSX or GXM library ?
- Other platform specific port (Sega 32X, Sony PSP, Xbox original, PS2 etc...)
