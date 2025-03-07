#!/bin/bash

# because network-online.target gives some weird IP after execution 
# wait 20 second to gather the IP address before printing
echo IP waiting 20 sec...
sleep 20

# exec file
./home/pi/Desktop/from_host/lcd_app.elf
