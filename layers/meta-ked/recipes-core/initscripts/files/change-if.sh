#!/bin/sh

ip link show eth1
if test "$?" != "0"; then
    echo "No second ethernet interface found"
    exit 0
fi

# save device status
ETH0_ISUP=$(ip link show eth0 up)
ETH1_ISUP=$(ip link show eth1 up)

grep -q "root=/dev/nfs" /proc/cmdline 

# only rename interface eth0 when nfsroot is not used
if [ $? -eq 0 ]; then
	exit 0
fi

ip link set eth0 down
ip link set eth1 down

# rename devices
ip link set eth0 name eth0-old
ip link set eth1 name eth0
ip link set eth0-old name eth1

# restore device status
# Remember: They're exchanged now
if test -n "${ETH0_ISUP}"; then
    echo "Restoring eth1 to UP state"
    ip link set eth1 up
fi
if test -n "${ETH1_ISUP}"; then
    echo "Restoring eth0 to UP state"
    ip link set eth0 up
fi
    
