#!/bin/sh
sudo python /home/tc/pinclock.py &
sleep 1
sudo wpa_supplicant -B -D wext -i wlan0 -c /opt/wpa_configure.conf &
sleep 1
sudo /sbin/udhcpc -b -i wlan0 -x hostname:$(/bin/hostname) -p /var/run/udhcpc.wlan0.pid &
sudo cp /home/tc/Phoenix /etc/localtime
