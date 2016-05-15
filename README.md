## Cyclops LED Driver
Precision, wide-bandwidth current source with optional optical feedback mode
for driving high-power LEDs.

### Documentation
Documentation and usage information are maintained within this repository in
[MANUAL.md](./MANUAL.md). If you have questions concerning usage, performance,
etc., please direct them toward the [Open Ephys
forum](https://groups.google.com/forum/#!forum/open-ephys).

### TODO
**Revision 3.6**
- [ ] Get [MANUAL.md](./MANUAL.md) in working order
- [ ] Create an fiber coupled LED interface with integrated optical power
  measurements
  - [In progress](./experimental/photodiode/)
  - [Bilateral, commutated LED](./resources/Open_source_fiber-coupled_bilateral_LED_for_in_vivo_applications.pdf)
- [ ] High side current sense
- [ ] Arduino gets replaced with a
  [Teensy](https://www.pjrc.com/teensy/teensy31.html).
- [ ] Indication light pipes get replaced with indicator modules, like
  [these](http://www.digikey.com/product-detail/en/lumex-opto-components-inc/SSF-LXH305ID-TR/67-1374-2-ND/229162)
- [ ] Schottky between MCU VIn and board power to prevent LEDs from turning on when USB is plugged in.

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
