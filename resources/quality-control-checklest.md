### Cyclops Quality Control Procedure
The following procedure is to be performed on boards purchased from an external vendor and sold in a kit or fully assembled state.

Board Serial Number:
Date:
Initials:

#### Setup
- Insert alligator clip across power switch solder points
- Insert device into PCB clamp
- Power from 15V, 1.5A capable bench-top power supply.

- [ ] Power indicator LED turns on.

#### DC Levels
- Using a multimeter, probe the 12V, 2.5V, -5V, and -1.25V test points

- [ ] 12V good
- [ ] -5V good
- [ ] -1.25V good
- [ ] While probing the 2.5V test point, use a ESD-safe screwdriver on the trimpot to get exactly 2.5V. 

#### Dynamic characteristics
- Set MDO3000's AFG to produce 0-2.5V, 100 Hz, 10% duty cycle square wave terminated in 50 Ohms.
- Insert test LED/amplified photodiode into banana sockets and AUX BNC port.
- Insert AFG output of MDO3000 output into VCTL BNC port of device with a 50 Ohm series coax terminator.
- Bring CURR output of device to Ch1 of MDO3000
- Bring photodiode output of device to Ch2 of MDO3000
- Triggering on CURR Channel set scope to measure rise and fall times
- Bring front panel potentiometer to 100% position.
- Input switch to EXT source

- [ ] Examine waveshape and rise/fall times in current FB mode. Rise/fall times < 300 ns. No ringing on waveform.
- [ ] Return input switch to OFF (middle) position

#### Overcurrent indication
- Bring gain potentiometer to full on position
- Briefly tap on the TEST button.

- [ ] Ensure that the >1A indicator LED turned on during pulse.
- [ ] __RETURN GAIN POT. TO ZERO POSITION__

#### Finish
- [ ] Remove all power connectors. 
- [ ] Remove alligator clip.
- [ ] Initial and serial number the board using sharpie on the large power trace on the right side of the board.
- [ ] Enter board serial number into the spreadsheet.