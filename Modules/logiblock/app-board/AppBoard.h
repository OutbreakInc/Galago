#ifndef _LOGIBLOCK_APP_BOARDS_H_
#define _LOGIBLOCK_APP_BOARDS_H_

////////////////////////////////////////////////////////////////
// GXB - Galago expansion bus, a multiplexed SPI bus with autodiscovery and device addressing (experimental)

namespace Logiblock {

struct Endpoint;

class AppBoard
{
public:
	
	struct DeviceDetails
	{
		unsigned short vendorID;
		unsigned short productID;
		unsigned short version;
	};
	
			AppBoard(void);
	
	//Reset the bus. Doing this de-enumerates all connected devices.
	void	reset(void);
	
	//Detect devices that haven't been enumerated yet.  Returns true if one or more devices were found.
	bool	detect(void);
	
	//Return (in 'detailsOut', provided by the caller) details of the specified device
	bool	details(byte address, DeviceDetails* detailsOut);
	
	//Return the address of the next highest device after the one specified, filtered by
	//  vendorID (if provided) and productID (if provided)
	byte	find(byte afterAddress = 0, unsigned short vendorID = 0, unsigned short productID = 0);
	
	//send a message to a device. This can be used to select a slave SPI device, activate board features
	//  and a number of other functions. See the protocol reference for a board to learn what to send it.
	bool	write(byte address, byte const* command = 0, int length = 0);
	
	//de-enumerate one device by address.  No effect if the device is not present
	void	detach(byte address);
	
private:
	
	int			setupGXB(void);
	byte		nextAddress(Endpoint**& predecessor);
	void		insert(Endpoint** predecessor, Endpoint* e);
	void		remove(byte address);
	bool		internalWrite(byte address, byte const* command = 0, int length = 0);
	void		udelay(int timeStep, int i);
	void		readBytes(int timeStep, byte* out, int length);
	bool		writeByte(unsigned int timeStep, byte b);
	
	Endpoint*	endpoints;
};

extern AppBoard appBoard;	//static single instance

} //ns

#endif //!defined(_LOGIBLOCK_APP_BOARDS_H_)
