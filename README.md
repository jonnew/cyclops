### Cyclops LED Driver
Precision, wide-bandwidth current source with optional optical feedback mode
for driving high power LEDs

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

#### Documentation
Documentation and usage information are maintained within this repository in
[MANUAL.md](./MANUAL.md). If you have questions concerning usage, performance,
etc., please direct them toward the [Open Ephys
forum](https://groups.google.com/forum/#!forum/open-ephys).

#### Hardware Licensing
<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img
alt="Creative Commons License" style="border-width:0"
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
