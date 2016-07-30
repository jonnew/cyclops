#ifndef CL_CYCLOPS_H
#define CL_CYCLOPS_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif
#include "RPC_defs.h"
#include "SPI_fifo.h"
#include "Waveform.h"

/**
 * @ingroup    ns-source
 * @brief      Registers the Sources inside ``source_list`` with the library
 *
 * @param      source_list  The source list
 * @param      list_size    The list size
 */
#define REGISTER_SOURCE_LIST(source_list, list_size) \
    cyclops::Source** cyclops::source::globalSourceList_ptr = (source_list);\
    uint8_t cyclops::source::globalSourceList_size = (list_size);

namespace cyclops{
  extern bool AquisitionStatus;
}

#endif
