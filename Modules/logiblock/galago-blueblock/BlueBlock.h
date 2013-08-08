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
	
	bool					resetBluetooth(void);
	
	bool					setBluetoothEnabled(bool enabled);
	
	inline bool				enableBluetooth(void)		{return(setBluetoothEnabled(true));}
	inline bool				disableBluetooth(void)		{return(setBluetoothEnabled(false));}
	
	bool					setLED(bool on);
	
	inline bool				ledOn(void)		{return(setLED(true));}
	inline bool				ledOff(void)	{return(setLED(false));}
	
private:
	
	byte					_bluetoothAppBoard;
};

extern BlueBlock blueBlock;

} } //namespace Logiblock::AppBoards

#endif //__LOGIBLOCK_BLUEBLOCK_H__
