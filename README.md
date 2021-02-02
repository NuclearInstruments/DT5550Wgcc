## This code is in alfa version. Do not use in on field applications


## Installation

Enter in the linux-x86_64 folder and install the driver files

```
sudo rm /usr/lib/libftd3xx.so
sudo cp libftd3xx.so /usr/lib/
sudo cp libftd3xx.so.0.5.21 /usr/lib/
sudo cp 51-ftd3xx.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
```
## Source
* https://www.ftdichip.com/Drivers/D3XX/Linux/d3xx-linux-x86_64-0.5.21.tar.zip


## Compile library

```
make clean && make
```

In this first version of the library the code is not a real library, it is a class instantiated inside the test.cpp file. 

There is no guardaties that this code really works in all situation.
 
