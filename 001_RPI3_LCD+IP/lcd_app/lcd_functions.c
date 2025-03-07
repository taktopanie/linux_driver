
#include "lcd_functions.h"

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
	sprintf(msg, "%c[Ly%03dx%03d;", LCD_ESCAPE_CHAR, y, x);
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

/* This function gathers the ip address of the system and prints it on LCD */
int print_ip_address(int lcd_fd)
{
	int fd;
	struct ifreq ifr;

	char iface[] = "wlan0";

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;

	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

    char * message = (char *)inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);

    write(lcd_fd ,message ,strlen(message));


	return 0;
}

void lcd_write(int fd, char* message)
{
    write(fd, message, strlen(message));
}