# 1. ESP32 LED Dimmer

## 1.1. Introducion

This thing is to connect a LED-Lamp (with up to 16 channels and 4096 steps resolution per channel) to a mqtt server

* ESP32 as µC
* PCA9685 as PWM device
  * Frequency of PWM between 25..1500Hz configurable
  * 4096 Brightness values to implement a good mapping for brightness curves
* FRAM MB85RC16 with 10^12 erase/write cycles for values
* Different mosfets possible (low gate volatge ist required because we working with 3,3V)
* Configuration is stored in ESP32 flash wich has 100.000 erase/write cycles
* MQTT control
* Keeps brightness value between power cycles
* Mostly fast connections to WiFi net (sometimes not, maybe a hardware/driver bug)
* If no WiFi connaction can etablished, it opens after 30 seconds a WiFi accesspoint to connect via Smartphone/Notebook to configure connections and credentials
* Simple schematic -  everything is connected via I²C bus
* Made to power on and off, i.e. for a updated light with an oldscool light switch

## 1.2. Simplified Diagram

![Simple diagram](./doc/simple-diagram.svg)

## 1.3. Simplified Schematic

For a complete schematic, take a look at the [example section of hardware documentation](#example-schematic)

![Simple schematic](./doc/simple-schematic.svg)

## 1.4. Motivation

 I tried some of these configurable projects (EasyESP, Tasmota, ESPHome, ...) but every solution of this list
 has big disadvantages like crashes, very slow wifi connection, no way to store the light brightnes values
 through power-cycles.
 At the end of this frustratig way I decided to write a software that matches (hopefully) all my requirements

## 1.5. TODO

- [ ] Consider username and password on mqtt connection.
- [ ] Implement mqtt port configuration.
- [ ] The Reconnector sometimes makes MQTT reconnects but maybe the underlaying TCP-Stack sends a wrong state. To be analyzed.
- [ ] More documentation.
- [ ] Make PCB.
  - [ ] With consider of jtag and reset + flash-button.
- [ ] Implement timer for periodic status updates.
  - [ ] And configuration of period time
- [ ] CSS tuning
- [ ] 'Homepage' and logo for embedded server
- [ ] SSL Connections (possible?)

## 1.6. Documentation

// TODO

### 1.6.1. Storage

#### Rarely written

We have two types of storage for data.
the first one is the flash which have a livetime of 100.000 erase/program cycles and is used for rarely written data:

* Firmware
* Configuration data from web-interface

#### Often written

The second is a small FRAM, the the MB85RC16 (I²C, 3.3V) (<https://www.fujitsu.com/uk/Images/MB85RC16.pdf>)
which can be written 10^12 times per Byte. It is used to store the brightness values which may be changed very often.

### 1.6.2. Hardware

#### 1.6.2.1. Mosfets

Every N-Channel Mosfet that is sure fully on @ 2.5V gate voltage

I use:

* IRLML6344 (37mΩ @ Ug=2.5V) <https://www.infineon.com/dgdl/irlml6344pbf.pdf?fileId=5546d462533600a4015356689c44262c>
* AO3400 (53mΩ @ Ug=2.5V) <http://www.aosmd.com/pdfs/datasheet/ao3400.pdf>

### 1.6.3. FRAM

Requirements:

* I²C, min 400kHz
* 3.3V VDD
* Single adress byte (bigger FRAMs using two bytes for the bigger address space)

I use:

* MB85RC16 (16K=2K×8) (<https://www.fujitsu.com/uk/Images/MB85RC16.pdf>)

### 1.6.4. PCA9685

* TSSOP28
* <https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf>

### 1.6.5. DC/DC

* ESP needs up to 0.5A @3.3V (peak value)

I use 24V LED stripes so some DC/DC modeles from ebay/aliexpress are fine.

* Cheap and small are MP1584 based buck convertert (serch for MP1584 by Ebay Aliexpress or Amazon)
  * 4.5-28V to 0.8-20V and 3A (manually adjust it to 3.3V ad glue the trimmer)
  * <https://www.monolithicpower.com/en/documentview/productdocument/index/version/2/document_type/Datasheet/lang/en/sku/MP1584/document_id/204>

<a id="example-schematic"></a>

### 1.6.6. Example schematic

// TODO