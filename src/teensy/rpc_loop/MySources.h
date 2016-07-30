#ifndef CL_MY_SOURCES_H
#define CL_MY_SOURCES_H
//
// This file contains the globals definitions of the Source objects Just include
// this file into your main `.ino` script, and you'll get access to the objects
// here.
//

uint16_t triangle_wave(uint8_t seq){
  return (abs(20 - seq)/40.0)*2048;
}

uint32_t triangle_update_period(uint8_t seq){
  return 30000 + seq;
}

uint16_t vd_1[] = {1, 2048};
uint32_t ht_1[] = {70000, 70000};

cyclops::generatedSource gen ( triangle_wave
                             , triangle_update_period
                             , 40
                             , cyclops::source::LOOPBACK);

cyclops::storedSource sto_1 ( vd_1
                            , ht_1
                            , 2
                            , cyclops::source::LOOPBACK);

cyclops::squareSource square (cyclops::source::LOOPBACK);

/* You must register these sources with the library, by:
 *
 * 1. making a globally scoped array of pointers to the objects.
 * 2. assign the global array to the ``globalSourceList_ptr``
 *
 * Only the registered sources are guaranteed to work when using the RPC,
 * especially when using the OE GUI.
 */
cyclops::Source* globalSourceList[] = { &gen
                                      , &sto_1
                                      , &square
                                      };
// Assigning this array to the special pointer.
REGISTER_SOURCE_LIST(globalSourceList, 3);

#endif
