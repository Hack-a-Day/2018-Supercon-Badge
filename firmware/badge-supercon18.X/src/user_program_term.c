#include "badge_user.h"

// Simple terminal program:
// Characters from the serial port are printed on the screen, and characters
// from the keyboard are sent to the serial port
//
// NOTE: Just like with a real terminal, characters from the keyboard don't get
// printed on the screen; the host computer (to which you connect the serial
// port) should take care of echoing the characters (or not, e.g. when you're
// typing a password).
//
// NOTE: this is a simple program that basically loops at full speed. It works
// fine but of course it's not very efficient with energy. It would be better
// to put the CPU to sleep and let the UART or the keyboard wake it up. This
// is left as an excercise for the reader :-)

void user_term_init(void)
	{
	clr_buffer();
	video_gotoxy(0,0);
    serial_flush();
	}

void user_term_loop(void)
	{
        if (rx_sta())
        {
            stdio_c(rx_read());
        }

        uint8_t c;
        if (stdio_get(&c))
        {
            tx_write(c);
        }
	}
