/*
Copyright (c) 2014 Jon Newman (jpnewman ~at~ mit <dot> edu)
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

You should have received a copy of the GNU General Public License
along with CL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Board.h"

namespace cyclops{

using namespace board;

// Populate the LUTs
static const uint8_t a_in_lut_data[] = {(uint8_t)ADC0, (uint8_t)ADC1, (uint8_t)ADC2, (uint8_t)ADC3};
const uint8_t *Board::_a_in_lut = a_in_lut_data;
static const uint8_t cs_lut_data[] = {(uint8_t)CS0, (uint8_t)CS1, (uint8_t)CS2, (uint8_t)CS3};
const uint8_t *Board::_cs_lut = cs_lut_data;
static const uint8_t oc_lut_data[] = {(uint8_t)OC0, (uint8_t)OC1, (uint8_t)OC2, (uint8_t)OC3};
const uint8_t *Board::_oc_lut = oc_lut_data;
static const uint8_t trig_lut_data[] = {(uint8_t)TRIG0, (uint8_t)TRIG1, (uint8_t)TRIG2, (uint8_t)TRIG3};
const uint8_t *Board::_trig_lut = trig_lut_data;
static const uint8_t ldac_lut_data[] = {(uint8_t)LDAC0, (uint8_t)LDAC1, (uint8_t)LDAC2, (uint8_t)LDAC3};
const uint8_t *Board::_ldac_lut = ldac_lut_data;
static const uint8_t board_id_data[] = {(uint8_t)BOARD_ADDR0, (uint8_t)BOARD_ADDR1, (uint8_t)BOARD_ADDR2, (uint8_t)BOARD_ADDR3};
const uint8_t *Board::_boardID_lut = board_id_data;

Board::Board(Channel _channel): channel(_channel),                                    
                                    trig_config(NO_TRIG),
                                    software_hook_fn(nullptr)
{
    // hopefully, the BOARD_ADDR<channle> pin is high...
    _oc_trig = 0;
    // Set pin modes
    pinMode(_oc_lut[channel], OUTPUT);
    pinMode(_trig_lut[channel], INPUT);
    // Pull the trigger line down weakly
    digitalWrite(_trig_lut[channel], LOW);
    // Initialize the OC pin
    digitalWrite(_oc_lut[channel], LOW);

    // Configure load-DAC line
    pinMode(_ldac_lut[channel], OUTPUT);
    digitalWrite(_ldac_lut[channel], HIGH);
    SPI_fifo.setup_pcs_pin(channel, _cs_lut[channel]);
    
}

float Board::overCurrentProtect(float current_limit_mA)
{
    // Measure the current
    float current_mA = measureCurrent();

    // If we are over-current, then turn off input
    if (current_mA > current_limit_mA) {
        _oc_trig = 1;
        digitalWrite(_oc_lut[channel], HIGH);  // Switch off the input
    } else if (_oc_trig) {
        _oc_trig = 0;
        digitalWrite(_oc_lut[channel], LOW);  // Allow input
    }
    return current_mA;
}

float Board::measureCurrent(void)
{
    int sensor_value = analogRead(_a_in_lut[channel]);
    return 1000.0 * sensor_value * (5.0 / 1023.0);
}

void Board::dac_load(void)
{
    digitalWrite(_ldac_lut[channel], LOW);
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    digitalWrite(_ldac_lut[channel], HIGH);
}

void Board::dac_shutdown(void)
{
    // Create data packet
    unsigned int spi_out = DAC_CONF_SHDN;    
    SPI_fifo.write16(channel, spi_out);    
}

void Board::dac_prog_voltage(uint16_t voltage)
{
    // Create data packet
    uint16_t spi_out = DAC_CONF_ACTIVE | (voltage & 0x0fff);
    SPI_fifo.write16(channel, spi_out);
}

void Board::uponExternalTrigger(void (*user_func)(void), int mode) {
    trig_config = EXT_TRIG;
    attachInterrupt(_trig_lut[channel], user_func, mode);
}

void Board::setSoftwareHook(void (*user_func)(void), triggerConfig tr_cfg){
    if (tr_cfg == NO_TRIG || tr_cfg == EXT_TRIG)
        return;
    detachInterrupt(_trig_lut[channel]); // test this!
    trig_config = tr_cfg;
    software_hook_fn = user_func;
}


const uint8_t* Board::getBoardAddressPins(){
    return _boardID_lut;
}

bool Board::isConnectedAtChannel(uint8_t channel_id){
    return digitalRead(_boardID_lut[channel_id]) == LOW;
}

} // NAMESPACE cyclops
