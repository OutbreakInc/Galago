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
	0x05, 0xac,	//product ID (LSB first)
	0x02, 0x06,	//product version (LSB first)
};

//GPIO direction and state that should be latched when the device is not addressed
u8 volatile gIODir;
u8 volatile gIOState;


#define PIN_APP_GPS_nEN			PB1
#define PIN_APP_SD_nCS			PB4
#define PIN_APP_GPS_ON			PB5

#define PINMASK_APP_GPS_nEN		(1 << PIN_APP_GPS_nEN)
#define PINMASK_APP_SD_nCS		(1 << PIN_APP_SD_nCS)
#define PINMASK_APP_GPS_ON		(1 << PIN_APP_GPS_ON)

void	appInit(void)
{
	//initial state is logic high driven to all three endpoints
	gIOState = (PINMASK_APP_GPS_nEN | PINMASK_APP_SD_nCS | PINMASK_APP_GPS_ON);
	gIODir = (PINMASK_APP_GPS_nEN | PINMASK_APP_SD_nCS | PINMASK_APP_GPS_ON);
}

void	appReleaseBus(void)
{
	//when the bus is released, drive nCS lines high, removing them from the SPI bus
	gIOState |= (PINMASK_APP_SD_nCS);
}

void	appCommand(u8 method)
{
	switch(method)
	{
	case 0x00:	//enable the GPS module
		gIOState |= PINMASK_APP_GPS_ON;
		break;
		
	case 0x01:	//disable the GPS module
		gIOState &= ~PINMASK_APP_GPS_ON;
		break;
		
	case 0x02:	//read test string
		SPISetupGXB();
		
		u8 const* msg = "hello";
		u8 i = 5;
		while(i-- && SPIExchangeGXB(*msg++));
		
		SPIDisableGXB();
		
		break;
		
	case 0x10:	//connect TXD/RXD to the GPS module
		gIOState &= ~PINMASK_APP_GPS_nEN;
		break;
		
	case 0x11:	//disconnect TXD/RXD from the GPS module
		gIOState |= PINMASK_APP_GPS_nEN;
		break;
		
	case 0x20:	//select the microSD card, deselected on rising SEL in appReleaseBus()
		gIOState &= ~PINMASK_APP_SD_nCS;
		break;
	}
}

