/************************************
 * This is the puzzle. Can you solve it?
 ************************************/

#include "badge_user.h"

void draw_header(uint8_t status);

void puzzle_init(void)
	{
	/* This will run when User Program is first selected form the menu */
	set_cursor_state (0);
	clr_buffer();
	draw_header(0);
	}

void draw_header(uint8_t status) {
	video_set_color(15,2);
	video_gotoxy(0,9);
    uint8_t i;
	for (i=0; i<120; i++) stdio_c(' ');
	video_gotoxy(1,9);
	//We just want to keep the puzzle secret until someone solves it.
	//See the official .hex file to reflash badge with puzzle intact
	stdio_write("Fake Source Until After Supercon");
	video_gotoxy(1,11);
	stdio_write("Reflash official .hex for puzzle");
	}

void puzzle_loop(void)
	{
	/* This should contain user program (will be run after init function)*/
	while(1) { ;; }
	}


