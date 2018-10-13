#include "splash.h"
#include <stdint.h>

const uint32_t arc[25] = {
  0b00000000000000000000000000001111,
  0b00000000000000000000000011111111,
  0b00000000000000000000011111111111,
  0b00000000000000000001111111111111,
  0b00000000000000000011111111111111,
  0b00000000000000001111111111111111,
  0b00000000000000011111111111111111,
  0b00000000000000111111111111111111,
  0b00000000000001111111111111111111,
  0b00000000000011111111111111111100,
  0b00000000000011111111111111000000,
  0b00000000000111111111111100000000,
  0b00000000001111111111110000000000,
  0b00000000001111111111100000000000,
  0b00000000011111111111000000000000,
  0b00000000011111111110000000000000,
  0b00000000011111111110000000000000,
  0b00000000111111111100000000000000,
  0b00000000111111111100000000000000,
  0b00000000111111111000000000000000,
  0b00000000111111111000000000000000,
  0b00000001111111111000000000000000,
  0b00000001111111111000000000000000,
  0b00000001111111110000000000000000,
  0b00000001111111110000000000000000
};

extern uint8_t ram[65536L];
//uint32_t shadow[2400];

#define SPLASH_DELAY	100
#define SPLASH_X_LOC	52
#define SPLASH_Y_LOC	17

/*** Orientations ***
  0 = top left arc
  1 = top right arc
  2 = bottom right arc
  3 = bottom left arc
********************/

/*** Offsets ***
  0 = Top down, bits 24 through 0
  1 = bits 24 through 0, bottom up
  2 = bottom up, bits 0 through 24
  3 = bottom up, bits 24 through 0
***************/

// H  arc (52,17,0) (76,17,1) box (52,12,60,60) (92,37,100,60)
// A  arc (110,12,0) (134,12,1) (134,36,2) (110,36,3) box (149,36,157,60)
// C  arc (167,17,0) (167,36,3) box (190,12,215,20) (190,52,215,60)
// K  arc (248,17,2) (248,36,1) bot (225,12,233,60)
// A  arc (52,70,0) (76,70,1) (134,94,2) (52,94,3) box (92,94,100,118)
// D  arc (110,70,0) (134,70,1) (134,94,2) (110,36,3) box (149,70,157,93)
// A  arc (167,70,0) (191,70,1) (191,94,2) (167,36,3) box (207,94,215,118)
// Y  arc (225,70,3) (249,70,2) box (244,90,252,118)

// S arc (52,128,0) (76,152,2)
// U arc (110,152,3) (134,152,2) box (110,128,8,24) (150,128,8,24) 
// P arc (167,128,0) (191,128,1) (191,152,2) (167,152,3) box (167,152,8,25)
// E arc (225,128,0) (249,128,1) (225,152,3)
// R arc (52,186,0) (76,186,1) box (52,210,8,25)
// C arc (110,186,0) (110,210,3) box (134,186,24,8) (134,227,24,8)
// O arc (167,186,0) (191,186,1) (191,210,2) (167,210,3)
// N arc (225,186,0) (249,186,1) box (225,210,8,25) (266,210,8,25)

void draw_and_record_pixel(uint16_t actualx, uint16_t actualy, uint32_t back)
	{
	//actualx += SPLASH_X_LOC;
	//actualy += SPLASH_Y_LOC;
	ram[(actualy*40)+(actualx/8)] |= (1<<(actualx%8));
	TFT_24_7789_Write_Data3((back>>16)&0xFF,(back>>8)&0xFF,(back>>0)&0xFF);
	}

void draw_and_record_box(uint16_t actualx, uint16_t actualy, uint16_t width, uint16_t height, uint32_t back)
	{
	tft_fill_area(actualx, actualy, width, height, back);
	uint16_t i,j;
	for (i=0; i<height; i++)
		{
		for (j=0; j<width; j++)
			{
			ram[((actualy+i)*40)+((actualx+j)/8)] |= (1<<((actualx+j)%8));
			}
		}
	}
void draw_bg_or_not (uint16_t actualx, uint16_t actualy)
	{
	//actualx += SPLASH_X_LOC;
	//actualy += SPLASH_Y_LOC;
	if (ram[(actualy*40)+(actualx/8)] & (1<<(actualx%8))) TFT_24_7789_Write_Data3(0x16,0xae,0xef);
	else TFT_24_7789_Write_Data3(0,0,0);
	}

void draw_arc (uint16_t x, uint16_t y, uint8_t rotate, uint32_t back)
{
  uint16_t i,j;
  tft_set_write_area(x,y,24,24);
  TFT_24_7789_Write_Command(0x2C);


  //FIXME: Why do I need these +1 adjustments. Off-by-one in tft_set_write_area?
  //for (i=0; i<((xlen+1)*(ylen+1)); i++)
  for (i=0; i<25; i++)
	{
	for (j=0; j<25; j++)
		{
		//TODO: Test for arc 1 or 0 and place color pixel as necessary
		if (rotate == 0)
			{
			if (arc[i] & 1<<(24-j)) draw_and_record_pixel(x+i,y+j,back);
			else draw_bg_or_not(x+i,y+j);
			}
		else if (rotate == 1)
			{
			if (arc[24-j] & 1<<(24-i)) draw_and_record_pixel(x+j,y+i,back);
			else draw_bg_or_not(x+j,y+i);
			}
		else if (rotate == 2)
			{
			if (arc[24-i] & 1<<(j)) draw_and_record_pixel(x+i,y+j,back);
			else draw_bg_or_not(x+i,y+j);
			}
		else if (rotate == 3)
			{
			if (arc[j] & 1<<(i)) draw_and_record_pixel(x+j,y+i,back);
			else draw_bg_or_not(x+j,y+i);
			}
		}
		//TFT_24_7789_Write_Data3((back>>16)&0xFF,(back>>8)&0xFF,(back>>0)&0xFF);
    }
}

struct Cipher_box
	{
    int16_t x;	    // Pixel location on screen (negative values for left overflow)
    uint8_t row; // Which of 12 columns in the splash logo this belongs to
	};

void show_splash(void)
{
    uint16_t i,j;
    tft_fill_area(0,0,320,240,CIPHER_BACKGROUND);    //Make display black
    for (i=0; i<12; i++)
    {
	for (j=0; j<12; j++)
	{
	    if (b_cipher[i] & 1<<j) {
		uint16_t curX = CIPHER_X0+((12-j)*CIPHER_CHAR_WIDTH)-((j/3)*CIPHER_SPACE);
		uint8_t curY = CIPHER_Y0+(i*CIPHER_CHAR_WIDTH)+((i/3)*CIPHER_SPACE);
		//FIXME: Why do I need these -1 adjustments?
		tft_fill_area(curX,curY,CIPHER_CHAR_WIDTH-1,CIPHER_CHAR_WIDTH-1,CIPHER_COLOR);
	    }
	}
    }
}

void draw_vert_line(uint16_t x, uint8_t y, uint8_t height, uint32_t color)
{
    tft_fill_area(x,y,0,height-1,color);
}

void move_box_right(struct Cipher_box *b)
{
    uint8_t y = CIPHER_Y0 + (b->row * CIPHER_CHAR_WIDTH)+((b->row / 3) * CIPHER_SPACE);

    if ((b->x<320) && (b->x>=0))
    {
	//Draw leading edge
	draw_vert_line(b->x,y,CIPHER_CHAR_WIDTH,CIPHER_CURSOR);
    }
    if (b->x>=CIPHER_CHAR_WIDTH)
    {
	//Erase trailing edge
	//FIXME: erase with red if this is within the splash area
	if (overlaps_logo(b->x-CIPHER_CHAR_WIDTH,b->row))
	{
	    draw_vert_line(b->x-CIPHER_CHAR_WIDTH,y,CIPHER_CHAR_WIDTH,CIPHER_COLOR);
	}
	else
	{
	    draw_vert_line(b->x-CIPHER_CHAR_WIDTH,y,CIPHER_CHAR_WIDTH,CIPHER_BACKGROUND);
	}
    }
    
    b->x = b->x + 1;
}

void move_box_left(struct Cipher_box *b)
{
    uint8_t y = CIPHER_Y0 + (b->row * CIPHER_CHAR_WIDTH)+((b->row / 3) * CIPHER_SPACE);

    if ((b->x<320) && (b->x>=0))
    {
	//Draw leading edge
	draw_vert_line(b->x,y,CIPHER_CHAR_WIDTH,CIPHER_CURSOR);
    }
    if ((b->x+CIPHER_CHAR_WIDTH<320) && (b->x+CIPHER_CHAR_WIDTH>=0))
    {
	//Erase trailing edge
	//FIXME: erase with red if this is within the splash area
	if (overlaps_logo(b->x+CIPHER_CHAR_WIDTH,b->row))
	{
	    draw_vert_line(b->x+CIPHER_CHAR_WIDTH,y,CIPHER_CHAR_WIDTH,CIPHER_COLOR);
	}
	else
	{
	    draw_vert_line(b->x+CIPHER_CHAR_WIDTH,y,CIPHER_CHAR_WIDTH,CIPHER_BACKGROUND);
	}
    }

    b->x = b->x - 1;
}

uint8_t overlaps_logo(int16_t x, uint8_t row) {
    //Returns non-zero if the location overlaps part of the logo
    if ((x<CIPHER_X0) || (x>=320))
    {
	return 0;   //Not within area where logo will be drawn
    }
    int16_t offset_adjusted = x-CIPHER_X0;//55
    uint8_t superblock_size = (CIPHER_CHAR_WIDTH*3) + CIPHER_SPACE;//59
    uint8_t superblock = offset_adjusted / superblock_size;//0
    uint8_t local_x = offset_adjusted-(superblock * superblock_size);//55
    
    if (local_x >= (CIPHER_CHAR_WIDTH*3))
    {
	return 0;   //Falls within space between squares on logo
    }
    
    uint8_t index = (local_x/(CIPHER_CHAR_WIDTH))+(3*superblock);
    if (b_cipher[row] & 1<<(11-index))
    {
	return 1;
    }
    return 0;
}

void splash_delay(uint32_t delay)
	{
	uint32_t splash_startwait = millis();
	while((millis()-splash_startwait) < delay) { ;; }
	return;
	}

void animate_splash(void)
{
    uint16_t i;
    
	
	/*
	// H  arc (52,17,0) (76,17,1) box (52,12,60,60) (92,37,100,60)
	draw_arc(52,17,0,spcolor); draw_arc(77,17,1,spcolor); tft_fill_area(52,12,8,48,spcolor); tft_fill_area(93,37,8,23,spcolor); 
	// A  arc (110,12,0) (134,12,1) (134,36,2) (110,36,3) box (149,36,157,60)
	draw_arc(110,12,0,spcolor); draw_arc(134,12,1,spcolor); draw_arc(134,37,2,spcolor); draw_arc(110,37,3,spcolor); tft_fill_area(150,36,8,25,spcolor);
	// C  arc (167,17,0) (167,36,3) box (190,12,215,20) (190,52,215,60)
	draw_arc(167,12,0,spcolor); draw_arc(167,37,3,spcolor); tft_fill_area(190,12,25,8,spcolor); tft_fill_area(190,53,25,8,spcolor);
	// K  arc (248,17,2) (248,36,1) bot (225,12,233,60)
	draw_arc(248,12,2,spcolor); draw_arc(248,37,1,spcolor); tft_fill_area(225,12,8,48,spcolor);
	// A  arc (52,70,0) (76,70,1) (134,94,2) (52,94,3) box (92,94,100,118)
	draw_arc(52,70,0,spcolor); draw_arc(76,70,1,spcolor); draw_arc(76,95,2,spcolor); draw_arc(52,95,3,spcolor); tft_fill_area(92,94,8,24,spcolor);
	// D  arc (110,70,0) (134,70,1) (134,94,2) (110,36,3) box (149,70,157,93)
	draw_arc(110,70,0,spcolor); draw_arc(134,70,1,spcolor); draw_arc(134,95,2,spcolor); draw_arc(110,95,3,spcolor); tft_fill_area(150,70,8,24,spcolor);
	// A  arc (167,70,0) (191,70,1) (191,94,2) (167,36,3) box (207,94,215,118)
	draw_arc(167,70,0,spcolor); draw_arc(191,70,1,spcolor); draw_arc(191,95,2,spcolor); draw_arc(167,95,3,spcolor); tft_fill_area(207,94,8,25,spcolor);
	// Y  arc (225,70,3) (249,70,2) box (244,90,252,118)
	draw_arc(225,70,3,spcolor); draw_arc(249,70,2,spcolor); tft_fill_area(244,90,8,29,spcolor);
	
	// S arc (52,128,0) (76,152,2)
	draw_arc(52,128,0,spcolor); draw_arc(76,153,2,spcolor);
	// U arc (110,152,3) (134,152,2) box (110,128,8,24) (150,128,8,24)
	draw_arc(110,153,3,spcolor); draw_arc(134,153,2,spcolor); tft_fill_area(110,128,8,24,spcolor); tft_fill_area(150,128,8,24,spcolor);
	// P arc (167,128,0) (191,128,1) (191,152,2) (167,152,3) box (167,152,8,25)
	draw_arc(167,128,0,spcolor); draw_arc(191,128,1,spcolor); draw_arc(191,153,2,spcolor); draw_arc(167,153,3,spcolor); tft_fill_area(167,152,8,25,spcolor);
	// E arc (225,128,0) (249,128,1) (225,152,3)
	draw_arc(225,128,0,spcolor); draw_arc(249,128,1,spcolor); draw_arc(225,153,3,spcolor);
	// R arc (52,186,0) (76,186,1) box (52,210,8,25)
	draw_arc(52,186,0,spcolor); draw_arc(76,186,1,spcolor); tft_fill_area(52,210,8,25,spcolor);
	// C arc (110,186,0) (110,210,3) box (134,186,24,8) (134,227,24,8)
	draw_arc(110,186,0,spcolor); draw_arc(110,211,3,spcolor); tft_fill_area(134,186,24,8,spcolor); tft_fill_area(134,227,24,8,spcolor);
	// O arc (167,186,0) (191,186,1) (191,210,2) (167,210,3)
	draw_arc(167,186,0,spcolor); draw_arc(191,186,1,spcolor); draw_arc(191,211,2,spcolor); draw_arc(167,211,3,spcolor);
	// N arc (225,186,0) (249,186,1) box (225,210,8,25) (266,210,8,25)
	draw_arc(225,186,0,spcolor); draw_arc(249,186,1,spcolor); tft_fill_area(225,210,8,25,spcolor); tft_fill_area(265,210,8,25,spcolor);
	*/
	
	/*
	draw_arc(10,10,0,0xFF0000);
	draw_arc(35,10,1,0xFF0000);
	draw_arc(35,35,2,0xFF0000);
	draw_arc(10,35,3,0xFF0000);
	 */

	uint16_t spoffsetx, spoffsety;
	uint32_t spcolor;
	
	//while(1) {
	for (i=0; i<9600; i++) { ram[i] = 0; }
    
	tft_fill_area(0,0,320,240,CIPHER_BACKGROUND);    //Make display black
	splash_delay(1300);
	spoffsetx = 6;
	spoffsety = 6;
	spcolor = 0x0016aeef;
	
	
	// U arc (110,152,3) (134,152,2) box (110,128,8,24) (150,128,8,24)
	draw_arc(110-spoffsetx,153-spoffsety,3,spcolor); draw_arc(134-spoffsetx,153-spoffsety,2,spcolor); draw_and_record_box(110-spoffsetx,128-spoffsety,8,24,spcolor); draw_and_record_box(150-spoffsetx,128-spoffsety,8,24,spcolor);
	splash_delay(SPLASH_DELAY);
	// D  arc (110,70,0) (134,70,1) (134,94,2) (110,36,3) box (149,70,157,93)
	draw_arc(110-spoffsetx,70-spoffsety,0,spcolor); draw_arc(134-spoffsetx,70-spoffsety,1,spcolor); draw_arc(134-spoffsetx,95-spoffsety,2,spcolor); draw_arc(110-spoffsetx,95-spoffsety,3,spcolor); draw_and_record_box(150-spoffsetx,70-spoffsety,8,24,spcolor);
	splash_delay(SPLASH_DELAY);
	// A  arc (167,70,0) (191,70,1) (191,94,2) (167,36,3) box (207,94,215,118)
	draw_arc(167-spoffsetx,70-spoffsety,0,spcolor); draw_arc(191-spoffsetx,70-spoffsety,1,spcolor); draw_arc(191-spoffsetx,95-spoffsety,2,spcolor); draw_arc(167-spoffsetx,95-spoffsety,3,spcolor); draw_and_record_box(207-spoffsetx,94-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// P arc (167,128,0) (191,128,1) (191,152,2) (167,152,3) box (167,152,8,25)
	draw_arc(167-spoffsetx,128-spoffsety,0,spcolor); draw_arc(191-spoffsetx,128-spoffsety,1,spcolor); draw_arc(191-spoffsetx,153-spoffsety,2,spcolor); draw_arc(167-spoffsetx,153-spoffsety,3,spcolor); draw_and_record_box(167-spoffsetx,152-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// O arc (167,186,0) (191,186,1) (191,210,2) (167,210,3)
	draw_arc(167-spoffsetx,186-spoffsety,0,spcolor); draw_arc(191-spoffsetx,186-spoffsety,1,spcolor); draw_arc(191-spoffsetx,211-spoffsety,2,spcolor); draw_arc(167-spoffsetx,211-spoffsety,3,spcolor);
	splash_delay(SPLASH_DELAY);
	// C arc (110,186,0) (110,210,3) box (134,186,24,8) (134,227,24,8)
	draw_arc(110-spoffsetx,186-spoffsety,0,spcolor); draw_arc(110-spoffsetx,211-spoffsety,3,spcolor); draw_and_record_box(134-spoffsetx,186-spoffsety,24,8,spcolor); draw_and_record_box(134-spoffsetx,227-spoffsety,24,8,spcolor);
	splash_delay(SPLASH_DELAY);
	// R arc (52,186,0) (76,186,1) box (52,210,8,25)
	draw_arc(52-spoffsetx,186-spoffsety,0,spcolor); draw_arc(76-spoffsetx,186-spoffsety,1,spcolor); draw_and_record_box(52-spoffsetx,210-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// S arc (52,128,0) (76,152,2)
	draw_arc(52-spoffsetx,128-spoffsety,0,spcolor); draw_arc(76-spoffsetx,153-spoffsety,2,spcolor);
	splash_delay(SPLASH_DELAY);
	// A  arc (52,70,0) (76,70,1) (134,94,2) (52,94,3) box (92,94,100,118)
	draw_arc(52-spoffsetx,70-spoffsety,0,spcolor); draw_arc(76-spoffsetx,70-spoffsety,1,spcolor); draw_arc(76-spoffsetx,95-spoffsety,2,spcolor); draw_arc(52-spoffsetx,95-spoffsety,3,spcolor); draw_and_record_box(92-spoffsetx,94-spoffsety,8,24,spcolor);
	splash_delay(SPLASH_DELAY);
	// H  arc (52,17,0) (76,17,1) box (52,12,60,60) (92,37,100,60)
	draw_arc(52-spoffsetx,17-spoffsety,0,spcolor); draw_arc(77-spoffsetx,17-spoffsety,1,spcolor); draw_and_record_box(52-spoffsetx,12-spoffsety,8,48,spcolor); draw_and_record_box(93-spoffsetx,37-spoffsety,8,23,spcolor);
	splash_delay(SPLASH_DELAY);
	// A  arc (110,12,0) (134,12,1) (134,36,2) (110,36,3) box (149,36,157,60)
	draw_arc(110-spoffsetx,12-spoffsety,0,spcolor); draw_arc(134-spoffsetx,12-spoffsety,1,spcolor); draw_arc(134-spoffsetx,37-spoffsety,2,spcolor); draw_arc(110-spoffsetx,37-spoffsety,3,spcolor); draw_and_record_box(150-spoffsetx,36-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// C  arc (167,17,0) (167,36,3) box (190,12,215,20) (190,52,215,60)
	draw_arc(167-spoffsetx,12-spoffsety,0,spcolor); draw_arc(167-spoffsetx,37-spoffsety,3,spcolor); draw_and_record_box(190-spoffsetx,12-spoffsety,25,8,spcolor); draw_and_record_box(190-spoffsetx,53-spoffsety,25,8,spcolor);
	splash_delay(SPLASH_DELAY);
	// K  arc (248,17,2) (248,36,1) bot (225,12,233,60)
	draw_arc(248-spoffsetx,12-spoffsety,2,spcolor); draw_arc(248-spoffsetx,37-spoffsety,1,spcolor); draw_and_record_box(225-spoffsetx,12-spoffsety,8,48,spcolor);
	splash_delay(SPLASH_DELAY);
	// Y  arc (225,70,3) (249,70,2) box (244,90,252,118)
	draw_arc(225-spoffsetx,70-spoffsety,3,spcolor); draw_arc(249-spoffsetx,70-spoffsety,2,spcolor); draw_and_record_box(244-spoffsetx,90-spoffsety,8,29,spcolor);
	splash_delay(SPLASH_DELAY);
	// E arc (225,128,0) (249,128,1) (225,152,3)
	draw_arc(225-spoffsetx,128-spoffsety,0,spcolor); draw_arc(249-spoffsetx,128-spoffsety,1,spcolor); draw_arc(225-spoffsetx,153-spoffsety,3,spcolor);
	splash_delay(SPLASH_DELAY);
	// N arc (225,186,0) (249,186,1) box (225,210,8,25) (266,210,8,25)
	draw_arc(225-spoffsetx,186-spoffsety,0,spcolor); draw_arc(249-spoffsetx,186-spoffsety,1,spcolor); draw_and_record_box(225-spoffsetx,210-spoffsety,8,25,spcolor); draw_and_record_box(265-spoffsetx,210-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	
	spoffsetx = 0;
	spoffsety = 0;
	spcolor = 0x00FFFFFF;
	
	// H  arc (52,17,0) (76,17,1) box (52,12,60,60) (92,37,100,60)
	draw_arc(52-spoffsetx,17-spoffsety,0,spcolor); draw_arc(77-spoffsetx,17-spoffsety,1,spcolor); tft_fill_area(52-spoffsetx,12-spoffsety,8,48,spcolor); tft_fill_area(93-spoffsetx,37-spoffsety,8,23,spcolor);
	splash_delay(SPLASH_DELAY);
	// A  arc (110,12,0) (134,12,1) (134,36,2) (110,36,3) box (149,36,157,60)
	draw_arc(110-spoffsetx,12-spoffsety,0,spcolor); draw_arc(134-spoffsetx,12-spoffsety,1,spcolor); draw_arc(134-spoffsetx,37-spoffsety,2,spcolor); draw_arc(110-spoffsetx,37-spoffsety,3,spcolor); tft_fill_area(150-spoffsetx,36-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// C  arc (167,17,0) (167,36,3) box (190,12,215,20) (190,52,215,60)
	draw_arc(167-spoffsetx,12-spoffsety,0,spcolor); draw_arc(167-spoffsetx,37-spoffsety,3,spcolor); tft_fill_area(190-spoffsetx,12-spoffsety,25,8,spcolor); tft_fill_area(190-spoffsetx,53-spoffsety,25,8,spcolor);
	splash_delay(SPLASH_DELAY);
	// K  arc (248,17,2) (248,36,1) bot (225,12,233,60)
	draw_arc(248-spoffsetx,12-spoffsety,2,spcolor); draw_arc(248-spoffsetx,37-spoffsety,1,spcolor); tft_fill_area(225-spoffsetx,12-spoffsety,8,48,spcolor);
	splash_delay(SPLASH_DELAY);
	// A  arc (52,70,0) (76,70,1) (134,94,2) (52,94,3) box (92,94,100,118)
	draw_arc(52-spoffsetx,70-spoffsety,0,spcolor); draw_arc(76-spoffsetx,70-spoffsety,1,spcolor); draw_arc(76-spoffsetx,95-spoffsety,2,spcolor); draw_arc(52-spoffsetx,95-spoffsety,3,spcolor); tft_fill_area(92-spoffsetx,94-spoffsety,8,24,spcolor);
	splash_delay(SPLASH_DELAY);
	// D  arc (110,70,0) (134,70,1) (134,94,2) (110,36,3) box (149,70,157,93)
	draw_arc(110-spoffsetx,70-spoffsety,0,spcolor); draw_arc(134-spoffsetx,70-spoffsety,1,spcolor); draw_arc(134-spoffsetx,95-spoffsety,2,spcolor); draw_arc(110-spoffsetx,95-spoffsety,3,spcolor); tft_fill_area(150-spoffsetx,70-spoffsety,8,24,spcolor);
	splash_delay(SPLASH_DELAY);
	// A  arc (167,70,0) (191,70,1) (191,94,2) (167,36,3) box (207,94,215,118)
	draw_arc(167-spoffsetx,70-spoffsety,0,spcolor); draw_arc(191-spoffsetx,70-spoffsety,1,spcolor); draw_arc(191-spoffsetx,95-spoffsety,2,spcolor); draw_arc(167-spoffsetx,95-spoffsety,3,spcolor); tft_fill_area(207-spoffsetx,94-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// Y  arc (225,70,3) (249,70,2) box (244,90,252,118)
	draw_arc(225-spoffsetx,70-spoffsety,3,spcolor); draw_arc(249-spoffsetx,70-spoffsety,2,spcolor); tft_fill_area(244-spoffsetx,90-spoffsety,8,29,spcolor);
	splash_delay(SPLASH_DELAY);
	// S arc (52,128,0) (76,152,2)
	draw_arc(52-spoffsetx,128-spoffsety,0,spcolor); draw_arc(76-spoffsetx,153-spoffsety,2,spcolor);
	splash_delay(SPLASH_DELAY);
	// U arc (110,152,3) (134,152,2) box (110,128,8,24) (150,128,8,24)
	draw_arc(110-spoffsetx,153-spoffsety,3,spcolor); draw_arc(134-spoffsetx,153-spoffsety,2,spcolor); draw_and_record_box(110-spoffsetx,128-spoffsety,8,24,spcolor); tft_fill_area(150-spoffsetx,128-spoffsety,8,24,spcolor);
	splash_delay(SPLASH_DELAY);
	// P arc (167,128,0) (191,128,1) (191,152,2) (167,152,3) box (167,152,8,25)
	draw_arc(167-spoffsetx,128-spoffsety,0,spcolor); draw_arc(191-spoffsetx,128-spoffsety,1,spcolor); draw_arc(191-spoffsetx,153-spoffsety,2,spcolor); draw_arc(167-spoffsetx,153-spoffsety,3,spcolor); tft_fill_area(167-spoffsetx,152-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// E arc (225,128,0) (249,128,1) (225,152,3)
	draw_arc(225-spoffsetx,128-spoffsety,0,spcolor); draw_arc(249-spoffsetx,128-spoffsety,1,spcolor); draw_arc(225-spoffsetx,153-spoffsety,3,spcolor);
	splash_delay(SPLASH_DELAY);
	// R arc (52,186,0) (76,186,1) box (52,210,8,25)
	draw_arc(52-spoffsetx,186-spoffsety,0,spcolor); draw_arc(76-spoffsetx,186-spoffsety,1,spcolor); tft_fill_area(52-spoffsetx,210-spoffsety,8,25,spcolor);
	splash_delay(SPLASH_DELAY);
	// C arc (110,186,0) (110,210,3) box (134,186,24,8) (134,227,24,8)
	draw_arc(110-spoffsetx,186-spoffsety,0,spcolor); draw_arc(110-spoffsetx,211-spoffsety,3,spcolor); tft_fill_area(134-spoffsetx,186-spoffsety,24,8,spcolor); tft_fill_area(134-spoffsetx,227-spoffsety,24,8,spcolor);
	splash_delay(SPLASH_DELAY);
	// O arc (167,186,0) (191,186,1) (191,210,2) (167,210,3)
	draw_arc(167-spoffsetx,186-spoffsety,0,spcolor); draw_arc(191-spoffsetx,186-spoffsety,1,spcolor); draw_arc(191-spoffsetx,211-spoffsety,2,spcolor); draw_arc(167-spoffsetx,211-spoffsety,3,spcolor);
	splash_delay(SPLASH_DELAY);
	// N arc (225,186,0) (249,186,1) box (225,210,8,25) (266,210,8,25)
	draw_arc(225-spoffsetx,186-spoffsety,0,spcolor); draw_arc(249-spoffsetx,186-spoffsety,1,spcolor); tft_fill_area(225-spoffsetx,210-spoffsety,8,25,spcolor); tft_fill_area(265-spoffsetx,210-spoffsety,8,25,spcolor);
	splash_delay(1300);
	
	//	}
	
	/*
    struct Cipher_box box0 = { 0, 0 };
    struct Cipher_box box1 = { 319+(10*CIPHER_CHAR_WIDTH), 1 };
    struct Cipher_box box2 = { 0-(2*CIPHER_CHAR_WIDTH), 2 };
    struct Cipher_box box3 = { 319+(8*CIPHER_CHAR_WIDTH), 3 };
    struct Cipher_box box4 = { 0-(4*CIPHER_CHAR_WIDTH), 4 };
    struct Cipher_box box5 = { 319+(6*CIPHER_CHAR_WIDTH), 5 };
    struct Cipher_box box6 = { 0-(6*CIPHER_CHAR_WIDTH), 6 };
    struct Cipher_box box7 = { 319+(4*CIPHER_CHAR_WIDTH), 7 };
    struct Cipher_box box8 = { 0-(8*CIPHER_CHAR_WIDTH), 8 };
    struct Cipher_box box9 = { 319+(2*CIPHER_CHAR_WIDTH), 9 };
    struct Cipher_box box10 = { 0-(10*CIPHER_CHAR_WIDTH), 10 };
    struct Cipher_box box11 = { 319, 11 };
    
    tft_fill_area(0,0,320,240,CIPHER_BACKGROUND);    //Make display black
    
    while (1) {
	if (millis()>=splash_waitfor)
	{
	    splash_waitfor = millis()+4;
	    move_box_right(&box0);
	    move_box_left(&box1);
	    move_box_right(&box2);
	    move_box_left(&box3);
	    move_box_right(&box4);
	    move_box_left(&box5);
	    move_box_right(&box6);
	    move_box_left(&box7);
	    move_box_right(&box8);
	    move_box_left(&box9);
	    move_box_right(&box10);
	    move_box_left(&box11);
	    
	    if (box10.x>=(320+CIPHER_CHAR_WIDTH))
	    {
		break;
	    }
	}
    }
	 */
}
