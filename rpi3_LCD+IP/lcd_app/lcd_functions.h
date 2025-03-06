#ifndef LCD_FUNCTIONS_H_
#define LCD_FUNCTIONS_H_

#include <sys/types.h>    
#include <sys/socket.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

int display_clear(int fd);
int display_off(int fd);
int display_on(int fd);
int display_reset(int fd);
int cursor_ON(int fd);
int cursor_OFF(int fd);
int blink_ON(int fd);
int blink_OFF(int fd);
int cursor_LEFT(int fd);
int cursor_RIGHT(int fd);
int display_LEFT(int fd);
int display_RIGHT(int fd);
int goto_X_LCD(int fd, int x);
int goto_Y_LCD(int fd, int y);
int goto_XY_LCD(int fd, int x, int y);
int backlight_ON(int fd);
int backlight_OFF(int fd);

int print_ip_address(int lcd_fd);
void lcd_write(int fd, char* message);

#endif /*LCD_FUNCTIONS_H_*/
