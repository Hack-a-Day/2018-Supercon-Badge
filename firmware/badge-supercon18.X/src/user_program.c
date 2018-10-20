/************************************
 * This is the framework for those
 * who wish to write their own C
 * code for the basic badge
 ************************************/

/*Moving tiles

shuffle_array[10]
tile_array[10]

cursory is 0..1
cursorx is 9..0

if cursory is zero
  moving left or right
    tile_array[cursorx] will be shifted left or right (opposite whichever was pushed)
    value 10 will be inserted in tile_array
  moving down
    inserts value from shuffle_array in to tile_array
 
else
  moving left or right
    shifts cursorx but doesn't affect either array
  moving up
    value from tile array is placed in shuffle array
    tile array position is replaced with 10

*/

#include "badge_user.h"

#define SHUFFLE_ROW		3
#define TILE_ROW		4
#define SCORE_ROW		7
#define EMPTY			10
#define POINT			11
#define RIGHT			1
#define LEFT			-1

void user_program_init(void)
	{
	/* This will run when User Program is first selected form the menu */
	clr_buffer();
	video_gotoxy(5,0);
    stdio_write("Moving Tiles");
	}

void draw_tile(uint8_t tiles[], uint8_t row)
	{
	uint8_t i;
	uint8_t str[2] = { ' ',0};
	video_gotoxy(10,row);
	for (i=0; i<10; i++) {
		if (tiles[i] == EMPTY) stdio_write(" ");
		else if (tiles[i] == POINT) stdio_write("v");
		else
			{
			str[0] = tiles[i]+'0';	//hack to turn ints to strings
			stdio_write(str);
			}
		}
	}

void draw_score(uint8_t scores[])
	{
	uint8_t i;
	uint8_t str[2] = { ' ',0};
	//video_gotoxy(10,7);
	for (i=0; i<10; i++)
		{
		if (scores[i] > 0) color_buffer[SCORE_ROW][10+i] = 0x2F;
		else color_buffer[SCORE_ROW][10+i] = 0x4F;
		if (i==9) str[0] = '0';
		else str[0] = i+1+'0';
		//stdio_write(str);
		disp_buffer[SCORE_ROW][10+i] = str[0];
		}
	}

void blank_score(void)
	{
	uint8_t i;
	uint8_t str[2] = { ' ',0};
	for (i=0; i<10; i++)
		{
		color_buffer[SCORE_ROW][10+i] = 0x0F;
		if (i==9) str[0] = '0';
		else str[0] = i+1+'0';
		disp_buffer[SCORE_ROW][10+i] = str[0];
		}
	}

void shift(uint8_t tiles[], uint8_t shuffles[], uint8_t newIdx, int8_t direction)
	{
	uint8_t oldIdx = newIdx-direction;
	tiles[oldIdx] = tiles[newIdx];
	tiles[newIdx] = EMPTY;
	
	shuffles[newIdx] = shuffles[oldIdx];
	shuffles[oldIdx] = EMPTY;
	}

void move_cursor(uint8_t tiles[], uint8_t shuffles[], uint8_t newIdx, int8_t direction)
	{
	uint8_t oldIdx = newIdx-direction;
	
	shuffles[newIdx] = shuffles[oldIdx];
	shuffles[oldIdx] = EMPTY;
	}

void place(uint8_t shuffles[], uint8_t tiles[], uint8_t idx)
	{
	tiles[idx] = shuffles[idx];
	shuffles[idx] = POINT;
	}

void remove(uint8_t shuffles[], uint8_t tiles[], uint8_t idx)
	{
	shuffles[idx] = tiles[idx];
	tiles[idx] = EMPTY;
	}

void score(uint8_t scores[], uint8_t tiles[])
	{
	uint8_t i;
	uint32_t num = tiles[0];
	if (num == 0)
		{
		for (i=0; i<10; i++) scores[i] = 0;
		return;
		}
	else scores[0] = 1;
	for (i=1; i<10; i++)
		{
		num = (num*10)+tiles[i];
		if (num%(i+1) == 0) scores[i] = 1;
		else scores[i] = 0;
		}
	}

void redraw_screen(uint8_t shuffles[], uint8_t tiles[], uint8_t scores[], uint8_t y)
	{
	draw_tile(shuffles, SHUFFLE_ROW);
	draw_tile(tiles, TILE_ROW);
	if (y)
		{
		score(scores, tiles);
		draw_score(scores);
		}
	else blank_score();
	}

uint8_t ftw(uint8_t scores[])
	{
	uint8_t i;
	for (i=0; i<10; i++)
		{
		if (scores[i] == 0) return 0;
		}
	return 1;
	}

void user_program_loop(void)
	{
	/* This should contain user program (will be run after init function)*/
	
	uint8_t cursory = 0;
	uint8_t cursorx = 5;
	uint8_t shuffle_array[] = {EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,5,EMPTY,EMPTY,EMPTY,EMPTY};
	uint8_t tile_array[] = {0,1,2,3,4,EMPTY,6,7,8,9};
	uint8_t score_array[] = {0,0,0,0,0,0,0,0,1,0};

	redraw_screen(shuffle_array, tile_array, score_array, cursory);

	
	while(1) //Loop forever
		{

		/* React to user input */
		uint8_t get_stat, char_out;
		get_stat = stdio_get(&char_out);
		if (get_stat!=0)
			{
			/* Show which letter was pressed just as an example: */
			/* Directly address color_buffer: first nibble is BG, second nibble is FG*/
			if ((char_out==K_LT) && (cursorx != 0))
				{
				if (cursory == 1)
					{
					move_cursor(tile_array, shuffle_array, --cursorx, LEFT);
					redraw_screen(shuffle_array, tile_array, score_array, cursory);
					}
				else
					{
					shift(tile_array, shuffle_array, --cursorx, LEFT);
					redraw_screen(shuffle_array, tile_array, score_array, cursory);
					}
				}
			
			if ((char_out==K_RT) && (cursorx != 9))
				{
				if (cursory == 1)
					{
					move_cursor(tile_array, shuffle_array, ++cursorx, RIGHT);
					redraw_screen(shuffle_array, tile_array, score_array, cursory);
					}
				else
					{
					shift(tile_array, shuffle_array, ++cursorx, RIGHT);
					redraw_screen(shuffle_array, tile_array, score_array, cursory);
					}
				}
			if ((char_out==K_DN) && (cursory == 0))
				{
				cursory = 1;
				place(shuffle_array, tile_array, cursorx);
				redraw_screen(shuffle_array, tile_array, score_array, cursory);
				}
			if ((char_out==K_UP) && (cursory == 1))
				{
				cursory = 0;
				remove(shuffle_array, tile_array, cursorx);
				redraw_screen(shuffle_array, tile_array, score_array, cursory);
				}
			if (ftw(score_array))
				{
				video_gotoxy(10,9);
				stdio_write("Winner! Winner!");
				}
			}
		}
	}

