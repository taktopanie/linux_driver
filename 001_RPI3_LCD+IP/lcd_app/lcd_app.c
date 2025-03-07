#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "lcd_functions.h"

/* USER INTERFACE USED BY HD44780 WITH THIS DRIVER IS IN CHARLCD.C */

/*=========================================================================================================
RPi Pin                           GPIO number      16x2 LCD pin    Purpose 
===========================================================================================================
P21                               gpio21           4(RS)           Register selection (Character vs. Command)
P20                               gpio20           5(RW)           Read/write 
P16                               gpio16           6(EN)           Enable
P1                                gpio1            11(D4)          Data line 4
P7                                gpio7            12(D5)          Data line 5
P8                                gpio8            13(D6)          Data line 6
P25                               gpio25           14(D7)          Data line 7 
(GND)                             ----             15(BKLTA)       Backlight anode(+)
(sys_5V supply)                   ----             16(BKLTK)       Backlight cathode(-)

(GND)                             ----             1(VSS/GND)      Ground
(sys_5V supply)                   ----             2(VCC)          +5V supply 
============================================================================================================= */

int main(int argc, char *argv[])
 {	
	int fd; 
	fd = open("/dev/lcd", O_WRONLY);

	if(fd <= 0)
	{	
		printf("Cannot open device: %s\n", strerror(errno));
		return -1;
	}

	int tmp ;
	char scan_msg [50];

	if(display_clear(fd))
		{
			printf("Cannot clear the screen: %s\n", strerror(errno));
			return errno;
		}
		
	// backlight_ON(fd);

	display_clear(fd);
	blink_OFF(fd);
	cursor_OFF(fd);

	lcd_write(fd, "RPi IP address:");

	goto_XY_LCD(fd, 0, 1);
	print_ip_address(fd);
	
	if(tmp < 0)
	{
		return errno;
	}
	backlight_ON(fd);

	close (fd);
	return 0;
 }
