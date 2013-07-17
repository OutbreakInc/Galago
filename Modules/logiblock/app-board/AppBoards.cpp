#include <GalagoAPI.h>
using namespace Galago;

#include "AppBoards.h"

#include <LPC13xx.h>

////////////////////////////////////////////////////////////////
// GXB - Galago expansion bus, a multiplexed SPI bus with autodiscovery and device addressing (experimental)

namespace Logiblock {

	struct Endpoint
	{
		Endpoint*		next;
		
		byte const*		descriptor;
		
		byte			descriptorLength;
		byte			address;
		byte			protocolVersion;
		
		unsigned short	vendorID() const	{return(((unsigned short*)descriptor)[0]);}
		unsigned short	productID() const	{return(((unsigned short*)descriptor)[1]);}
		unsigned short	version() const		{return(((unsigned short*)descriptor)[2]);}
		
						Endpoint(byte protoVersion, byte addr, byte const* desc, byte descLen):
							next(0),
							address(addr),
							protocolVersion(protoVersion),
							descriptor(desc),
							descriptorLength(descLen)
		{
		}
	};

}

using namespace Logiblock;


		AppBoards::AppBoards(void):
			endpoints(0)
{
}
void	AppBoards::reset(void)
{
	//empty endpoints list
	Endpoint* e = endpoints;
	endpoints = 0;
	while(e)
	{
		Endpoint* d = e;
		e = e->next;
		delete d->descriptor;
		delete d;
	}
	
	unsigned int kResetKeyLE = 0x012707C1; 	//= {0xC1, 0x07, 0x27, 0x01} - special 'all' address (0xFE) and reset key: 0xC1072701 (big-E)
	
	internalWrite(0xFE, (byte const*)&kResetKeyLE, 4);
}
bool	AppBoards::detect(void)
{
	int timeStep = setupGXB();
	
	bool empty, invalid, found = false;
	do
	{
		empty = invalid = true;
		
		io.sel = true;
		udelay(timeStep, 10);
		
		if(writeByte(timeStep, 0))	//address the autodetection pseudo-address
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
				byte* descriptor = new byte[descriptorLength];
				
				readBytes(timeStep, descriptor, descriptorLength);
				
				Endpoint** predecessor;
				byte address = nextAddress(predecessor);
				
				if(writeByte(timeStep, address))	//assign an address
				{
					Endpoint* e = new Endpoint(uuidLengthAndVersion[17], address, descriptor, descriptorLength);
					insert(predecessor, e);	//add to linked list of endpoints
					
					found = true;
				}
				else
					empty = true;
			}
		}
		//else empty is true and we exit
		
		io.sel = false;
		udelay(timeStep, 10);
	}
	while(!empty);
	
	return(found);
}

bool	AppBoards::details(byte address, DeviceDetails* detailsOut)
{
	Endpoint* e = endpoints;
	while((e != 0) && (e->address < address))
		e = e->next;
	
	if((e == 0) || (e->address != address))
		return(false);
	
	detailsOut->vendorID = e->vendorID();
	detailsOut->productID = e->productID();
	detailsOut->version = e->version();
}

byte	AppBoards::find(byte afterAddress, unsigned short vendorID, unsigned short productID)
{
	Endpoint* e = endpoints;
	while((e != 0) && (e->address < afterAddress))
	{
		if(((vendorID == 0) || (e->vendorID() == vendorID)) || ((productID == 0) || (e->productID() == productID)))
			break;
		e = e->next;
	}
	
	return(e? (e->address) : 0);
}

bool	AppBoards::write(byte address, byte const* command, int length)
{
	if((address == 0) || (address >= 0xFE))
		return(false);
	return(internalWrite(address, command, length));
}

void	AppBoards::detach(byte address)
{
	remove(address);
	
	byte disconnect = 0xFF;
	internalWrite(0xFE, &disconnect, 1);
}

int			AppBoards::setupGXB(void)
{
	io.spi.stop();
	io.sel = false;
	io.sel.setOutput();
	io.sck.setOutput();
	io.sck = false;
	
	int timeStep = system.getCoreFrequency() / 12000000UL;	//empirically determined, yields about 100KHz. Max per the spec is 1MHz
	
	return(timeStep);
}

byte		AppBoards::nextAddress(Endpoint**& predecessor)
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

void		AppBoards::insert(Endpoint** predecessor, Endpoint* e)
{
	//because we already have a predecessor pointer-to-Endpoint, insertion into the linked list is trivial
	e->next = *predecessor;
	*predecessor = e;
}

void		AppBoards::remove(byte address)
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

bool		AppBoards::internalWrite(byte address, byte const* command, int length)
{
	int timeStep = setupGXB();
	io.sel = true;
	udelay(timeStep, 10);
	
	bool special = (!address) || (address == 0xFE);
	
	//write a byte, paying attention to the acknowledgement unless this is a special command
	if(!writeByte(timeStep, address) && !special)
	{
		//no answer! forcibly de-enumerate the device, which is idempotent
		//  because we will need to reenumerate it anyway.
		udelay(timeStep, 10);
		io.sel = false;
		udelay(timeStep, 10);
		
		detach(address);
		return(false);
	}
	
	udelay(timeStep, 10);
	
	while(length--)
		writeByte(timeStep, *command++);
	
	udelay(timeStep, 10);
	io.sel = false;
	udelay(timeStep, 10);
	
	return(true);
}

void	AppBoards::udelay(int timeStep, int i)
{
	while(i--)
		for(int j = 0; j < timeStep; j++)
			__asm__ volatile ("nop" ::);
}

void	AppBoards::readBytes(int timeStep, byte* out, int length)
{
	*LPC1300::GPIO0Dir &= ~(1 << 9);
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
}
bool	AppBoards::writeByte(unsigned int timeStep, byte b)
{
	*LPC1300::IOConfigPIO0_9 = (*LPC1300::IOConfigPIO0_9 & ~(LPC1300::IOConfigPIO0_9_Repeat | 3)) | LPC1300::IOConfigPIO0_9_Function_PIO;
	*LPC1300::GPIO0Dir |= (1 << 9);
	int i = 8;
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
	bool ack = (LPC1300::GPIO0[1 << 9] == 0);
	LPC1300::GPIO0[1 << 6] = 0;
	return(ack);
}


AppBoards appBoards;




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
	
	if(!appBoards.write(c->address, &command, 1))
	{
		appBoards.detect();
		c->count = 0;
		
		c->address = appBoards.find();
	}
	
	system.when(system.delay(500), &demo, v);
}

int		main(void)
{
	appBoards.reset();
	appBoards.detect();
	
	Context* c = new Context();
	c->address = appBoards.find();
	
	demo(c);
	
	while(true)
		system.sleep();
}

*/
