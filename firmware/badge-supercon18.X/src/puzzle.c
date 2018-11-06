/************************************
 * This is the puzzle. Can you solve it?
 ************************************/

#include "badge_user.h"

#define SHUFFLE_ROW		4
#define TILE_ROW		8
#define TILE_OFFSETX	1
#define TILE_SIZEX		4
#define SCORE_ROW		16
#define EMPTY			10
#define POINT			11
#define RIGHT			1
#define LEFT			-1

#define SOLVED			0
#define UNSOLVED		1
#define NEUTRAL			2
#define CARDS			3
#define POINTER			4


/* prototypes */
void draw_header(uint8_t status);



const uint8_t tile_colors[][2] = 
	{
		{ 15,2 },
		{ 15,4 },
		{ 15,8 },
		{ 15,9 },
		{ 10,0 },
	};

const uint8_t glyph[] = 
	{
	0b00000000,
	0b00110000,
	0b00111100,
	0b00110001,
	0b00110010,
	0b00010000,
	0b00111000,
	0b00001111,
	0b00011111,
	0b00010010,
	0b00111010,
	0b00111001,
	0b00100110,
	0b00110100,
	0b00000110,
	0b00010100,
	0b00100011,
	0b00100111,
	0b00010110,
	0b00100010,
	0b00000010,
	0b00000111,
	0b00101010,
	0b00001110,
	0b00101111,
	0b00010101,
	0b00111011,
	0b00011100,
	0b00110011,
	0b00011101,
	0b00101011,
	0b00011110,
	0b00111101,
	0b00110101,
	0b00010111,
	0b00110110,
	0b00101110,
	0b00110111,
	0b00111110 
	};

const uint8_t cypher0[19] = { 8, 21, 2, 15, 17, 4, 21, 23, 22, 2, 8, 26, 1, 26, 20, 4, 23, 23, 35 };
const uint8_t cypher1[19] = { 4, 28, 1, 14, 6, 7, 18, 3, 13, 1, 28, 8, 5, 21, 25, 4, 10, 17, 22 };

const uint8_t puzzle_messages[2][14] = { " Use Arrows  ", "Winner Winner" };

void glyph_line(uint8_t data)
	{
	uint8_t i;
	for (i=0; i<8; i++)
		{
		if (data & 1<<(7-i)) TFT_24_7789_Write_Data3(0xFF,0xFF,0xFF);
		else TFT_24_7789_Write_Data3(0,0,0);
		}
	}
void draw_glyph(uint8_t idx, uint16_t x, uint16_t y)
	{
	uint8_t font[2] = {0,0};
	
	uint8_t i;
	for (i=0; i<3; i++)
		{
		if (glyph[idx] & (1<<i))
			{
			font[0] |= 1<<(i*3);
			font[0] |= 1<<((i*3)+1);
			}
		if (glyph[idx] & (1<<(i+3)))
			{
			font[1] |= 1<<(i*3);
			font[1] |= 1<<((i*3)+1);
			}
		}
	tft_set_write_area(x,y,7,4);
	TFT_24_7789_Write_Command(0x2C);
    //FIXME: Why do I need these +1 adjustments. Off-by-one in tft_set_write_area?
	glyph_line(font[1]);
	glyph_line(font[1]);
	glyph_line(0);
	glyph_line(font[0]);
	glyph_line(font[0]);
	}

void puzzle_init(void)
	{
	/* This will run when User Program is first selected form the menu */
	set_cursor_state (0);
	clr_buffer();
	draw_header(0);
	}

void repeat_char(uint8_t times, uint8_t character)
	{
	uint8_t i;
	for (i=0; i<times; i++) stdio_c(character);
	}

void draw_single_tile(uint8_t x, uint8_t y, uint8_t letter, uint8_t colorIdx)
	{
	video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y);
	video_set_color(tile_colors[colorIdx][0],tile_colors[colorIdx][1]);
	stdio_c(201); stdio_c(205); stdio_c(187);
	video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y+1);
	stdio_c(186); stdio_c(letter); stdio_c(186);
	//video_set_color(16,tile_colors[colorIdx][0]);
	
	//video_set_color(tile_colors[colorIdx][0],tile_colors[colorIdx][1]);
	
	video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y+2);
	stdio_c(200); stdio_c(205); stdio_c(188);
	}

void erase_single_tile(uint8_t x, uint8_t y)
	{
	uint8_t i;
	video_set_color(0,0);
	for (i=0; i<3; i++)
		{
		video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y+i);
		repeat_char(3,' ');
		}
	}

void draw_pointer(uint8_t x, uint8_t y, uint8_t colorIdx)
	{
	video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y);
	video_set_color(tile_colors[colorIdx][0],tile_colors[colorIdx][1]);
	stdio_write(" | ");
	video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y+1);
	stdio_write(" | ");
	video_gotoxy(TILE_OFFSETX+(x*TILE_SIZEX),y+2);
	stdio_write(" v ");
	}
	
void draw_all_tiles(uint8_t tiles[], uint8_t row)
	{
	uint8_t i;
	uint8_t str[2] = { ' ',0};
	video_gotoxy(10,row);
	for (i=0; i<10; i++) {
		if (tiles[i] == EMPTY) erase_single_tile(i,row);
		else if (tiles[i] == POINT) draw_pointer(i,row,POINTER);
		else
			{
			draw_single_tile(i,row,tiles[i]+'0',CARDS);
			}
		}
	}

void draw_score(uint8_t scores[])
	{
	uint8_t i;
	for (i=0; i<10; i++)
		{
		if (scores[i] > 0) draw_single_tile(i,SCORE_ROW,(i+1)%10+'0',SOLVED);
		else draw_single_tile(i,SCORE_ROW,(i+1)%10+'0',UNSOLVED);
		}
	}

void draw_header(uint8_t status) {
	if (status)
		{
		video_set_color(15,2);
		}
	else
		{
		video_set_color(15,4);
		}
	video_gotoxy(0,0);
    uint8_t i;
	for (i=0; i<120; i++) stdio_c(' ');
	video_gotoxy(13,1);
	stdio_write((char *)puzzle_messages[status]); //This (char *) casting hack is stupid
	}
void blank_score(void)
	{
	uint8_t i;
	for (i=0; i<10; i++)
		{
		draw_single_tile(i,SCORE_ROW,(i+1)%10+'0',NEUTRAL);
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

void remove_tile(uint8_t shuffles[], uint8_t tiles[], uint8_t idx)
	{
	shuffles[idx] = tiles[idx];
	tiles[idx] = EMPTY;
	}

void score(uint8_t scores[], uint8_t tiles[])
	{
	uint8_t i;
	uint64_t num = tiles[0];
	if (num == 0) scores[0] = 0;
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
	draw_all_tiles(shuffles, SHUFFLE_ROW);
	draw_all_tiles(tiles, TILE_ROW);
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

void puzzle_loop(void)
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
				remove_tile(shuffle_array, tile_array, cursorx);
				redraw_screen(shuffle_array, tile_array, score_array, cursory);
				}
			if (ftw(score_array))
				{
				draw_header(1);
				//Make sure we get one last refresh
				uint32_t now = millis();
				while(millis()-now < 800) { ;; }
				enable_display_scanning(0);	//Turn off display scanning
				uint8_t i;
				for (i=0; i<19; i++)
					{
					draw_glyph(cypher0[i]-tile_array[i%10],(i*16)+16,154);
					//draw_glyph(i,(i*16)+16,154);
					draw_glyph(cypher1[i]-tile_array[i%10],(i*16)+16,166);
					}
				while(1) { ;; }
				}
			}
		}
	}


