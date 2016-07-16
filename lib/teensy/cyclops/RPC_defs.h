/** @file RPC_defs.h
    @brief Defines a few constants of the RPC Format Specification.
           This file is part of CL.
    
    @page rpc-desc RPC Format Description

    @tableofcontents

    @section rpc-overview Overview
    There are two kinds of Serial packets, single byte and multi byte.
    The size of a packet is determined by MSB of header and the ``command`` field.

    @section S-header-desc Single Byte Packets
    Single byte Headers are distinguished from other headers by the MSB bit. For
    Single byte headers, this bit is always set.
    
    Field       | Bits  | Description
    ----------- | ----- | -----------
    Reserved    | [7]   | Always ``1``.
    ``channel`` | [6:3] | This is a bitmask and determines if "command" is applied on the Channel ``x``
    ``command`` | [2:0] | The command field
    
    @subsection S-cmd-desc Command descriptions
    
    command[2:0] | Name     | Effect
    ------------ | -------  | -------------
    ``000``      | start    | Launch Waveform generation.
    ``001``      | stop     | Pause Waveform generation.
    ``010``      | reset    | Reset selected sources. @attention The system is *not* reset to _initial configuration_!
    ``011``      | swap     | Swap the Cyclops instances of the 2 high ``channel`` bits.
    ``111``      | identity | Send device description.
    
    @section M-header-desc Multi Byte Packets
    Packet is formed by concatenating the header with argument bytes.

    Field       | Bits  | Description
    ----------- | ----- | -----------
    Reserved    | [7]   | Always ``0``.
    ``channel`` | [6:5] | Command is appplied on Channel ``channel[1:0]``
    ``command`` | [4:0] | The command field.

    @subsection M-cmd-desc Command descriptions

    command[4:0]  | Name               | Size(Bytes) | Effect
    ------------- | ------------------ | ----------- | --------
    ``00000``     | change_source_l    | 2           | Changes Source instance to the one reffered by @ref src-id-sec. Mode is set to ``LOOPBACK``.
    ``00001``     | change_source_o    | 2           | Changes Source instance to the one reffered by @ref src-id-sec. Mode is set to ``ONE_SHOT``.
    ``00010``     | change_source_n    | 3           | Changes Source instance to the one reffered by @ref src-id-sec. Mode is set to ``N_SHOT``. \f$N\f$ is set to @ref shot_cycle "shot_cycle".
    ``00011``     | change_time_period | 5           | Set time period of updates @attention Works only if Source::holdTime is a constant!
    ``00100``     | time_factor        | 5           | Scale Source::holdTime values by this factor. \f$\in [0, \infty)\f$.
    ``00101``     | voltage_factor     | 5           | Scale Source::getVoltage values by this factor. \f$\in [0, \infty)\f$.
    ``00110``     | voltage_offset     | 3           | Add this DC offset level to Source::getVoltage values. \f$\in [0, \infty)\f$.
    ``00111``     | square_on_time     | 5           | Set squareSource pulse "ON" time.
    ``01000``     | square_off_time    | 5           | Set squareSource pulse "OFF" time.
    ``01001``     | square_on_level    | 3           | Set squareSource pulse "ON" voltage.
    ``01010``     | square_off_level   | 3           | Set squareSource pulse "OFF" voltage.
    
    @note       Voltage scaling can also be manually accomplished by the
                tweaking the GAIN knob on Cyclops Front Panel.

    @subsubsection src-id-sec src_id
    Each Source has a unique ID which is internally used by Task. The OE plugin
    can just use the number as a reference. @sa Source::src_id
    
    @subsection M-arg-byte-desc Argument Bytes

    | Command Name       | Argument[0]   | Argument[1]      |
    | ------------------ | ------------- | ---------------- |
    | change_source_l    | uint8  src_id |                  |
    | change_source_o    | uint8  src_id |                  |
    | change_source_n    | uint8  src_id | @anchor shot_cycle uint8 shot_cycle |
    | change_time_period | uint32 val    |                  |
    | time_factor        | float  val    |                  |
    | voltage_factor     | float  val    |                  |
    | voltage_offset     | int16  val    |                  |
    | square_on_time     | uint32 val    |                  |
    | square_off_time    | uint32 val    |                  |
    | square_on_level    | uint16 val    |                  |
    | square_off_level   | uint16 val    |                  |

    @sa Task

    @author Ananya Bahadur
*/

#ifndef CL_RPC_DEFS_H
#define CL_RPC_DEFS_H

#define RPC_HEADER_SZ   1
#define RPC_MAX_ARGS    4

// Single Byte Definitions
#define SB_NUM_CMD       5
#define SB_CHANNEL_MASK  0x78 // channel[6:3]
#define SB_CMD_MASK      0x07 // command[2:0]
#define SB_CHANNEL_SHIFT 0x3
#define SB_CMD_SHIFT     0x0
#define EXTRACT_SB_CHANNELS(s_header_byte) (((uint8_t)s_header_byte & SB_CHANNEL_MASK) >> SB_CHANNEL_SHIFT)
#define EXTRACT_SB_CMD(s_header_byte)      (((uint8_t)s_header_byte & SB_CMD_MASK) >> SB_CMD_SHIFT)

// Multi Byte Definitions
#define MB_NUM_CMD       11
#define MB_CHANNEL_MASK  0x60 // channel[6:5]
#define MB_CMD_MASK      0x1f // command[4:0]
#define MB_CHANNEL_SHIFT 0x5
#define MB_CMD_SHIFT     0x0
#define EXTRACT_MB_CHANNEL(m_header_byte) ((m_header_byte & MB_CHANNEL_MASK) >> MB_CHANNEL_SHIFT)
#define EXTRACT_MB_CMD(m_header_byte)     ((m_header_byte & MB_CMD_MASK) >> MB_CMD_SHIFT)

const static uint8_t multi_sz_map[MB_NUM_CMD] = {
  2, 2, 3, 5,
  5, 5, 3, 5,
  5, 3, 3
};

/**
 * @brief      Gets the packet size or a given "command".
 *
 * @param[in]  header_byte  The header byte
 *
 * @return     The packet size.
 */
inline uint8_t getPacketSize(uint8_t header_byte){
  if (header_byte & 0x80) return 1;
  return multi_sz_map[EXTRACT_MB_CMD(header_byte)];
}
#endif
