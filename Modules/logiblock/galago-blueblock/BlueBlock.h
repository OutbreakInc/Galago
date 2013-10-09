#ifndef __LOGIBLOCK_BLUEBLOCK_H__
#define __LOGIBLOCK_BLUEBLOCK_H__

#include <GalagoAPI.h>
using namespace Galago;

namespace Logiblock { namespace AppBoards {

class BlueBlock
{
public:
							BlueBlock(void);
	
	//initialize the app board.  Returns true if a board was found.
	bool					init(void);
	
	//returns the App Board's GXB address. 0 means not found, 0xFE and 0xFF are invalid.
	inline byte				boardAddress(void) const	{return(_bluetoothAppBoard);}

	//reset the Bluetooth module
	bool					resetBluetooth(void);
	
	//connect or disconnect the Bluetooth module from the UART.  Use to share the UART (serial port) with other devices
	bool					setBluetoothEnabled(bool enabled);
	
	//shortcut methods for .setBluetoothEnabled()
	inline bool				enableBluetooth(void)		{return(setBluetoothEnabled(true));}
	inline bool				disableBluetooth(void)		{return(setBluetoothEnabled(false));}
	
	//set the LED state
	bool					setLED(bool on);
	
	//shortcut methods for .setLED()
	inline bool				ledOn(void)		{return(setLED(true));}
	inline bool				ledOff(void)	{return(setLED(false));}
	
private:
	
	byte					_bluetoothAppBoard;
};

extern BlueBlock blueBlock;

} } //namespace Logiblock::AppBoards

#endif //__LOGIBLOCK_BLUEBLOCK_H__
