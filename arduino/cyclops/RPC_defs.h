/** @file RPC_defs.h
    @brief Defines a few constants of the RPC Format Specification.
           This file is part of CL.
  
    @section header-desc HEADER Field Description
    ~~~~~~~~~~~~~~~~~~~~
        <1: reserved> <2: channel> <5: command>
        [7]   reserved
        [6:5] Channel
        [4:0] Command
    ~~~~~~~~~~~~~~~~~~~~
    @author Ananya Bahadur
*/

#ifndef CL_RPC_DEFS_h
#define CL_RPC_DEFS_h

#define RPC_HEADER_SZ 1
#define RPC_CHANNEL   0x60

#define _CHANNEL(header_byte) header_byte & RPC_CHANNEL
#endif
