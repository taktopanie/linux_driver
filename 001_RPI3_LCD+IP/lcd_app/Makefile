ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
GCC_PATH=/home/maciej/Desktop/moje/programming/Linux/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/


app: lcd_app.elf

lcd_app.o: lcd_app.c
	$(GCC_PATH)$(CROSS_COMPILE)gcc -c -o $@ $^

lcd_functions.o: lcd_functions.c
	$(GCC_PATH)$(CROSS_COMPILE)gcc -c -o $@ $^

lcd_app.elf: lcd_functions.o lcd_app.o
	$(GCC_PATH)$(CROSS_COMPILE)gcc -o $@ $^

clean:
	rm lcd_app.elf