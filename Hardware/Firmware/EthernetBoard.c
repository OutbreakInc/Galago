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
	0x04, 0xac,	//product ID (LSB first)
	0x01, 0x04,	//product version (LSB first)
};

//GPIO direction and state that should be latched when the device is not addressed
u8 volatile gIODir;
u8 volatile gIOState;


#define PIN_APP_ETH_nCS			PB1
#define PIN_APP_ETH_nRST		PB4
#define PIN_APP_SD_nCS			PB5

#define PINMASK_APP_ETH_nCS		(1 << PIN_APP_ETH_nCS)
#define PINMASK_APP_ETH_nRST	(1 << PIN_APP_ETH_nRST)
#define PINMASK_APP_SD_nCS		(1 << PIN_APP_SD_nCS)

void	appInit(void)
{
	//initial state is logic high driven to all three endpoints
	gIOState = (PINMASK_APP_ETH_nCS | PINMASK_APP_ETH_nRST | PINMASK_APP_SD_nCS);
	gIODir = (PINMASK_APP_ETH_nCS | PINMASK_APP_ETH_nRST | PINMASK_APP_SD_nCS);
}

void	appReleaseBus(void)
{
	//when the bus is released, drive nCS lines high, removing them from the SPI bus
	gIOState |= (PINMASK_APP_ETH_nCS | PINMASK_APP_SD_nCS);
}

void	appCommand(u8 method)
{
	switch(method)
	{
	case 0x00:	//select the ENC28J60 Ethernet interface, deselected on rising SEL in appReleaseBus()
		gIOState &= ~PINMASK_APP_ETH_nCS;
		break;
		
	case 0x01:	//select the microSD card, deselected on rising SEL in appReleaseBus()
		gIOState &= ~PINMASK_APP_SD_nCS;
		break;
	
	case 0x10:	//assert !reset on the ENC28J60 Ethernet interface
		gIOState &= ~PINMASK_APP_ETH_nRST;
		break;
		
	case 0x11:	//deassert !reset on the ENC28J60 Ethernet interface
		gIOState |= PINMASK_APP_ETH_nRST;
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

