ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

app: lcd_app.o

lcd_app.o: lcd_app.c
	$(CROSS_COMPILE)gcc -c -o $@ $^

clean:
	rm lcd.o gpio.o lcd_app.elf lcd_app.o



