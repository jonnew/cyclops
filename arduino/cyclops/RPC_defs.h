/** @file RPC_defs.h
    @brief Defines a few constants of the RPC Format Specification.
           This file is part of CL.
  
    @section header-desc HEADER Field Description
    ~~~~~~~~~~~~~~~~~~~~
        <2: channel> <6: command>
        [7:6] Channel
        [5:0] Command
    ~~~~~~~~~~~~~~~~~~~~
    @author Ananya Bahadur
*/

#ifndef CL_RPC_DEFS_h
#define CL_RPC_DEFS_h

#define RPC_HEADER_SZ 1
#define RPC_CHANNEL_RSHIFT 6
#define RPC_CHANNEL_MASK   0x02
#define RPC_COMMAND_RSHIFT 0
#define RPC_COMMAND_MASK   0x3f

#define EXTRACT_CHANNEL(header_byte) ((header_byte >> RPC_CHANNEL_RSHIFT) & RPC_CHANNEL_MASK)
#define EXTRACT_COMMAND(header_byte) ((header_byte >> RPC_COMMAND_RSHIFT) & RPC_COMMAND_MASK)
#endif