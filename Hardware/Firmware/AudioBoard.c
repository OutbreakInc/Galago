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
	0x06, 0xac,	//product ID (LSB first)
	0x02, 0x06,	//product version (LSB first)
};

//GPIO direction and state that should be latched when the device is not addressed
u8 volatile gIODir;
u8 volatile gIOState;


#define PIN_APP_SD_nCS			PB1
#define PIN_APP_DAC_nEN			PB4

#define PINMASK_APP_SD_nCS		(1 << PIN_APP_SD_nCS)
#define PINMASK_APP_DAC_nEN		(1 << PIN_APP_DAC_nEN)

void	appInit(void)
{
	//initial state is logic high driven to both endpoints (so both are disabled)
	gIOState = (PINMASK_APP_SD_nCS | PINMASK_APP_DAC_nEN);
	gIODir = (PINMASK_APP_SD_nCS | PINMASK_APP_DAC_nEN);
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
	case 0x00:	//select the SD card, deselected on rising SEL in appReleaseBus()
		gIOState &= ~PINMASK_APP_SD_nCS;
		break;
		
	case 0x10:	//connect the P1,P2,P3,P5 GPIO lines to the DAC for a software SPI interface
				// that doesn't interfere with the main SPI port (so it can read audio data
				//simultaneously)
		gIOState &= ~PINMASK_APP_DAC_nEN;
		break;
	
	case 0x11:	//release DAC interface
		gIOState |= PINMASK_APP_DAC_nEN;
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

