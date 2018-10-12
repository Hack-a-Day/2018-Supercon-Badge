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

void draw_arc (uint16_t x, uint16_t y, uint16_t xlen, uint16_t ylen, uint32_t back)
{
    uint32_t i,j;
    tft_set_write_area(x,y,xlen,ylen);
    TFT_24_7789_Write_Command(0x2C);
    //FIXME: Why do I need these +1 adjustments. Off-by-one in tft_set_write_area?
    for (i=0; i<((xlen+1)*(ylen+1)); i++)
    {
    //TODO: Test for arc 1 or 0 and place color pixel as necessary
    TFT_24_7789_Write_Data3((back>>16)&0xFF,(back>>8)&0xFF,(back>>0)&0xFF);
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

void animate_splash(void)
{
    uint16_t x = 0;
    uint16_t splash_waitfor = 0;
    
	tft_fill_area(0,0,320,240,CIPHER_BACKGROUND);    //Make display black
	draw_arc(10,10,100,100,0xFF0000);
	while (1) { ;; }
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
