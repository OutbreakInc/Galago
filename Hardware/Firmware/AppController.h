///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APP_CONTROLLER_H__
#define __APP_CONTROLLER_H__

#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef signed char		s8;
typedef signed short	s16;

//standard SPI slave routines
void	SPISetupSlave(void);
void	SPIDisableSlave(void);
u8		SPIExchangeSlave(u8 byte, u8 output);

//no setup required for master mode
u8		SPIExchangeMaster(u8 byte);

//special two-wire synchronous slave protocol with a dominant bit (permits collision-avoidance) for GXB
void	SPISetupGXB(void);
void	SPIDisableGXB(void);
u8		SPIExchangeGXB(u8 byte);


///////////////////////////////////////////////////////////////////////////////////////////

//All of the following must be declared by the application implementation

extern u8 volatile gIODir;
extern u8 volatile gIOState;

extern u8 const kUUIDAndDescriptor[] PROGMEM;

//called when the app board is initialized
void	appInit(void);

//called when shared IO lines are to be released
//  In practical terms, if an App Controller controls chip-select lines of devices on the SPI/GXB bus
//  then they must enter a high-impedance state when this is called.
void	appReleaseBus(void);

//called when the App Controller is addressed, for any reason apart from discovery, de-enumeration
//  or any other standard capability.  The method parameter is the first byte sent following the address
//  in the GXB protocol.  If the method requires further bytes to be sent or received, they can be
//  exchanged in this function.  Do not set IO pin state directly in this function; instead, set the
//  gIODir and gIOState globals.  Remember that this function can be preempted by a falling SEL signal
//  at any time.  Do not consider anything done in this function to take effect unless it completes
//  before the falling edge of the SEL line.
void	appCommand(u8 method);

#endif //!defined(__APP_CONTROLLER_H__)