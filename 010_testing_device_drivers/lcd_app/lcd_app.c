#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

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
 
#define LCD_ESCAPE_CHAR		27
#define LCD_ESCAPE_LEN		24


int display_clear(int fd)
{
	int tmp = write(fd, "\n\f", 2);
	if(tmp < 0) return errno;
	return 0;
}

int display_off(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Lb", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int display_on(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[LB", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int display_reset(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[LI", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int cursor_ON(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[LC", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int cursor_OFF(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Lc", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int blink_ON(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[LB", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int blink_OFF(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Lb", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int cursor_LEFT(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Ll", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int cursor_RIGHT(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Lr", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int display_LEFT(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[LL", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int display_RIGHT(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[LR", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int goto_X_LCD(int fd, int x)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Lx%03d;", LCD_ESCAPE_CHAR, x);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int goto_Y_LCD(int fd, int y)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Ly%03d;", LCD_ESCAPE_CHAR, y);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int goto_XY_LCD(int fd, int x, int y)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[Ly%03d%03d;", LCD_ESCAPE_CHAR, x, y);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int backlight_ON(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[L+", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}

int backlight_OFF(int fd)
{
	char msg [LCD_ESCAPE_LEN];
	sprintf(msg, "%c[L-", LCD_ESCAPE_CHAR);
	int tmp = write(fd, msg, strlen(msg));

	if(tmp < 0) return errno;
	return 0;
}


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

	printf("What do you want to display?: ");

	if(display_clear(fd))
		{
			printf("Cannot clear the screen: %s\n", strerror(errno));
			return errno;
		}
		
	backlight_ON(fd);

	tmp = write(fd, "Input message...", strlen("Input message...") -1 );

	if(tmp < 0)
	{
		return errno;
	}

	scanf("%80[^\n]s",&scan_msg);

	display_clear(fd);
	blink_OFF(fd);
	cursor_OFF(fd);

	goto_XY_LCD(fd, 0, 1);

	tmp = write(fd, scan_msg, strlen(scan_msg));

	if(tmp < 0)
	{
		return errno;
	}

	for(int x = 0; x < 10 ; x++)
	{
		display_RIGHT(fd);
		usleep(200000);
	}

	for(int x = 0; x < 10 ; x++)
	{
		display_LEFT(fd);
		usleep(300000);
	}
	
	blink_ON(fd);
	sleep(5);

	backlight_OFF(fd);

	close (fd);
	return 0;
 }
