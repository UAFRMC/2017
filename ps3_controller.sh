#!/bin/bash
grep -h "^deb.*falk-t-j/qtsixa" /etc/apt/sources.list.d/* > /dev/null 2>&1
if [ $? -ne 0 ]
then
	sudo add-apt-repository -y ppa:falk-t-j/qtsixa
	sudo apt-get update
	sudo apt-get install -y qtsixa
fi
echo "PLUGIN the ps3 controller"
read
sudo sixad
echo "UNPLUG the ps3 controller"
read
sudo service sixad restart