/*
Copyright (c) Jon Newman (jpnewman ~at~ mit <dot> edu)
All right reserved.

This file is part of the Cyclops Library (CL) for Arduino.

CL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
jjjjju
You should have received a copy of the GNU General Public License
along with CL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Cyclops.h>

cyclops::Board board(cyclops::board::CH0);

void setup()
{
  // Start the SPI bus, which is common to all attached boards
  cyclops::start_boards();
}

void loop()
{
  board.dac_prog_voltage(4095);
  board.dac_load();
  delayMicroseconds(1000);
  board.dac_prog_voltage(0);
  board.dac_load();
  delay(4999);
}

