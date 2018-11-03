#include "badge_user.h"

//simple one way terminal - characters from serial are directed to stdio

void user_program_init(void)
	{
	clr_buffer();
	video_gotoxy(0,0);
	}

void user_program_loop(void)
	{
	uint8_t temp;
	if (rx_sta())
		{
		temp = rx_read();
		stdio_c(temp);
		}	
	}

