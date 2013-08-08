#include <GalagoAPI.h>
#include <AppBoard.h>
#include <BlueBlock.h>

using namespace Galago;
using namespace Logiblock::AppBoards;


//global singleton
namespace Logiblock { namespace AppBoards { BlueBlock blueBlock; } }

			BlueBlock::BlueBlock(void):
				_bluetoothAppBoard(0)
{
}

bool		BlueBlock::init(void)
{
	return((_bluetoothAppBoard = appBoard.find(0, 0x0b1, 0xac07)) != 0);
}

bool		BlueBlock::resetBluetooth(void)
{
	if(_bluetoothAppBoard == 0)
		return(false);
		
	static byte const resetSequence[] = {0x10, 0x11};
	bool ok = appBoard.write(_bluetoothAppBoard, resetSequence, 1);
	ok = ok && appBoard.write(_bluetoothAppBoard, resetSequence + 1, 1);
	
	return(ok);
}

bool		BlueBlock::setBluetoothEnabled(bool enabled)
{
	if(_bluetoothAppBoard == 0)
		return(false);
		
	byte txrxEnabled = enabled? 0x12 : 0x13;
	return(appBoard.write(_bluetoothAppBoard, &txrxEnabled, 1));
}

bool		BlueBlock::setLED(bool on)
{
	if(_bluetoothAppBoard == 0)
		return(false);
		
	byte ledOn = on? 0x14 : 0x15;
	return(appBoard.write(_bluetoothAppBoard, &ledOn, 1));
}

//TBD:
//  due to the module's firmware, these are only accessible when not paired, which
//  is stupid because we have no transport-level way to know if it's connected.
// -> needs a better solution

//bool		BlueBlock::setBluetoothName(char const* name, )
//bool		BlueBlock::setBluetoothPairingCode(char const* name, )
//bool		BlueBlock::setBluetoothBaudRate(char const* name, )
