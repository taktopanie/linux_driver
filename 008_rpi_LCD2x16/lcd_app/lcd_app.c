/*
 ============================================================================
 Name        : lcd_text.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     :
 Copyright   : Your copyright notice
 Description : This Application prints your message on the 16x2 LCD along with ip address of BBB
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include<stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "lcd.h"
#include "gpio.h"

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
 




/* Configure the direction of gpios used for LCD connections
 */
int init_gpios(void)
{
	/* configure the direction for LCD pins */
	gpio_configure_dir(GPIO_LCD_RS,GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_LCD_RW,GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_LCD_EN,GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_LCD_D4,GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_LCD_D5,GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_LCD_D6,GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_LCD_D7,GPIO_DIR_OUT);

	gpio_write_value(GPIO_LCD_RS,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_LCD_EN,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_LCD_RW,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_LCD_D4,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_LCD_D5,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_LCD_D6,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_LCD_D7,GPIO_LOW_VALUE);

	return 0;

}

/* This functin prints system's current date and time information on the LCD */
void print_time_and_date()
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	lcd_send_command(DDRAM_FIRST_LINE_BASE_ADDR);

	lcd_printf("%d-%d-%d",1900+timeinfo->tm_year, timeinfo->tm_mon + 1, timeinfo->tm_mday);

	lcd_send_command(DDRAM_SECOND_LINE_BASE_ADDR);

	lcd_printf("%d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

}

/* This function gathers the ip address of the system and prints it on LCD */
int print_ip_address()
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

	//display result
	// 1. print the interface name on the first line
	lcd_send_command(DDRAM_FIRST_LINE_BASE_ADDR);
	lcd_print_string((char *)iface);
	lcd_send_command(DDRAM_SECOND_LINE_BASE_ADDR);
	lcd_print_string((char *)inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));


	return 0;
}

/* Some silly graphics */
void transition_graphics(void)
{

	sleep(1);

	lcd_set_cursor(1,1);

	lcd_display_clear();

	for (uint8_t n =0 ; n < 2 ; n++)
	{
		for(uint8_t i=0;i<16;i++)
		{
			lcd_print_char('*');
			usleep(75*1000);

		}
			lcd_set_cursor(2,16);
			lcd_send_command(0x04);
	}

	lcd_set_cursor(1,1);
	lcd_send_command(0x06);
	usleep(450 * 1000);

	lcd_display_clear();

}

int main(int argc, char *argv[])
 {
	char msg_buf[81];
	int value;

	printf("This Application prints your message on the 16x2 LCD\n");

	init_gpios();

	printf("Enter your message(Max. 80 char) : ");
	scanf("%80[^\n]s",msg_buf);

	int msg_len = strlen(msg_buf);

	printf("Message %d: %s\n",msg_len,msg_buf);

        /* Initialization of LCD */
	lcd_init();

        sleep(1);

	while(1)
	{
		lcd_display_clear();

		lcd_printf(msg_buf);

		sleep(1);

		/*if message length is > 16, do left shift of message */
		if(msg_len > 16){

			for(int i = 0 ; i < msg_len-16; i++){
				//LCD command to left shift the display 
				lcd_send_command(0x18);
				usleep(500 * 1000);
			}
		}

	
		/*Clear display */
		lcd_display_clear();
		
		/*Return home */
		lcd_display_return_home();	
		
		transition_graphics();
		print_ip_address();	/*Print IP address of RPi */
		sleep(1);
		transition_graphics();
		print_time_and_date();	/* Print data and time info */
		sleep(1);
		transition_graphics();
		
	}

	lcd_deinit();

    	return 0;
}


