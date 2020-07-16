
# Arduino based 433MHz transmitter

This Arduino sketch imitates an RF remote that uses OOK (On Off Keying). Specifically it was meant to clone a Dooya DC1602 remote for shades. It is meant to be linked to this Homebridge plugin for control of shades.

### [Homebridge Dooya Plugin](https://github.com/rjcarslon49/homebridge-dooya-0)

## Hardware
The main hardware is an [Arduino Uno rev3](https://store.arduino.cc/usa/arduino-uno-rev3). Attached to that you need a 433 MHz transmitter. 433 MHz transmitters and receivers are sold in pairs. I bought this pair, [RioRand](https://amazon.com/gp/product/B00HEDRHG6/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1), from Amazon and discarded the receiver. The transmitter is tiny and has 4 pins. Two are ground and VCC. These connect to the GND and 5V pins on the Arduino. The data pin connects to the pin 12 on the Arduino. The AN pin, antenna I just connected a loose piece of wire to.

## Encoding

The basic technology is [On-Off Keying with Pulse Width Modulation](https://en.wikipedia.org/wiki/On%E2%80%93off_keying). The transmitter sends a signal at 433.92 MHz. Sending the signal is ON, not sending it is OFF. Various combinations of an ON length and an OFF length encode the message. In the DC1602, a zero is encoded by a 720 µs transmission (ON) followed by a 360 µs non-transmission (OFF). A one is 360 µs on, 720 µs off. The Arduino simply turns on pin 12 to transmit and turns it off to pause tranmission.

DC1602 messages are grouped in "rows". Each row is 40 zero or one bits, preceeded by a beginning of row marker (4740 µs, 6220 µs). The end of a row is marked by a 9000 µs OFF and the end of a message is marked by a further 6000 µs OFF.

In my DC1602 at least, the 40 bit rows consist of a 28 bit fixed part (bit 0..27) that is the same in all rows. Bits 28-31 are the channel numbers. The last 8 bits are command bits (bits 32-39). Open and Close for some reason have two 
command codes that are sent. Each command is sent multiple 
times as rows in a message.

The commands 
Command  | Coding | Transmissions
-------- |:---------:|:------:
Open | EE | 3
. | E1 | 6
Stop | AA | 3
Close | CC | 3
. | C3 | 6

The channels are numbered oddly, F is channel 0, E is channel 1, etc.

So, if the fixed part is A1B2C3D, then a close message for channel 3 would consist of

* A1B2C3DCEE 3 times
* A1B2C3DCE1 5 times

I found that the number of retransmissions (3, 5) is not fixed. I increased the retransmissions to (5, 5) in my installation to increase reliability.

## Decoding Your Remote

It is pretty certain that your remote does not use the same coding that mine does. To decode yours you need a software defined radio and a decoding program. I used the program [rtl_433](https://github.com/merbanan/rtl_433). 

A radio is required. I used the [NooElec NESDR](https://amazon.com/gp/product/B01GDN1T4S/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&psc=1). It plugs into a USB port.

First try this Flex Decoder spec. Start up rtl_433 with this command and then operate your remote.

    rtl_433 -R 0 -X 'n=Dooya,s=360,l=720,r=14844,g=0,t=0,y=4740'
`
If it decodes the messages, you will see the rows displayed in hex and binary. Do this for enough of your remote commands for you to figure out the fixed part, the channel number and the commands.

If the spec above does not work for you, then you need to run an analysis first.

## RTL_433 Analysis

Use the "-A -R 0" options to discover the basic structure of the messages, the on-off timings and rows. After setting up the program and radio, run the program with -A and -R 0, operate the remote and it will spit out the specs for the OOK encoding.

Using that information, use rtl_433 as above to decode the messages from your remote and find the command and channel encodings.

A Flex Decoder can be used either with -R 0 -X <spec> or it can be put into the rtl_433.conf file like this 

    decoder 'n=Dooya,s=360,l=720,r=14844,g=0,t=0,y=4740'

