__Cyclops__ is an open-source, high-power LED driver that enables extremely
precise control of light power for optogenetic stimulation. The circuit was
developed by Jon Newman while in Steve Potter's lab at Georgia Tech in order to
complete his thesis work, which required the delivery of ultra-precise,
continuously time-varying light waveforms for optogenetic stimulation [1].
This was, and still is, not possible with commercial hardware for optogenetic
stimulation. Since its first use, the circuit has been continuously improved in
terms of speed, precision, programmability, and ease of use. This document
provides construction, usage, and performance documentation for the Cyclops LED
driver. This document evolves with the repository. To view old revisions,
checkout tags or old commits using their SHA.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

  - [Features](#features)
  - [TODO](#todo)
  - [Performance Specifications](#performance-specifications)
- [](#)
  - [Usage](#usage)
    - [Feedback Modes](#feedback-modes)
      - [Current Feedback Mode](#current-feedback-mode)
      - [Auxiliary Feedback Mode](#auxiliary-feedback-mode)
    - [Stimulus Generation Options](#stimulus-generation-options)
  - [Construction](#construction)
    - [Components](#components)
    - [PCB](#pcb)
    - [Enclosure](#enclosure)
    - [Circuit testing](#circuit-testing)
  - [License](#license)
    - [Hardware Licensing](#hardware-licensing)
    - [Software Licensing](#software-licensing)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->



### Features
**Circuit Features**
- Ultra-precise
- High power
- Up to 1.5A per LED
- Wide bandwidth
  - ~2.5 MHz -3 dB bandwidth
  - <200 ns 1.0A rise-time
- Current and optical feedback modes
- Built-in waveform generation
- Over-current protection
- Modular
  - Arduino compatible
  - 4 synchronizable optical channels
  - Accepts external analog, gate, or trigger inputs

**Stimulus Generation Options**
- External stimulus sequencer
- External digital trigger
  - TTL logic level
- External analog waveform generator
  - 0-5V analog signals
- Internal 12-bit DAC
  - Synchronized across up to 4 drivers
  - Powerful Arduino library
  - Programmable triggering logic
  - Respond to USB input
	  
### TODO 
**Revision 3.6**
- [ ] Get [DOC.md](DOC.md) in working order
- [ ] Create an fiber coupled LED interface with integrated optical power
  measurements
  - [In progress](https://github.com/jonnew/cyclops/tree/master/photodiode)
- [ ] High side current sense
- [ ] Arduino gets replaced with a
  [Teensy](https://www.pjrc.com/teensy/teensy31.html).

**Revision 3.5B**
- [ ] Get the board on CircuitHub
- [ ] Get the device on the Open Ephys store
- [ ] The rear panel holes seems to have slight vertical offsets
  - LED jacks: 1 mm to low 
  - Expansion header: 1 mm to low 
  - Aux/curr switch: 0.25 mm to low
  - *EDIT* After further inspection it seems like the expansion header might
    be 90% of the problem. Recheck before making changes.
- [x] The LED polarity labels on the panel are reversed 
- [x] Sometimes, because of intrinsic tolerance issues, the 5.1V zener's
  reverse breakdown is too low and it affects valid signals in the 0-5V range.
  Get a more reasonable value, e.g. 6V, zener.
  - *EDIT* This was due to a bad zener. Ignore this change. 
- [x] Trade out the LT1964A for a LT1764A, which has a 3A output capability and
  a will allow the circuit to read 1.5A instead of crapping out at 1.25 since
  the quiescent circuit is current hog.
- [x] Put some extra through holes in for wire-to-board access to +/- LED,
  digital GND, and I2C bus for EEPROM on thorlabs leds.

### Performance Specifications
TODO

---
### Usage
#### Feedback Modes
The main functional component of the device is a feedback assisted, power,
enhancement-mode N-MOSFET
([IRF510](http://www.vishay.com/docs/91015/sihf510.pdf)). Current is drawn from
the source pin of the FET in accordance with one of two feedback modes: current
feedback or auxiliary feedback. In both modes, the FET acts as a variable
resistor whose resistance is changed in inverse relation to the gate voltage.
The difference between the two modes is in how the gate voltage is regulated.
- In **current feedback mode**, the gate voltage is adjusted such that the
  voltage drop across the sense resistor (1 ohm in the schematic) is equal to a
  supplied reference voltage.
- In **auxiliary feedback mode**, some external voltage that has a positive and
  monotonic relationship with optical power is provided as a feedback signal.
  This is feedback voltage is typically supplied by an amplified photodiode

##### Current Feedback Mode
Using the circuit in current feedback mode ensures that the forward diode
current across the LED is precisely modulated according the voltage at the VREF
pin. This configuration is a standard method for driving LEDs because the
relationship between current and LED irradiance is smooth and monotonic. This
means that more current across the LED will generate more light power (while
staying within the LED's maximum ratings, of course). However, the relationship
between current and irradiance is not linear. For most LEDs, it looks like a
logarithmic function. Additionally, the efficiency of the LED is inversely
related to its temperature. So, as the LED operates and heats up, the amount of
light it produces drops even when the current is held constant. The severity of
an LED's temperature dependence and current/irradiance nonlinearity depend on
the type of LED (roughly, the color and who made it). These properties should
be clearly documented in the LED's data sheet. With a quality LED and proper
thermal management, the effects of temperature and static current/irradiance
nonlinearity are fairly minimal and can be ignored in most situations.

<div style="text-align:center">
  <img src ="https://raw.githubusercontent.com/jonnew/cyclops/master/art/current_feedback_diagram.png" style="width: 200px;" />
  <br>
  <b>Fig. 1</b> <i>Current feedback configuration.</i>
</div>

##### Auxiliary Feedback Mode
When extremely stable, linear control of light power is required, the auxiliary
feedback input can be used to used to compensate for the temperature dependence
and static nonlinearity of the current/irradiance relationship of the LED. For
example, when the auxiliary voltage is supplied by an amplified photodiode that
is somewhere indecent to radiation from the LED, or is sampled from the fiber
transporting LED light, the gate voltage is adjusted such that the measured
light power matches a DAC-supplied reference voltage.  This is the case in the
circuit diagram. This configuration is referred to as optical feedback mode.
The [PDA36A](https://www.thorlabs.com/thorProduct.cfm?partNumber=PDA36A)
adjustable amplified photodiode from Thorlabs is a good option for supplying
optical feedback. However, you can make your own amplified photodiode for a
fraction of the price, and a design is included within the cyclops repository.
Optical feedback completely linearizes the relationship between a supplied
reference voltage and the light power produced by the LED by compensating for
the current/irradiance nonlinearities and temperature dependence.

<div style="text-align:center">
  <img src ="https://raw.githubusercontent.com/jonnew/cyclops/master/art/optical_feedback_diagram.png" style="width: 200px;" />
  <br>
  <b>Fig. 1</b> <i>Optical feedback configuration.</i>
</div>

#### Stimulus Generation Options
There are three ways to generate light signals using the driver. The behavior
of each of these options is dependent on the feedback mode being used. The
behavior of each input option is described in relation to the feedback mode of
the driver.

1. `TEST` The test button is always available and will override all other input
   modes.

    __Behavior__:
  - *Current Feedback*: Source the current specified by the MAX CURR. dial.
  - *Auxilary Feedback*: Generate the optical power specified by the h * mW
    level that is  specified by the MAX POWER dial. The intensity of the LED
    will be dependent on the auxiliary feedback signal used which defines the
    'h' parameter. 

2. `EXT` This option is engaged when the SOURCE switch is moved to the EXT
position and user supplied voltage waveforms are present at the EXT BNC input.
If the user attempts to supply more than 5V to the EXT input, the circuit will
clamp the input signal to 5V.

    __Behavior__:
  - *Current Feedback*: Source the current specified by (EXT Voltage / 5V) *
    MAX CURR. 
  - *Auxilary Feedback*: Generate the optical power specified by (EXT
    Voltage/5V) * h  * mW. The intensity of the LED will be dependent on the
    auxiliary feedback signal used which defines the 'h' parameter. 

3. `DAC` - The internal DAC is engaged when the SOURCE switch is moved to the
DAC position and can be used to generate pre-programmed waveforms and waveform
sequences triggered by a digital pulse to the TRIG input. This feature relies
on optional Arduino installation and programming the device using its API.

    __Behavior__:
  - *Current Feedback*: Source the current specified by (DAC Voltage / 5V) *
    MAX CURR. 
  - *Auxilary Feedback*: Generate the optical power specified by (DAC
    Voltage/5V) * h  * mW. The intensity of the LED will be dependent on the
    auxiliary feedback signal used which defines the 'h' parameter. 


### Construction 
Before you build a board, its recommended that you get in touch
[@jonnew](https://github.com/jonnew) for a couple reasons:
- We are interested in getting feedback on which portions of this documentation
  need improvement.
- The cyclops is a fairly complex PCB. If you have doubts, let us know so we
  can help you with the build.

#### Components 
The bill of materials (BOM) required to build a 1,2,3, or 4 devices is
available on [this google
doc](https://docs.google.com/spreadsheets/d/1YQR_ujrZgILNx3XjomLKWgzDvirwKrKaRbVVzmBgk-s/edit?usp=sharing).
Most of the parts can be purchased from Digikey, but there are a few components
that need to be bought from other sources such as Newark, Adafruit, and Samtec.
All vendor, part number, and quantity information is listed on the BOM. In some
cases, two suppliers are provided for a single part to make alternative sources
available. As a convenience, the spreadsheet contains links to a pre-populated
Digikey cart (which are optimized for quantity discounts).

The PCB can be purchased from OSH park [here]() or Seed Studio [here](). This
is a four layer PCB. Note that OSH park sells a minimum of 3 boards per order
and Seed studio sells a minimum of 5 boards per order. PCB stencils, useful for
those performing reflow or hot-air soldering, are available from OSH stencils
[here](). If you plan to hand solder the board, then you do not need to
purchase these stencils.

The BOM includes several optional components, which are not in the
pre-populated Digikey cart. These include:
- __An extruded aluminum enclosure__, which houses the completed board. The
  enclosure is recommended because the large voltages and current transients
  used to drive high power LEDs can cause capacitive and inductive interference
  with nearby recording equipment. Acrylic front and rear panels can be
  purchased from Ponoko using the links supplied in the BOM. The instructions
  below show how these plastic pieces are modified to provide proper electrical
  shielding.
- An __M8-4 connector__. This is a rather expensive connector that allows
  cyclops to drive [Thorlabs LED modules]() or [Doric LED
  modules](http://doriclenses.com/life-sciences/243-led-modules).
    
#### PCB The cyclops PCB can be constructed by purchasing from one of the
pre-uploaded options:
  - [OSH Park]() - made in America, excellent quality.
  - [Seeed Studio]() - made in China, good quality.

Alternatively, the [gerber files](https://en.wikipedia.org/wiki/Gerber_format)
in the cyclops repo
([link](https://github.com/jonnew/cyclops/tree/master/cyclops/gerber)) can be
uploaded to the PCB fabrication service of your choice. The layer of each
gerber file is identified by its file extension:

     *.GBO = board outline 
     *.GTS = top solder mask 
     *.GBS = bottom solder mask
     *.GTO = top silk screen 
     *.GBO = bottom silk screen 
     *.GTL = top copper
     *.G2L = inner layer 2 copper 
     *.G3L = inner layer 3 copper 
     *.GBL = bottom copper 
     *.XLN = drill hits and sizes

#### PCB Assembly 

Below is an example of finished Cyclops board (ver 3.5A). Let's begin!

    <div style="text-align:center"> <img src
    ="https://github.com/jonnew/cyclops/blob/documentation/art/0_FinishedExample.jpg"
    /> <br> <b>Fig. 1</b> <i>A finished Cyclops board</i> </div>


*Below is Jon's original description*

PCB component population and soldering is fairly
straightforward and requires standard surface mount construction techniques.
However, there are a few bits of information that are worth reviewing before
you begin:

0. The barrel power jack (name: "POWER", value: PJ-063BH on the schematic)
should be mounted on the _bottom_ of the board. It fits on both the top and the
bottom, and will properly supply the board with power if mounted on the top.
However, if the barrel jack is mounted on the top side of the board, it will
not fit inside the enclosure.

    <div style="text-align:center"> <img src
    ="http://greentreesarborcareinc.com/wp-content/uploads/2014/01/image-placeholder.jpg"
    /> <br> <b>Fig. 1</b> <i>Power jack installation</i> </div>

0. When installing the power switch, be sure to use hookup wire capable of
handling the currents that the driver requires. AWG 20 (~0.8 mm diameter)
braided copper wire or thicker is recommended. Even if you don't want to use
the power switch, jumper the switch solder points using AWG 20 wire or thicker.

    <div style="text-align:center"> <img src
    ="http://greentreesarborcareinc.com/wp-content/uploads/2014/01/image-placeholder.jpg"
    /> <br> <b>Fig. 2</b> <i>Power switch installation</i> </div>

0. Each board has an address (0 through 3) that is defined by two solder
jumpers and the location of a ferrite chip. This allows cyclops boards to be
stacked to share a power supply while being driven by a common microcontroller.
For each board that will share a microcontroller, a unique address must be
specified and the solder jumpers and ferrite chip must be soldered in
appropriate positions to reflect this address. See the picture below to better
understand this addressing scheme.

    <div style="text-align:center"> <img src
    ="http://greentreesarborcareinc.com/wp-content/uploads/2014/01/image-placeholder.jpg"
    /> <br> <b>Fig. 3</b> <i>Set board address</i> </div>

0. Don't forget to install the heatsink.

    <div style="text-align:center"> <img src
    ="http://greentreesarborcareinc.com/wp-content/uploads/2014/01/image-placeholder.jpg"
    /> <br> <b>Fig. 4</b> <i>Installing the heatsink</i> </div>

0. The light pipes over the front LEDs need to be seated firmly for the board
to fit inside the enclosure.

    <div style="text-align:center"> <img src
    ="http://greentreesarborcareinc.com/wp-content/uploads/2014/01/image-placeholder.jpg"
    /> <br> <b>Fig. 5</b> <i>Install the light pipes</i> </div>

#### Enclosure

#### Circuit testing


Enclosure Construction Required Skills Required Tools PCB Construction Circuit
Tuning and Testing Enclosure Construction Building It Electrical Components
Enclosure The BOM includes To make the device, there are two major steps PCB
construction Circuit tuning and testing and one optional step: Enclosure
assembly.  The skills and tools required to complete device construction are
outlined below.  Required Skills Surface mount soldering Hot-air or reflow
soldering are preferred.  A tutorial on hot-air soldering can be found here A
great tutorial on general tips and tricks for surface mount soldering can be
found here Basic understanding of practical electronics, such as how to use a
multi-meter and oscilloscope Required Tools Solder stencil (optional) Stencils
are helpful for applying solder paste to the PCB. See the videos linked above
for usage information.

Soldering device At minimum, a soldering iron regulated to 700 deg. F (370 deg.
c) A hot-air rework tool or reflow oven are recommended A low cost,
high-quality hot-air rework station can be purchased from SparkFun A low cost
reflow oven can be made form a toaster oven as shown here. This link also
contains useful information on the basics of the reflow soldering process,
Electrical test equipment Mulitmeter A low cost mulitmeter is available from
sparkfun Oscilloscope Optional, but recommended for performance verification
Misc Jeweler's flat head screwdriver (e.g. these) Phillips head screwdriver (if
you are using the enclosure) A white paint pen (e.g. these) Solder Wire for
iron Paste for hot-air or reflow Copper braid ('solder wick') for solder
removal (e.g this) Usage Stereoscope or loupe PCB Construction Clean the board
using isoproyl alcohol

Apply solder paste to the pads A leaded, no-clean paste is recommended due to
ease of use.  Apply components to board Start with integrated circuits

End with passives (resistors, capacitors, inductors, and diodes).  Use the
stereoscope or loupe to ensure that pads are making contact with the pins of
the placed components.  Precise component alignment is not necessary.
Components will self-align during the reflow process.


Do not place or solder through-hole components at this time BNC connectors 0.1"
pitch headers IDC connector etc.


Reflow the solder paste on the board using your oven or hot air gun as
described in the links above.  After the solder has cooled, examine solder pads
using the stereoscope of loupe for solder bridges between pins, solder that has
not melted, or pads lacking a decent solder joint Fix any issues using a
standard soldering iron If there are solder bridges present, get rid of them
using some solder wick before moving on.  Solder through-hole components in
place using a standard soldering iron.  Solder the power switch in place using
AWG 20 wire or thicker

Circuit Tuning and Testing Before powering on the device, check for shorts
between power traces on the board.  Put your multimeter in continuity mode
Check for shorts between the Digital rail (TPXX) and ground (TPGND) Analog rail
and ground If there is a short, you must track it down and get rid of it before
applying power Enclosure Construction

### License 
#### Hardware Licensing <a rel="license"
href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative
Commons License" style="border-width:0"
src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br /><span
xmlns:dct="http://purl.org/dc/terms/" property="dct:title">Cyclops LED
Driver</span> by <a xmlns:cc="http://creativecommons.org/ns#"
href="https://github.com/jonnew/cyclops" property="cc:attributionName"
rel="cc:attributionURL">Jonathan P. Newman</a> is licensed under a <a
rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative
Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.<br
/>Based on a work at <a xmlns:dct="http://purl.org/dc/terms/"
href="https://github.com/jonnew/cyclops"
rel="dct:source">https://github.com/jonnew/cyclops</a>.

#### Software Licensing 
Copyright (c) Jonathan P. Newman All right reserved.

The code associated with the Cyclops project is free software: you can
redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The code associated with the Cyclops project is distributed in the hope that it
will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with
this code.  If not, see <http://www.gnu.org/licenses/>.

### References 
[1] T. Tchumatchenko\*, J.P. Newman\*, M.-f. Fong, S.M. Potter.
[Delivery of time-varying stimuli using
ChR2](http://journal.frontiersin.org/article/10.3389/fncir.2013.00184/full#).
(\* - equal contributions, co-first authors) Front. Neural Circuits (7:184)
2013.  doi: 10.3389/fncir.2013.00184
