ARCH=arm
CROSS_COMPILE=/home/maciej/Desktop/moje/programming/Linux/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

app: module_test

module_test: module_test.c
	$(CROSS_COMPILE)gcc -o $@ $^

clean:
	rm module_test



