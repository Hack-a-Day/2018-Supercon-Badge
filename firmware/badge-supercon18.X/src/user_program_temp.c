#include "badge_user.h"

//demo program to excercise IIC Si7020 temperature sensor via IIC

int8_t temper;
char user_string[25];
int8_t get_temp(void);

void user_program_init(void)
	{
	clr_buffer();
	video_set_color(EGA_YELLOW,EGA_BLACK);
	iic_init();
    stdio_write("Si7020 temperature demo:\n\n");
	video_set_color(EGA_WHITE,EGA_BLACK);
	}

void user_program_loop(void)
	{
	temper = get_temp();
	sprintf(user_string,"Temperature now is: %d deg C\n",temper);
	stdio_write(user_string);
	wait_ms(500);
	}

int8_t get_temp(void)
	{
	uint8_t d1,d2;
	uint32_t res;
	int8_t temperature;
	iic_start();
	iic_write(0x80);
	iic_write(0xF3);
	do
		{
		iic_restart();
		iic_write(0x81);
		d1 = iic_ackstat();
		} while (d1==1);
	d1 = iic_read(0);
	d2 = iic_read(1);
	iic_stop();	
	res = d1;
	res = (res * 256) + d2;
	res = res * 176;
	res = res  >> 16;
	temperature = res;
	temperature = temperature - 47;
	return temperature;
	}
