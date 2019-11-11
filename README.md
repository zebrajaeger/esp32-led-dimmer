## TODO

* make PCB
  * add schematic
* add description

## Facts

* 24V, 1A/CH (depend from cooling and type of mosfet).
* ESP32-Wroom-32 module; dual core 240MHz for fast response.
* Configurable via webbrowser (opens Accesspoint if needed).
* OTA updateable.
* MQTT control (tested with mosquitto).
* Keeps brightness and frequency persistent so it can be switched on and off to be energy efficient.
* FRAM to store brightness values (Livetime 10^12 erase/write cycles), MB85RC16 (I²C, 3.3V) (https://www.fujitsu.com/uk/Images/MB85RC16.pdf)
* PCA9685 as PWM modulator

