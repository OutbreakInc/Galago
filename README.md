# Galago by Outbreak, Inc.
A revolutionary microcontroller platform featuring an ARM Cortex chip, clever features, great open source tools, a tiny footprint and built-in debugger.  Galago enables rapid hardware device prototype development that scales to volume production without the costs and complexity normally involved with hardware.

[Learn more about Galago](http://outbreak.co/galago) | [See the Galago campaign on Kickstarter](http://www.kickstarter.com/projects/kuy/galago-make-things-better)

[![Photo of Galago](https://github.com/OutbreakInc/Galago/blob/master/Hardware/Galago/photos/Galago-0BAB0410-small.jpg?raw=true)](https://github.com/OutbreakInc/Galago/blob/master/Hardware/Galago/photos/Galago-0BAB0410.jpg?raw=true)

### Hardware designs
All hardware designs, unless otherwise specified, are released under the [Creative Commons CC BY-SA-3.0 license](http://creativecommons.org/licenses/by-sa/3.0 "Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0)").

Hardware design files are in CadSoft EAGLE(tm) 5.10 binary format.  EAGLE is available from CadSoft [(US link)](http://cadsoftusa.com) [(DE link)](http://cadsoft.de)

##### Audioblock (Audio App Board)
[![Photo of Audioblock App Board](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/AudioAppBoard/photos/AudioAppBoard-0BAC0606-small.jpg?raw=true)](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/AudioAppBoard/photos/AudioAppBoard-0BAC0606.jpg?raw=true)

The Audio App Board features a 16-bit, 48KHz stereo audio digital-to-analog converter driving two 1.5W class-D audio amplifiers and a low-noise switch-mode power supply with barrel jack.
+ [Audio App Board design files](https://github.com/OutbreakInc/Galago/tree/master/Hardware/AppBoards/AudioAppBoard/boards)
+ [Audio App Board schematic](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/AudioAppBoard/boards/AudioAppBoard-0BAC0603-schematics.pdf?raw=true)

##### Arduino Shield Adaptor
![Photo of Arduino Shield Adaptor prototype](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/ArduinoShieldAdaptor/photos/ArduinoShieldAdaptor-0BAC0801-2-small.jpg?raw=true)

The Arduino Shield Adaptor offers a way to connect supported Arduino(R) shields to Galago plus a rugged but efficient switch-mode power supply with barrel jack. The board offers compatiblity with 3.3V-friendly Arduino Shields, and may work with some 5V shields depending on their I/O-level and power supply requirements.  Check the specs of a shield prior to connecting.
+ [Arduino Shield Adaptor design files](https://github.com/OutbreakInc/Galago/tree/master/Hardware/AppBoards/ArduinoShieldAdaptor/boards)
+ [Arduino Shield Adaptor schematic](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/ArduinoShieldAdaptor/boards/ArduinoShieldAdaptor-0BAC0801-schematics.pdf?raw=true)

##### Bluetooth App Board
![Photo of Bluetooth App Board prototype](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/BluetoothAppBoard/photos/BluetoothBoard-0BAC0701-2-small.jpg?raw=true)

The Bluetooth(R) App Board hosts a Bluetooth module implementing the SPP (Serial Port Profile) connected to Galago's UART.  As a low-energy design, it relies on Galago's built-in power-supply. The Bluetooth App Board also features a breadboard-like 0.1" prototyping area that breaks out each Galago I/O pin.
+ [Bluetooth App Board design files](https://github.com/OutbreakInc/Galago/tree/master/Hardware/AppBoards/BluetoothAppBoard/boards)
+ [Bluetooth App Board schematic](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/BluetoothAppBoard/boards/BluetoothBoard-0BAC0702-schematics.pdf?raw=true)

##### Etherblock (Ethernet App Board)
[![Photo of Ethernet App Board](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/EthernetAppBoard/photos/EthernetAppBoard-0BAC0404-small.jpg?raw=true)](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/EthernetAppBoard/photos/EthernetAppBoard-0BAC0404.jpg?raw=true)

The Ethernet App Board features the ENC28J60 Ethernet controller with integrated MAC and PHY, plus a switch-mode power supply with barrel jack input.
+ [Ethernet App Board design files](https://github.com/OutbreakInc/Galago/tree/master/Hardware/AppBoards/EthernetAppBoard/boards)
+ [Ethernet App Board schematic](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/EthernetAppBoard/boards/EthernetAppBoard-0BAC0403-schematics.pdf?raw=true)

##### LED Driver App Board
![Photo of LED Driver App Board prototype](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/LEDAppBoard/photos/LEDAppBoard-0BAC0303-2-small.jpg?raw=true)

The LED Driver App Board is powered by the TI TLC5940 16-channel, 12-bit constant-current PWM controller.  This controller enables 4096-level brightness control of LEDs or precise control of up to 16 PWM-controlled servos.
+ [LED Driver App Board design files](https://github.com/OutbreakInc/Galago/tree/master/Hardware/AppBoards/LEDAppBoard/boards)
+ [LED Driver App Board schematic](https://github.com/OutbreakInc/Galago/blob/master/Hardware/AppBoards/LEDAppBoard/boards/LEDAppBoard-0BAC0304-schematics.pdf?raw=true)
