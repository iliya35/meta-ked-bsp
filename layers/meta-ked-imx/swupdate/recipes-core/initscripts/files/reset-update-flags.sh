#!/bin/sh


set_error_leds() {

	delay=$1

	for led in $(find /sys/class/leds -type l -name "*led?")
	do
	   echo timer > $led/trigger
	   echo $delay > $led/delay_on
	   echo $delay > $led/delay_off
	done 
}

case "$1" in

start)
        echo  "Watchdog disabled." 
        echo V > /dev/watchdog
        
        if grep -q "fallback" /proc/cmdline; then
                echo "Booting fallback rootfs!"
                echo "### WARNING ###"
                echo "### The last system-update failed.###" 
                echo "### For this reason we are booting the fallback rootfs copy. ###"
                echo "### Please retry system-update! ###"
                set_error_leds 200
                
                upgrade_avail=$(fw_printenv upgrade_available | cut -d "=" -f 2)
                
                if [ $upgrade_avail -ne 1 ];then
                	echo "setting upgrade_available=1"
                	fw_setenv upgrade_available 1
                fi 
        else
                echo "Booting regular rootfs."
                upgrade_avail=$(fw_printenv upgrade_available | cut -d "=" -f 2)
                
                 if [ $upgrade_avail -ne 0 ];then
                 	echo "setting upgrade_available=0"
                	fw_setenv upgrade_available 0
                fi 
        fi
        
        bootcount=$(fw_printenv bootcount | cut -d "=" -f 2)
        
         if [ $bootcount -ne 0 ];then
                echo "setting bootcount=0"
                fw_setenv bootcount 0
         fi 
        ;;

stop)
        exit 0
        ;;
esac
