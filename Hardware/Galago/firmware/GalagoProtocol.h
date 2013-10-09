/*
/	Copyright (C) Outbreak, Inc. 2010-2013.  All rights reserved.
/	Created: 2010Nov09
/	Author: Kuy Mainwaring
/	http://logiblock.com/products/galago
/
//////////////////////////////////////////////////////////////*/


// Galago version 0BAB0410 (the version that shipped for the Kickstarter campaign) carries an ATtiny45 microcontroller
//   that acts as a bridge between a host computer and the on-board ARM chip.  This protocol translator is a low-speed
//   USB device with USB ID 00B1:AB04 that implements a "vendor" USB interface, described below.
//
// This debugger is a special low-complexity and super-low-cost implementation of a USB-connected ARM SWD bridge.  In
//   essence, the host computer tracks the state of the ARM core and sends SWD messages to it.  The bridge simply
//   bridges these packets between the ARM and the host, pushing all debugging complexity to the host computer where
//   it can be easily developed, ported and updated.
//
// Thanks to its simplicity, the range of methods implmented is concise, and enumerated below in GalagoProtocol10.
//   Messages follow a simple format consisting of four bytes transmitted in a USB setup control packet to endpoint 0,
//   with an eight-byte response.  The four argument bytes are sent in the USB wValue and wIndex members of a standard
//   setup packet to the default endpoint.
//
// An explanation of ARM's SWD two-wire debugging protocol is beyond the scope of this document but elementally it
//   allows the interrogator (debugger bridge) to read and write eight basic registers.  Four registers, which I'll
//   call D0-D3, control the debugger interface itself.  Four others, A0-A3, are mapped onto a debug unit (which debug
//   unit is specified by D2) and offer a window into the feature they debug.  The most basic and important debug unit
//   is the memory unit, and its primary interface exposes A0-A3 as a control, address, (reserved) and data register
//   respectively.  To write a value to memory, use the D* registers to set up the interface and the A* register
//   interface, then set up the write in A0, put the memory address in A1 and the data in A3.  Reading is the same
//   except A3 is read instead of written.  Memory reads are pipelined so the first read of an array returns a
//   nonsense value, subsequent reads return the previous datum and the last memory read can be collected from D3.
//   Writing each D*/A* register triggers state changes or memory bus transfers; certain memory addresses trigger
//   core halts, steps and other debug subsystem actions (e.g. breakpoints).  General-purpose ARM registers are
//   memory-mapped through a special memory register aperture, and can be read and written by loading, reading and
//   writing an address and data register pair.  Many other operations are possible using these basic methods.
//
// The ARM debug interface is quirky in some areas and not perfectly documented, please study the ARM Architecture
//   Reference Manual for ARMv7-M (ARM DDI 0403C) and the ARM Debug Interface v5 (ARM IHI 0031A) for full details.
//
// Here's an example of a USB setup packet to exchange a SWD packet of "read D0"
//
//  setup =	//read D0
//  {
//    bmRequestType = (1 << 7) | (2 << 5) | (0 << 0),  // IN + VENDOR + DEVICE
//    bRequest = Galago_Protocol10_SWD_DoTransaction | SWD_OPCODE_READ | SWD_OPCODE_DP | SWD_OPCODE_R0,
//	  wValue = 0,
//	  wIndex = 0,
//    wLength = 0
//  }
//
// Responses are parsed from the eight-byte returned data array.  data[0] is a GalagoProtocolBase_Response code, and
//   data[1..7] depend on the method called - see the GalagoProtocol10 table.
//
// Because the SWD protocol requires the LSB to be sent first on the wire, you must reverse the bits you receive on the
//   host: 0xEE28:0x06D4 => 0x2BA01477.  This number identifies the licensee, revision and ARM Ltd.; in this example, a
//   Cortex M3.
//
// For writes, the wValue:wIndex carries the 32-bit write payload.  As with reads, you must reverse the bits on the
//   host before sending the packet.  E.g.: 0x12345678 => 0x1E6A:0x2C48
//
//  setup =	//write 0x12345678 to A3
//  {
//    bmRequestType = (1 << 7) | (2 << 5) | (0 << 0),  // IN + VENDOR + DEVICE (note SWD writes are done as USB reads)
//    bRequest = Galago_Protocol10_SWD_DoTransaction | SWD_OPCODE_WRITE | SWD_OPCODE_AP | SWD_OPCODE_R3,
//	  wValue = 0x1E6A,
//	  wIndex = 0x2C48,
//    wLength = 0
//  }
//
// Please note that many real-world SWD ports are also JTAG ports - SWD is designed to be overlaid on JTAG's TCK and
//   TMS signals - and a special bit sequence is needed to switch from JTAG to SWD (and another one to switch back.)
//   This sequence is 0111100111100111 (0x79E7 MSB first) to switch to SWD, or 0011110011100111 (0x3CE7 MSB first) to
//   switch to JTAG.  It's the same for all Cortex chips and is widely documented in footnotes, but it's a real nuisance
//   if you're not aware of it.  The Galago debugger automatically performs this switch as part of the
//   Galago_Protocol10_SWD_ResetBus method.

////////////////////////////////////////////////////////////////
// Low-level SWD protocol defines - used to compose SWD opcodes at wire-level

#define SWD_OPCODE_READ			(0x08)
#define SWD_OPCODE_WRITE		(0x00)
#define SWD_OPCODE_AP			(0x10)
#define SWD_OPCODE_DP			(0x00)
#define SWD_OPCODE_R0			(0x00)
#define SWD_OPCODE_R1			(0x04)
#define SWD_OPCODE_R2			(0x02)
#define SWD_OPCODE_R3			(0x06)
#define SWD_OPCODE_PARITY0		(0x00)
#define SWD_OPCODE_PARITY1		(0x01)

#define SWD_RESPONSE_OK			(0x1)
#define SWD_RESPONSE_BUSY		(0x2)
#define SWD_RESPONSE_ERR		(0x4)
#define SWD_RESPONSE_DEAD		(0x7)	//any response that's not OK, BUSY or ERR is to be considered DEAD

//
////////////////////////////////////////////////////////////////
// Common Protocol

enum GalagoProtocolBase
{
	//return structure depends on protocol version, but byte[0] of the response is always the protocol version
	Galago_Protocol_CommCheck					= 0x00,
};

enum GalagoProtocolBase_Response
{
	Galago_Response_UnknownError				= 0x00,
	Galago_Response_OK							= 0x01,
	Galago_Response_IncorrectMethod				= 0x02,
	Galago_Response_CannotProcessCommandNow		= 0x03,
	Galago_Response_BadArguments				= 0x04,
};

//
////////////////////////////////////////////////////////////////
// Protocol 1.0
//  Galago version 0BAB0410 (the version that shipped for the Kickstarter campaign) implements this protocol.

enum GalagoProtocol10
{
	//Galago_Protocol_CommCheck returns an empty CommCheck struct: no bytes follow the
	//  protocol version byte (1)
	
	////////////////////////////////////////////////////////////////
	// 4-bit Parallel I/O  methods
	
	//pass pin mask in low byte of wIndex (0 = input, 1 = output), returns ack
	Galago_Protocol10_ParallelIO_Enable_SetIOMask	= 0x01,
	
	//pass pin settings in low byte of wIndex, returns ack
	Galago_Protocol10_ParallelIO_Output				= 0x02,
	
	//returns ack + 1 byte of pin values
	Galago_Protocol10_ParallelIO_Input				= 0x03,
	
	
	//I2C methods (reserved for protocol 1.5)			= 0x04 - 0x07
	//SPI methods (reserved for protocol 1.5)			= 0x08 - 0x09,
	
	//set the CS state in wValue:
	//	0 asserts the !CS line (pull to GND)
	//	non-zero drives the line to the Galago Vdd voltage (3.3V)
	Galago_Protocol10_SPI_Enable_ChipSelect			= 0x0A,
	
	//Perform a bus reset on the SWD interface
	//  no arguments
	Galago_Protocol10_SWD_ResetBus					= 0x0B,
	
	//Note: Implemented only on beta hardware/firmware, not on shipping 0BAB0410 products
	Galago_Protocol10_SWD_GetVdd					= 0x0C,
	//unused codes:									= 0x0C - 0x1F
	
	////////////////////////////////////////////////////////////////
	// ARM SWD (Serial Wire Debug) methods
	
	//Output raw bits to the SWD interface.
	//  For some MCUs, this is needed to provide SWD-JTAG compatibility.  It can also be used to bit-bang SWD.
	//  Resetting (synchronizing) the bus is done by using this method to output two words of 0xFFFFFFFF.
	//  Up to 32 bits can be sent in wIndex:wValue; the number of bits to transmit is the lower 5 bits of this methodID
	Galago_Protocol10_SWD_OutputRawBits				= 0x20, // - 0x3F
	
	//Input raw bits from the SWD interface.  This can be used to bit-bang SWD.
	//  Up to 32 bits can be read; the number of bits to read is the lower 5 bits of this methodID.
	//  The response data is as follows:
	//    data[0] = Galago_Response_* code
	//    data[1-4] = 32-bit word response, in MSB-first (conventional Big-Endian) order
	Galago_Protocol10_SWD_InputRawBits				= 0x40, // - 0x5F
	
	//Perform a formal SWD read or write transaction, as specified by the pre-formatted opcode
	//  the pre-formatted opcode is built from the SWD_OPCODE 
	//  the opcode consists of the five variable bits [ARyxP], where:
	//    A is APnDP, R is RnW, y is the address high bit, x is the address low bit, P is even parity
	//  for reads, the response is as follows:
	//    data[0] = Galago_Response_* code
	//    data[1] = 3-bit SWD response, one of SWD_RESPONSE_*
	//    data[2-5] = 32-bit word, MSB-first
	//  for writes, the 32-bit word write payload goes in wIndex:wValue, and the response is as follows:
	//    data[0] = Galago_Response_* code
	//    data[1] = 3-bit SWD response, one of SWD_RESPONSE_*
	Galago_Protocol10_SWD_DoTransaction				= 0x60, // - 0x7F
};

enum GalagoProtocol10_Response
{
	Galago_Response10_SWD_BusDead	= 0x05,
	Galago_Response10_SWD_BusError	= 0x06,
};
