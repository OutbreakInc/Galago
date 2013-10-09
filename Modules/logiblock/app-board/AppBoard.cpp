#include <GalagoAPI.h>
using namespace Galago;

#include <LPC13xx.h>
#include <AppBoard.h>

////////////////////////////////////////////////////////////////
// GXB - Galago expansion bus, a multiplexed SPI bus with autodiscovery and device addressing (experimental)

namespace Logiblock {

	struct Endpoint
	{
		Endpoint*		next;
		
		byte const*		descriptor;
		
		byte			descriptorLength;	//does not count 16-byte UUID, 1-byte length and 1-byte version
		byte			address;
		byte			protocolVersion;
		
		byte const*		uuid() const		{return(descriptor);}
		unsigned short	vendorID() const	{return(((unsigned short*)(descriptor + 18))[0]);}
		unsigned short	productID() const	{return(((unsigned short*)(descriptor + 18))[1]);}
		unsigned short	version() const		{return(((unsigned short*)(descriptor + 18))[2]);}
		
						Endpoint(byte protoVersion, byte addr, byte const* desc, byte descLen):
							next(0),
							address(addr),
							protocolVersion(protoVersion),
							descriptor(desc),
							descriptorLength(descLen)
		{
		}
		
						~Endpoint(void)
						{
							if(descriptor != 0)
								delete[] descriptor;
						}
	};

}

using namespace Logiblock;


		AppBoard::AppBoard(void):
			endpoints(0)
{
}
void	AppBoard::reset(void)
{
	int timeStep = setupGXB();
	udelay(timeStep, 30);

	//empty endpoints list
	Endpoint* e = endpoints;
	endpoints = 0;
	while(e)
	{
		Endpoint* d = e;
		e = e->next;
		delete d;
	}
	
	unsigned int kResetKeyLE = 0x012707C1; 	//= {0xC1, 0x07, 0x27, 0x01} - special 'all' address (0xFE) and reset key: 0xC1072701 (big-E)
	
	internalWrite(0xFE, (byte const*)&kResetKeyLE, 4);
}
bool	AppBoard::detect(void)
{
	int oldSCKMode = *LPC1300::IOConfigPIO0_6;
	int timeStep = setupGXB();
	
	bool empty, invalid, found = false;
	do
	{
		empty = invalid = true;
		
		io.sel = true;
		udelay(timeStep, 10);
		
		byte autoDetect = 0;
		if(writeBytes(timeStep, &autoDetect, 1))	//address the autodetection pseudo-address
		{
			//wait
			udelay(timeStep, 10);
			
			byte uuidLengthAndVersion[18];
			
			//read 18 bytes - 128-bit UUID, length byte and a version byte
			readBytes(timeStep, uuidLengthAndVersion, 18);
			
			//parse the input. If no devices were found, give up
			unsigned int* p = (unsigned int*)uuidLengthAndVersion;
			for(int i = 0; i < 4; i++, p++)
			{
				empty = empty && (*p == (~0));	//responses that are all 1-bits are empty
				invalid = invalid && (*p == 0);	//and all 0-bits are invalid
			}
			empty = empty || invalid;	//invalid is the same as empty for us
			
			if(!empty)
			{
				byte descriptorLength = uuidLengthAndVersion[16] - 1;	//account for the fact that we already read the version byte
				byte* descriptor = new byte[descriptorLength + 18];	//include the 18 bytes we already read

				memcpy(descriptor, uuidLengthAndVersion, 18);
				
				readBytes(timeStep, descriptor + 18, descriptorLength);
				
				Endpoint** predecessor;
				byte address = nextAddress(predecessor);
				
				if(writeBytes(timeStep, &address, 1))	//assign an address
				{
					Endpoint* e = new Endpoint(uuidLengthAndVersion[17], address, descriptor, descriptorLength);
					insert(predecessor, e);	//add to linked list of endpoints
					
					found = true;
				}
				else
				{
					delete[] descriptor;
					empty = true;
				}
			}
		}
		//else empty is true and we exit
		
		io.sel = false;
		udelay(timeStep, 10);
	}
	while(!empty);
	
	*LPC1300::IOConfigPIO0_6 = oldSCKMode;
	return(found);
}

bool	AppBoard::details(byte address, DeviceDetails* detailsOut)
{
	Endpoint* e = endpoints;
	while((e != 0) && (e->address < address))
		e = e->next;
	
	if((e == 0) || (e->address != address))
		return(false);
	
	detailsOut->vendorID = e->vendorID();
	detailsOut->productID = e->productID();
	detailsOut->version = e->version();
	
	return(true);
}

bool	AppBoard::getUUID(byte address, byte* uuidOut, int length)
{
	Endpoint* e = endpoints;
	while((e != 0) && (e->address < address))
		e = e->next;
	
	if((e == 0) || (e->address != address))
		return(false);

	if(length > 16)
		length = 16;

	memcpy(uuidOut, e->uuid(), length);

	return(true);
}

byte	AppBoard::find(byte afterAddress, unsigned short vendorID, unsigned short productID)
{
	Endpoint* e = endpoints;
	while((e != 0) && (e->address < afterAddress))
		e = e->next;
	
	while(e != 0)
	{
		if(((vendorID == 0) || (e->vendorID() == vendorID)) && ((productID == 0) || (e->productID() == productID)))
			break;
		e = e->next;
	}
	
	return(e? (e->address) : 0);
}

bool	AppBoard::write(byte address, byte const* command, int length)
{
	if((address == 0) || (address >= 0xFE))
		return(false);
	return(internalWrite(address, command, length));
}

void	AppBoard::detach(byte address)
{
	remove(address);
	
	byte disconnect = 0xFF;
	internalWrite(address, &disconnect, 1);	//de-enumerate only this device
}

int			AppBoard::setupGXB(void)
{
	io.sel = false;
	io.sel.setOutput();
	io.sck.setOutput();	//we command sck until API exit, at which point we revert it.
	io.sck = false;
	
	return(system.getCoreFrequency() / 3000000UL);	//empirically determined, yields about 100KHz. Max per the spec is 1MHz
}

byte		AppBoard::nextAddress(Endpoint**& predecessor)
{
	byte address = 1;
	
	//iterate the list through a pointer-to-Endpoint that allows us to conveniently return both a valid address
	//  and the pointer to the preceding object in the linked list, even if it's the head pointer, this->endpoints.
	//  Doing it this way keeps the list sorted all the time, which has clear benefits.
	for(	predecessor = &endpoints;
			(*predecessor != 0) && (address >= (*predecessor)->address);	//while we're in the list and the current element has a lower address
			predecessor = &((*predecessor)->next)
		)
		address = (*predecessor)->address + 1;	//the presumed address is taken - move to the next available address
	
	return(address);
}

void		AppBoard::insert(Endpoint** predecessor, Endpoint* e)
{
	//because we already have a predecessor pointer-to-Endpoint, insertion into the linked list is trivial
	e->next = *predecessor;
	*predecessor = e;
}

void		AppBoard::remove(byte address)
{
	//find the endpoint with a matching address through a pointer-to-Endpoint
	//  this lets us remove it from the linked list in one go
	Endpoint** predecessor = &endpoints;
	//while we're in the list and haven't passed the desired address, iterate
	while((*predecessor != 0) && (address > (*predecessor)->address))
		predecessor = &((*predecessor)->next);
	
	//now check if we have a match
	//  it's slightly faster for lists longer than 1 to check after the iteration
	//  rather than during it
	if((*predecessor != 0) && ((*predecessor)->address == address))
	{
		Endpoint* d = *predecessor;
		*predecessor = d->next;
		delete d;
	}
	//else not found
}

bool		AppBoard::internalWrite(byte address, byte const* command, int length)
{
	int oldSCKMode = *LPC1300::IOConfigPIO0_6;
	int timeStep = setupGXB();
	io.sel = true;
	udelay(timeStep, 10);
	
	bool special = (!address) || (address == 0xFE);
	
	//write a byte, paying attention to the acknowledgement unless this is a special command
	if(!writeBytes(timeStep, &address, 1) && !special)
	{
		//no answer! forcibly de-enumerate the device, which is idempotent
		//  because we will need to reenumerate it anyway.
		udelay(timeStep, 10);
		io.sel = false;
		udelay(timeStep, 10);
		
		detach(address);	//implicit recursion
		*LPC1300::IOConfigPIO0_6 = oldSCKMode;
		return(false);
	}
	
	udelay(timeStep, 10);
	
	writeBytes(timeStep, command, length);
	
	udelay(timeStep, 10);
	io.sel = false;
	udelay(timeStep, 10);
	
	*LPC1300::IOConfigPIO0_6 = oldSCKMode;
	return(true);
}

void	AppBoard::udelay(int timeStep, int i)
{
	while(i--)
		for(int j = 0; j < timeStep; j++)
			__asm__ volatile ("nop" ::);
}

void	AppBoard::readBytes(int timeStep, byte* out, int length)
{
	unsigned int oldMode = *LPC1300::IOConfigPIO0_9, oldDir = (*LPC1300::GPIO0Dir & (1 << 9));
	*LPC1300::GPIO0Dir &= ~(1 << 9);	//mosi is an input
	*LPC1300::IOConfigPIO0_9 = (*LPC1300::IOConfigPIO0_9 & ~LPC1300::IOConfigPIO0_9_Repeat) | LPC1300::IOConfigPIO0_9_PullUp;
	while(length--)
	{
		byte cb = 0;
		for(int i = 0; i < 8; i++)
		{
			udelay(timeStep, 1);
			LPC1300::GPIO0[1 << 6] = (1 << 6);
			udelay(timeStep, 1);
			cb <<= 1;
			if(LPC1300::GPIO0[1 << 9])
				cb |= 1;
			LPC1300::GPIO0[1 << 6] = 0;
		}
		*out++ = cb;
	}
	*LPC1300::IOConfigPIO0_9 = oldMode;
	*LPC1300::GPIO0Dir = (*LPC1300::GPIO0Dir & ~(1 << 9)) | oldDir;
}
bool	AppBoard::writeBytes(unsigned int timeStep, byte const* bytes, int length)
{
	unsigned int oldMode = *LPC1300::IOConfigPIO0_9, oldDir = (*LPC1300::GPIO0Dir & (1 << 9));
	
	bool ack = true;
	while(ack && (length--))
	{
		int i = 8;
		byte b = *bytes++;
		
		*LPC1300::IOConfigPIO0_9 = (*LPC1300::IOConfigPIO0_9 & ~(LPC1300::IOConfigPIO0_9_Repeat | 3)) | LPC1300::IOConfigPIO0_9_Function_PIO;
		*LPC1300::GPIO0Dir |= (1 << 9);
		do
		{
			LPC1300::GPIO0[1 << 9] = (b & 0x80)? (1 << 9) : 0;
			b <<= 1;
			udelay(timeStep, 1);
			LPC1300::GPIO0[1 << 6] = (1 << 6);
			udelay(timeStep, 1);
			if(i == 1)
			{
				*LPC1300::GPIO0Dir &= ~(1 << 9);
				*LPC1300::IOConfigPIO0_9 = (*LPC1300::IOConfigPIO0_9 & ~LPC1300::IOConfigPIO0_9_Repeat) | LPC1300::IOConfigPIO0_9_PullUp;
			}
			LPC1300::GPIO0[1 << 6] = 0;
		}
		while(--i);
		
		udelay(timeStep, 1);
		LPC1300::GPIO0[1 << 6] = (1 << 6);
		udelay(timeStep, 1);
		ack = ack && (LPC1300::GPIO0[1 << 9] == 0);
		LPC1300::GPIO0[1 << 6] = 0;
	}
	
	*LPC1300::IOConfigPIO0_9 = oldMode;
	*LPC1300::GPIO0Dir = (*LPC1300::GPIO0Dir & ~(1 << 9)) | oldDir;
	return(ack);
}


namespace Logiblock { AppBoard appBoard; }




/*

struct Context
{
	int		count;
	byte	address;
	Context(): count(0)	{}
};

static const byte commands[] = {0x00, 0x01, 0x02, 0x10, 0x11, 0x12, 0x20, 0x21};

void	demo(void* v = 0, Task t = Task(), bool s = true)
{
	Context* c = (Context*)v;
	
	byte command = commands[++c->count & 7];
	
	if(!appBoard.write(c->address, &command, 1))
	{
		appBoard.detect();
		c->count = 0;
		
		c->address = appBoard.find();
	}
	
	system.when(system.delay(500), &demo, v);
}

int		main(void)
{
	appBoard.reset();
	appBoard.detect();
	
	Context* c = new Context();
	c->address = appBoard.find();
	
	demo(c);
	
	while(true)
		system.sleep();
}

*/
