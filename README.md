# 1. ESP32 LED Dimmer

[![Actions Status](https://github.com/zebrajaeger/esp32-led-dimmer/workflows/CI/badge.svg)](https://github.com/zebrajaeger/esp32-led-dimmer/workflows/CI/badge.svg?branch=master)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Community-orange.svg)

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [1.1. Introducion](#11-introducion)
- [1.2. Simplified Diagram](#12-simplified-diagram)
- [1.3. Simplified Schematic](#13-simplified-schematic)
- [1.4. Motivation](#14-motivation)
- [1.5. TODO](#15-todo)
  - [1.5.1. Software](#151-software)
    - [1.5.1.1. Features](#1511-features)
    - [1.5.1.2. Bugs](#1512-bugs)
  - [1.5.2. Hardware](#152-hardware)
- [1.6. Documentation](#16-documentation)
  - [1.6.1. Installation](#161-installation)
  - [1.6.2. Debug via JTAG](#162-debug-via-jtag)
  - [1.6.3. Storage](#163-storage)
    - [1.6.3.1. Rarely written](#1631-rarely-written)
    - [1.6.3.2. Often written](#1632-often-written)
  - [1.6.4. Software](#164-software)
    - [1.6.4.1. Configuration](#1641-configuration)
    - [1.6.4.2. MQTT](#1642-mqtt)
  - [1.6.5. Hardware 1 / schematic](#165-hardware-1--schematic)
    - [1.6.5.1. µC](#1651-%C2%B5c)
    - [1.6.5.2. Mosfets](#1652-mosfets)
    - [1.6.5.3. FRAM](#1653-fram)
    - [1.6.5.4. PCA9685](#1654-pca9685)
    - [1.6.5.5. DC/DC](#1655-dcdc)
  - [1.6.6. Hardware 2 (external components)](#166-hardware-2-external-components)
    - [1.6.6.1. AC/DC](#1661-acdc)
    - [1.6.6.2. LED stripes](#1662-led-stripes)
- [1.7. Tools I used for development](#17-tools-i-used-for-development)
  - [1.7.1. Software](#171-software)
  - [1.7.2. Hardware](#172-hardware)
  - [1.7.3. Extended Docs](#173-extended-docs)
- [1.8. Example (My implementation)](#18-example-my-implementation)
  - [1.8.1. Prototype](#181-prototype)
  - [1.8.2. Freshly produces PCB](#182-freshly-produces-pcb)
  - [1.8.3. Soldered PCB](#183-soldered-pcb)
    - [1.8.3.1. Known bugs](#1831-known-bugs)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## 1.1. Introducion

This thing is to connect a LED-Lamp (with up to 16 channels and 4096 steps resolution per channel) to a mqtt server

- ESP32 as µC
- PCA9685 as PWM device
  - Frequency of PWM between 25..1500Hz configurable
  - 4096 Brightness values to implement a good mapping for brightness curves
- FRAM MB85RC16 with 10^12 erase/write cycles for values
- Different mosfets possible (low gate volatge ist required because we working with 3,3V)
- Configuration is stored in ESP32 flash wich has 100.000 erase/write cycles
- MQTT control
- Keeps brightness value between power cycles
- Mostly fast connections to WiFi net (sometimes not, maybe a hardware/driver bug)
- If no WiFi connaction can etablished, it opens after 30 seconds a WiFi accesspoint to connect via Smartphone/Notebook to configure connections and credentials
- Simple schematic -  everything is connected via I²C bus
- Made to power on and off, i.e. for a updated light with an oldscool light switch

## 1.2. Simplified Diagram

![Simple diagram](./doc/simple-diagram.svg)

## 1.3. Simplified Schematic

*You can find the complete schematic (and PCB) here:* <https://easyeda.com/zebrajaeger/led_dimmer_v2>

![Simple schematic](./doc/simple-schematic.svg)

## 1.4. Motivation

 I tried some of these configurable projects (EasyESP, Tasmota, ESPHome, ...) but every solution of this list
 has big disadvantages like crashes, very slow wifi connection, no way to store the light brightnes values
 through power-cycles.
 At the end of this frustrating way I decided to write a software that matches (hopefully) all my requirements

## 1.5. TODO

### 1.5.1. Software

#### 1.5.1.1. Features

- [ ] Add new Images.
- 'Homepage' and logo for embedded server, Diode and Poti or something like that.
  - [ ] Logo.
  - [ ] Webapp backend.
  - [ ] Webapp frontend.
- Smart Config [Expressiv doc](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_smartconfig.html), [Google](https://lmgtfy.com/?q=esp32+smartconfig), [Other](https://www.switchdoc.com/2018/06/tutorial-esp32-bc24-provisioning-for-wifi/)
- Art-Net (<https://en.wikipedia.org/wiki/Art-Net>).
  - [X] Implementation.
  - [ ] Configuration.
  - [ ] Documentation.
- MQTT
  - [ ] Port configuration.
  - [ ] Send username, password and port on mqtt connection.
- Timer.
  - [X] Implement
  - [X] for periodic status updates.
  - [ ] Configuration of Status Update period.
- [X] ~~~SSL Connections~~~ (possible?) Not possible.
- Dimming between different light-values with given time.
  - [ ] Linear (simple).
  - [ ] S-curve or log or something like that what looks nice and smooth.

#### 1.5.1.2. Bugs

- [ ] Fix Setting Trouble (UI Update).
- The Reconnector sometimes makes MQTT reconnects but maybe the underlaying TCP-Stack sends a wrong state.
  - [ ] Analyze.
  - [ ] Fix.
- Every 2nd boot Connection trouble analyze
  - [X] Analyze.
  - [X] Fix. Hack -> see https://github.com/espressif/arduino-esp32/issues/2501
- [X] ~~~CSS tuning~~~. No access to the page generators CSS.
- [ ] Image from an undestroyed ESP Node....

### 1.5.2. Hardware

- [X] Make PCB.
  - [X] With consider of jtag and reset + flash-button.
- [ ] 2nd Status LED (another Color).

## 1.6. Documentation

### 1.6.1. Installation

- Install platformio as described here: <https://platformio.org/install/ide?install=vscode>.
- On the PIO Home tab within Visual Studio Code, go to the left toolbar and choose 'Platforms'
- Install 'Expressif 32' platform
- Clone this project into a directory of your choice or download it as a zip-file from here: <https://github.com/zebrajaeger/esp32-led-dimmer/archive/master.zip> (same link as 'Clone or download' -> 'Download ZIP').
- In the PIO Home tab choode 'Open Project' and choose the folder with the project files.

- Edit the platformio.ini and change the line

      upload_port = COM4
  to the serial port where youre device is connected to.

- Click on the 'Alien head' icon on the left toolbar. the 'Project Tasks' List should be visible now.
  
    1. Not needed anymore ~~~Execute 'Upload File System Image'. This uploads the file-system-stuff.~~~
    1. Execute 'Upload'. this uploads the firmware itself.

- If you don't have, change the platformio.ini file and change the 'monitor_port' line with your device port. Connect with the PlatformIO Serial Monitor (the plug icon at the bottom toolbar).

- Because there are no WiFI credentials set, after a timeout of around 30s the device will spawn a accespoint you connect to with your Notebook or Smartphone. The name of the SSID starts with 'ESP'

### 1.6.2. Debug via JTAG

- Olimex JTAG debugger
- Change in

      <userhome>/.platformio/packages/tool-openocd-esp32/share/openocd/scripts/board/esp-wroom-32.cfg 
    the value of adapter_khz to 1MHz (or higher if it works stable):

      adapter_khz 1000

### 1.6.3. Storage

#### 1.6.3.1. Rarely written

We have two types of storage for data.
the first one is the flash which have a livetime of 100.000 erase/program cycles and is used for rarely written data:

- Firmware
- Configuration data from web-interface

#### 1.6.3.2. Often written

The second is a small FRAM, the the MB85RC16 (I²C, 3.3V) (<https://www.fujitsu.com/uk/Images/MB85RC16.pdf>)
which can be written 10^12 times per Byte. It is used to store the brightness values which may be changed very often.

### 1.6.4. Software

// TODO Only as code today, sorry

#### 1.6.4.1. Configuration

#### 1.6.4.2. MQTT

Dictionary:

- 'f': frequency: 24...1500 (in Hz)
- 'd': data
- 'c': channel index (starts with 0)
- 'v': brightness value: 0...4096 as linear PWM brightness steps or a  percentage value as string, i.E. "35.7%". Can be a simple value or an array. Starts with channel index.

There are three topics:

- &lt;TopicPrefix&gt;/alive
  - Device publishes 'true' after connection
  - On timeout the last will message is 'false'
- &lt;TopicPrefix&gt;/status
  - After startup and change settings the device will publish a json status. Also every 5 min. Example (formated):
  ```json
  {
    "device": {
      "name": "esp32-30aea485fe54",
      "ip": "192.168.178.60"
    },
    "light": {
      "frequency": 1500,
      "data": {
        "channel": 0,
        "value": [50,500,50,500,50,500,50,500,50,500,50,500,50,500,50,500]
      }
    }
  }
  ```
- &lt;TopicPrefix&gt;/set (channel values)
    - frequency is optional
    - data is optional
    - channel is the zero based index where the value starts
    - value can be a number(0...4096), a string(0.0%...100.0%) or an array with number(s) and/or string(s)
  ```json
  {
    "frequency": 1500,
    "data": {
      "channel": 0,
      "value": [50,500,50,500,50,500,50,500,50,500,50,500,50,500,50,500]
    }
  }
  ```
- &lt;TopicPrefix&gt;/set (patterns)
    - frequency is optional
    - data is optional
    - value can be a number(0...4096), a string(0.0%...100.0%) or an array with number(s) and/or string(s)
  ```json
  {
    "frequency": 1500,
    "data": {
      "all"
        "all": [0,500,4095]
    }
  }
  ```

### 1.6.5. Hardware 1 / schematic

I made the scheamtic and pcb with EASYEDA from JLPCB. A public link to this project is here: <https://easyeda.com/zebrajaeger/led_dimmer_v2>

#### 1.6.5.1. µC

- ESP32 (ESP32-WROOM-32)
  - Dual core, 240MHz  
  - Datasheet: <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>
  - Technical Reference Manual: <https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>
  - Instruction set: <https://www.espressif.com/sites/default/files/documentation/esp32_at_instruction_set_and_examples_en.pdf>

For Development:

ESP32 devkit v1

![PCA9685 Module](./doc/esp32_devkit_v1.webp)

#### 1.6.5.2. Mosfets

Every N-Channel Mosfet that is sure fully on @ 2.5V gate voltage

I use:

- IRLML6344 (37mΩ @ Ug=2.5V) <https://www.infineon.com/dgdl/irlml6344pbf.pdf?fileId=5546d462533600a4015356689c44262c>
- AO3400 (53mΩ @ Ug=2.5V) <http://www.aosmd.com/pdfs/datasheet/ao3400.pdf>

#### 1.6.5.3. FRAM

Requirements:

- I²C, min 400kHz
- 3.3V VDD
- Single adress byte (bigger FRAMs using two bytes for the bigger address space)

I use:

- MB85RC16 (16K=2K×8) (<https://www.fujitsu.com/uk/Images/MB85RC16.pdf>)

#### 1.6.5.4. PCA9685

- TSSOP28
- <https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf>

For development: PCA9685 module from ebay:
![PCA9685 Module](./doc/PCA9685-module.webp)

#### 1.6.5.5. DC/DC

- ESP needs up to 0.5A @3.3V (peak value). Average current consumption is 80mA.

I use 24V LED so some DC/DC modules from ebay/aliexpress are fine.

- Cheap and small are MP1584 based buck convertert (search for MP1584 on Ebay, Aliexpress or Amazon)
  - 4.5-28V to 0.8-20V and 3A (manually adjust it to 3.3V ad glue the trimmer)
  - <https://www.monolithicpower.com/en/documentview/productdocument/index/version/2/document_type/Datasheet/lang/en/sku/MP1584/document_id/204>

### 1.6.6. Hardware 2 (external components)

#### 1.6.6.1. AC/DC

I decidet to use a high efficient transformer from Enertex (<http://www.enertex.de/e-led-powersupply.html>) which is explicit designed for PWM applications. The only disadvantage is the non waterproof design.

#### 1.6.6.2. LED stripes

I use high CRI 24V LED stripes from <https://shop.led-studien.de/> with aluminium profiles as case and heatsink.

<a id="example-schematic"></a>

## 1.7. Tools I used for development

### 1.7.1. Software

- Visual Studio Code + PlatformIO
- EasyEDA to create the PCB
  - JLPCB to build the PCBS
  - AISLER <https://aisler.net> to build PCBs (more environment friendly than JLPCB because no delivery via aeroplane and default leadfree. JLPCB also provides leadfree PCBs but then the whole costs are higher than the tax-free threshold duty of 22€ in 2019).
- Gimp to work with images <https://www.gimp.org/>
- Sometimes Putty to connect to serial port (Serial console implementation of PlatformIO seems not to stable)

### 1.7.2. Hardware

For details see the hardware section.

- Esp32 DevKit v1
- PCA9685 module from ebay
- Breadboard
- Prototype PCB for MOSFETs (I forgot to order adapter PCBs)
- SOP-8 Adapter PCB from ebay for FRAM

### 1.7.3. Extended Docs

- Github Markdown: <https://github.github.com/gfm/>
- Github Markdown Code Blocks: <https://help.github.com/en/github/writing-on-github/creating-and-highlighting-code-blocks>

## 1.8. Example (My implementation)

For source see here: <https://easyeda.com/zebrajaeger/led_dimmer_v2>

### 1.8.1. Prototype

![Prototype](./doc/prototype_on_breadboard_2048.webp)

### 1.8.2. Freshly produces PCB

Bottom, Top

![PCB2.0](./doc/pcb_2.0_2048.webp)

### 1.8.3. Soldered PCB

Some connectors are not soldered because I don't need them.

No critical bugs found. Everything works as expected!

#### 1.8.3.1. Known bugs

- R29 must not be placed or fuse for SP. Otherwise, the flash-voltage is 1.8V instead 3.3V and the device cannot boot.
  - see the GPIO12(MTDI) section here: <https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection>  
  - Other option is to change the fuse 'XPD_SDIO_FORCE'. Search for 'VDD_SDIO ' here: <https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/efuse.html>
- There a two resisitors named R2 (R2 and R2(*))

![PCB2.0](./doc/pcb_20_soldered_2048.webp)