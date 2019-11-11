## TODO

* make PCB
  * add schematic
* add description

## Facts

* ESP32
* Configurable via webbrowser (opens Accesspoint if needed)
* OTA updateable
* MQTT control
* Keeps brightness and frequency persisten

## Storage

We have two types of storage for data.
the first one is the flash which have a livetime of 100.000 erase/program cycles and is used for rarely written data:
* program
* configuration over the web-interface

The second is a small FRAM, the the MB85RC16 (I²C, 3.3V) (https://www.fujitsu.com/uk/Images/MB85RC16.pdf)
which can be written 10^12 times per Byte. It is used to store the brightness values which may be changed very often.
