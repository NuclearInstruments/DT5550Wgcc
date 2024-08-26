## This code is in alfa version. Do not use in on field applications


## Installation of FTDI drivers

Download latest ftdi drivers from

https://ftdichip.com/wp-content/uploads/2024/07/libftd3xx-linux-x86_64-1.0.16.tgz

extract them

```
tar xvf libftd3xx-linux-x86_64-1.0.16.tgz
```

enter in the folder and install the FTDI drivers

```
sudo rm /usr/lib/libftd3xx.so
sudo cp libftd3xx.so /usr/lib/
sudo cp libftd3xx.so.0.5.21 /usr/lib/
sudo cp 51-ftd3xx.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
```

## Compile library 

```
make clean && make
```

In this first version of the library the code is not a real library, it is a class instantiated inside the test.cpp file. 

There is no guardaties that this code really works in all situation.
 
## Execute test

Edit the serial number of your device in test.cpp file.

Recompile library and test
```
make clean && make
```

Run using local library
```
export LD_LIBRARY_PATH=$(pwd) && ./test
```

## Install the library in the system


```
sudo make install
```
