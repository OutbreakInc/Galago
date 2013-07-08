#define _ _ /* No comments or C code before this line!

# This is an integrated build script - mark the file as executable and run it to build:
#  $ chmod +x thisFile.c
#  $ ./thisFile.c

MCU="attiny45" MCU_DEF="__AVR_ATtiny45__" F_CPU="16000000" ./avrbuild.sh $0
exit 0
*/

///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////

#include "AppController.h"


//The following descriptor block defines this product:

u8 const kUUIDAndDescriptor[] PROGMEM =
{
	//this 128-bit UUID gets filled in post-build and is UNIQUE FOR EACH CHIP, like a serial number
	//  THIS IS IMPORTANT - if two devices on the bus have the same UUID, bad things will happen!
	//  Also, do not hard-code a UUID for testing here - the 0xFF...0xFF value is an important safeguard.
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	
	//descriptor
	7,			//size (excludes this byte)
	1,			//version
	0xb1, 0x00,	//vendor ID (LSB first)
	0x03, 0xac,	//product ID (LSB first)
	0x01, 0x06,	//product version (LSB first)
};

//GPIO direction and state that should be latched when the device is not addressed
u8 volatile gIODir;
u8 volatile gIOState;


#define PIN_APP_TLC_nTIMING		PB1
#define PIN_APP_TLC_nCS			PB4

#define PINMASK_APP_TLC_nTIMING	(1 << PIN_APP_TLC_nTIMING)
#define PINMASK_APP_TLC_nCS		(1 << PIN_APP_TLC_nCS)

void	appInit(void)
{
	//initial state is logic high driven to all three endpoints
	gIOState = (PINMASK_APP_TLC_nTIMING | PIN_APP_TLC_nCS);
	gIODir = (PINMASK_APP_TLC_nTIMING | PIN_APP_TLC_nCS);
}

void	appReleaseBus(void)
{
	//when the bus is released, drive nCS lines high, removing them from the SPI bus
	gIOState |= (PIN_APP_TLC_nCS);
}

void	appCommand(u8 method)
{
	switch(method)
	{
	case 0x00:	//select the TLC5940 interface, deselected on rising SEL in appReleaseBus()
		gIOState &= ~PIN_APP_TLC_nCS;
		break;
		
	case 0x10:	//connect the timing pins GSCLK, BLANK and XLAT to P1, P3 and P5 respectively
		gIOState &= ~PIN_APP_TLC_nTIMING;
		break;
		
	case 0x11:	//disconnect the timing pins GSCLK, BLANK and XLAT
		gIOState |= PIN_APP_TLC_nTIMING;
		break;
		
	case 0x85:	//read test string
		SPISetupGXB();
		
		u8 const* msg = "hello";
		u8 i = 5;
		while(i-- && SPIExchangeGXB(*msg++));
		
		SPIDisableGXB();
		
		break;
	}
}

