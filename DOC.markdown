Cyclops Documentation (UNDER CONSTRUCTION)
==============
This documents provides construction, usage, and performance documentation for the Cyclops LED driver. This document   evolves with the repository. To view old revisions, checkout tags or old commits using their SHA.


### TODO
- [ ] The rear panel holes seems to have slight vertical offsets
  - LED jacks: 1 mm to low 
  - Exapnsion header: 1 mm to low 
  - Aux/curr swithc: 0.25 mm to low
- [ ] The LED polarity labels on the panel is reversed 
# Specification

# Usage

# Construction 
Before you build a board, its recommended that you get in touch [jonnew](https://github.com/jonnew)for a couple reasons:
- We are interested in getting feedback on which portions of this documentation need improvement.
- The cyclops is a fairly complex PCB. If you have doubts, let us know so we can help you with the build.

## Components
The bill of materials (BOM) required to build a 1,2,3, or 4-channel device is available on a [https://docs.google.com/spreadsheets/d/1YQR_ujrZgILNx3XjomLKWgzDvirwKrKaRbVVzmBgk-s/edit?usp=sharing Google spreadsheet]. Most of the parts can be purchased from Digikey, but there are a few components that need to be bought from other sources such as Newark, Adafruit, and Samtec. All vendor, part number, and quantity information is listed on the BOM. As a convenience, the spreadsheet contains links to pre-populated Digikey carts (which are optimized for quantity discounts) for up to 4 circuits.

The PCB can be purchased from OSH park [TODO here]. This is a four layer PCB. Note that OSH park sells a minimum of 3 boards per order. PCB stencils, useful for those performing reflow or hot-air soldering, are available from OSH stencils at the following link: TODO. If you plan to hand solder the board, then you do not need to purchase these stencils.

The BOM includes several optional components, which are not in the pre-populated Digikey carts. These include
- An extruded aluminum enclosure, which houses the completed board. The enclosure is recommended because the large voltages and current transients used to drive high power LEDs can cause capacitive and inductive interference with nearby recording equipment. Acrylic front and rear panels can be purchased from Ponoko using the links supplied in the BOM. The instructions below show how these plastic pieces are modified to provide proper electrical shielding.
- TODO: An M8-4 connector.
    
## PCB

## Enclosure

## Circuit testing


Enclosure
Construction
Required Skills
Required Tools
PCB Construction 
Circuit Tuning and Testing 
Enclosure Construction
Building It
Electrical Components
Enclosure
The BOM includes To make the device, there are two major steps
PCB construction
Circuit tuning and testing 
and one optional step:
Enclosure assembly. 
The skills and tools required to complete device construction are outlined below.
Required Skills
Surface mount soldering
Hot-air or reflow soldering are preferred.
A tutorial on hot-air soldering can be found here
A great tutorial on general tips and tricks for surface mount soldering can be found here
Basic understanding of practical electronics, such as how to use a multi-meter and oscilloscope
Required Tools
Solder stencil (optional)
Stencils are helpful for applying solder paste to the PCB. See the videos linked above for usage information.

Soldering device
At minimum, a soldering iron regulated to 700 deg. F (370 deg. c)
A hot-air rework tool or reflow oven are recommended
A low cost, high-quality hot-air rework station can be purchased from SparkFun
A low cost reflow oven can be made form a toaster oven as shown here. This link also contains useful information on the basics of the reflow soldering process,
Electrical test equipment
Mulitmeter
A low cost mulitmeter is available from sparkfun
Oscilloscope
Optional, but recommended for performance verification
Misc
Jeweler's flat head screwdriver (e.g. these)
Phillips head screwdriver (if you are using the enclosure)
A white paint pen (e.g. these)
Solder
Wire for iron 
Paste for hot-air or reflow
Copper braid ('solder wick') for solder removal (e.g this)
Usage 
Stereoscope or loupe
PCB Construction 
Clean the board using isoproyl alcohol

Apply solder paste to the pads
A leaded, no-clean paste is recommended due to ease of use. 
Apply components to board
Start with integrated circuits

End with passives (resistors, capacitors, inductors, and diodes). 
Use the stereoscope or loupe to ensure that pads are making contact with the pins of the placed components.
Precise component alignment is not necessary. Components will self-align during the reflow process.


Do not place or solder through-hole components at this time
BNC connectors
0.1" pitch headers
IDC connector
etc.


Reflow the solder paste on the board using your oven or hot air gun as described in the links above.
After the solder has cooled, examine solder pads using the stereoscope of loupe for solder bridges between pins, solder that has not melted, or pads lacking a decent solder joint
Fix any issues using a standard soldering iron
If there are solder bridges present, get rid of them using some solder wick before moving on.
Solder through-hole components in place using a standard soldering iron.
Solder the power switch in place using AWG 20 wire or thicker

Circuit Tuning and Testing 
Before powering on the device, check for shorts between power traces on the board.
Put your multimeter in continuity mode
Check for shorts between the
Digital rail (TPXX) and ground (TPGND)
Analog rail  and ground
If there is a short, you must track it down and get rid of it before applying power
Enclosure Construction
