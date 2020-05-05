
/*
0| Background number to display. -1 for pitch black

0| Special action to trigger. This can be a mini-game, the screen shaking, screen fadein...
* 
* 2 : Screen going to black after space is pressed.
* 1 : Screen going from black when entered

0| Character to display on screen (sprite number)
0| First sprite number to display before looping to the frame set before. 0 to disable.
0| End frame for sprite 1.
0| End frame for sprite 2. (only if the 2nd option is enabled)
0| Time it needs to display up until the last frame (100 is one second)
0| Size on screen in width (100 is the max or 1.0f)
0| Size on screen in height (100 is the max or 1.0f)

0| 2nd character to display on screen
0| First sprite number to display before looping to the frame set before. 0 to disable.
0| End frame for sprite 1.
0| End frame for sprite 2. (only if the 2nd option is enabled)
0| Time it needs to display up until the last frame (100 is one second)
0| Size on screen in width (100 is the max or 1.0f)
0| Size on screen in height (100 is the max or 1.0f)

0| If there's a choice to be made. 0 for none, 1 for yes. (Two paths possible)

0| Is only possible if the previous paramater is set to 1. The story position inside of the array for Choice 1.
0| Is only possible if the second previous paramter is set to 1. The story position inside of the array for Choice 2.

\n is a line break and tells the interpreter to display the number on the next line of text.
It is a special character also used in other languages like HTML.

*/

#include "array.h"

char story_text[5192][256] = {
	{"0|0|-1|0|0|0|0|0|0|-1|0|0|0|0|0|0|0|0|0|The year is 2008.\nI was at home watching some TV shows.\nYou know, the usual boring stuff..."},
	{"0|0|-1|0|0|0|0|0|0|-1|0|0|0|0|0|0|0|0|0|Suddenly however, i heard a loud knock at my door.\nI was a little scared but did walk towards my door."},
	{"1|0|0|0|9|0|900|20|50|-1|0|0|0|0|0|0|0|0|0|When i opened it,\nthere were 3 muscular looking men in front of me."},
	{"1|0|0|0|9|0|900|20|50|-1|0|0|0|0|0|0|0|0|0|0|Man :\nHello sir, how are you doing ?"},
	{"1|0|0|0|9|0|900|20|50|-1|0|0|0|0|0|0|0|0|0|Me :\nI'm fine. What brings you here ?"}
};
