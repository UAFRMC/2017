#!/bin/bash
sudo rm -rf /tmp/ar.tar.xz /tmp/arduino-1.6.5-r5
wget downloads.arduino.cc/arduino-1.6.5-r5-linux64.tar.xz -O /tmp/ar.tar.xz -o /dev/null
sudo rm -rf /usr/share/arduino-1.6.5-r5 /usr/bin/arduino
sudo tar -xJf /tmp/ar.tar.xz -C /usr/share/
sudo rm -rf /tmp/ar.tar.xz /tmp/arduino-1.6.5-r5
sudo ln -s /usr/share/arduino-1.6.5-r5/arduino /usr/bin/arduino
sudo cp /usr/share/arduino-1.6.5-r5/arduino.desktop /usr/share/applications/
sudo sed -i 's/FULL_PATH/\/usr\/share\/arduino-1.6.5-r5/g' /usr/share/applications/arduino.desktop