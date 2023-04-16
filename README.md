# Bally-Pinball-Clock

![clock](https://user-images.githubusercontent.com/60443687/232238852-93e0d1fc-f36c-4a17-a63e-6b94fe91d550.JPG)

![stuff](https://user-images.githubusercontent.com/60443687/232238864-b302072b-93ec-482a-96a0-a83b6c17f098.jpg)

Instructions for building an Atmega328 based board that plugs into a Bally 6-digit pinball display that shows the time.

Basic parts needed:

* Atmega328 IC and socket (aka Arduino)
* RTC module (clock keeps time when not powered on)
* Nixie Tube power supply (optional, supports original plasma displays)
* Small stripboard
* 20 pin .156 female J1 connector and female pins
* 7805 Voltage regulator
* 2 momentary push buttons
* 2 LEDs
* 2 220 Ohm resistors
* 1 10k Ohm resistor
* 2 10 uF capacitors
* 2 22 pF capacitors
* 16 MHz clock crystal
* DC power jack
* 12V/1A plug-in power supply
* misc electronics -- wire/headers/solder 

<h1>Wiring</h1>



<h1>High Voltage Power Supply</h1>

![wiring](https://user-images.githubusercontent.com/60443687/232325776-770f1fc3-383e-423f-9419-03286d210708.png)

The HVPS is available here:
https://threeneurons.wordpress.com/nixie-power-supply/hv-supply-kit/

![hvpsschmatic](https://user-images.githubusercontent.com/60443687/232324982-f77bae17-73c5-48b1-93c8-24ee0421cd89.jpg)

<h1>DS1307 Real Time Clock Module</h1>

<img src="https://user-images.githubusercontent.com/60443687/232324989-c929c078-a96e-4805-afff-f8824a594c5f.jpg" width="100" />

<!--![rtc](https://user-images.githubusercontent.com/60443687/232324989-c929c078-a96e-4805-afff-f8824a594c5f.jpg)-->
