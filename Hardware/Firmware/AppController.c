///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////

#include "AppController.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#define PIN_SPI_SEL			PB3
#define PIN_SPI_SCK			PB2
#define PIN_SPI_MOSI		PB0

#define PINMASK_SPI_SEL		(1 << PIN_SPI_SEL)
#define PINMASK_SPI_SCK		(1 << PIN_SPI_SCK)
#define PINMASK_SPI_MOSI	(1 << PIN_SPI_MOSI)


//There are three reserved GXB addresses, used for stateless group addressing of all
//  unenumerated devices (0x00, 'probe address') and all enumerated devices
//  (0xFE, 'broadcast address').  Address 0xFF is reserved for communication robustness.

#define BUS_PROBE_ADDRESS		(0x00)
#define BUS_BROADCAST_ADDRESS	(0xFE)
#define BUS_RESERVED_ADDRESS	(0xFF)

///////////////////////////////////////////////////////////////////////////////////////////

u8		gSPIPseudovalue;

//Performs an SPI transaction (sends and receives a byte) in master mode
u8		SPIExchangeMaster(u8 byte)
{
	USIDR = byte;
	for(u8 i = 0; i < 8; i++)
	{
		USICR = 0x11;
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		USICR = 0x13;
	}
	return(USIDR);
}

//Setup or take down the SPI interface for a slave transfer
void	SPISetupSlave(void)
{
	gSPIPseudovalue = (gIOState & (1 << 1))? 0xFF : 0x00;
	USIDR = gSPIPseudovalue;
	USICR = 0x18;	//3-wire bus, external clock, shift on falling, read on rising edge, interrupt armed
}
void	SPIDisableSlave(void)
{
	USICR = 0x00;
}

u8		SPIExchangeSlave(u8 byte, u8 output)
{
	//only load the value if the pin is marked as output, else use the pseudovalue set in SPISetupSlave()
	USIDR = output? byte : gSPIPseudovalue;
	
	u8 out;
	
	//tricky - after 4 bits, fill the next 4 with the appropriate constant value to prevent output glitches
	USISR = 0xE8;
	while(!(USISR & 0x40));	//wait until USIOIF is 1
	
	out = USIDR;
	if(gIOState & (1 << 1))	USIDR |= 0x0F;
	else					USIDR &= ~0x0F;
	
	USISR = 0xE8;	//count another 8 edges
	while(!(USISR & 0x40));	//wait for the remaining 4 bits
	
	return(((out << 4) & 0xF0) | (USIDR & 0x0F));	//fuse the nibbles together
}


void	SPISetupGXB(void)
{
	//set up MOSI as a psuedo-open-drain shift register output
	DDRB &= ~PINMASK_SPI_MOSI;
	PORTB &= ~PINMASK_SPI_MOSI;
}

void	SPIDisableGXB(void)
{
	DDRB &= ~PINMASK_SPI_MOSI;
}

u8		SPIExchangeGXB(u8 byte)
{
	for(u8 bitIdx = 0; bitIdx < 8; bitIdx++)
	{
		while((PINB & PINMASK_SPI_SCK));	//wait while SCK is high
		
		//SCK now low, shift and open-drain-drive a bit
		if(byte & 0x80)
		{
			PORTB |= PINMASK_SPI_MOSI;
			DDRB |= PINMASK_SPI_MOSI;
			DDRB &= ~PINMASK_SPI_MOSI;	//release line for 1
			PORTB &= ~PINMASK_SPI_MOSI;
		}
		else
		{
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			DDRB |= PINMASK_SPI_MOSI;	//sink line for 0
		}
		
		while(!(PINB & PINMASK_SPI_SCK));	//wait while SCK is low
		
		//SCK now high, check if there's a bus conflict
		if((byte & 0x80) && (!(PINB & PINMASK_SPI_MOSI)))	//if we're trying to drive a 1 (i.e. not sinking) and the line reads 0
			return(0);
		
		//no conflict, shift to next bit
		byte <<= 1;
	}
	
	while((PINB & PINMASK_SPI_SCK));	//wait while SCK is high on the final bit
	return(1);	//success!
}

void	SPIAckGXB(void)
{
	//ack bits are logic 0
	DDRB |= PINMASK_SPI_MOSI;	//sink line for 0
	PORTB &= ~PINMASK_SPI_MOSI;

	while(!(PINB & PINMASK_SPI_SCK));	//wait while SCK is low
	while((PINB & PINMASK_SPI_SCK));	//wait while SCK is high
	
	DDRB &= ~PINMASK_SPI_MOSI;
}


u8		ExchangeROM(void)
{
	SPISetupGXB();
	
	//length = the UUID, the length byte and all extra bytes described by the length byte
	u8 romLength = pgm_read_byte(kUUIDAndDescriptor + 16) + 16 + 1;
	
	for(u8 byteIdx = 0; byteIdx < romLength; byteIdx++)
		if(!SPIExchangeGXB(pgm_read_byte(kUUIDAndDescriptor + byteIdx)))
			return(0);	//bus collision
	
	//success!
	SPIDisableGXB();
	return(1);
}

//corrupts the stack in a specific way and jumps to core (like longjmp'ing back to core())
void	escape(void)
{
	__asm__ volatile
	(
		"eor	r1, r1		\n"
		"out	0x3f, r1	\n"
		"ldi	r28, 0x5F	\n"	//registers 0x3d:0x3e = 0x15f (initial stack value 0xff)
		"ldi	r29, 0x01	\n"
		"out	0x3e, r29	\n"
		"out	0x3d, r28	\n"
		"rcall	core		\n"
		"rcall	_exit		\n"
		::
	);
}

//handle a change on the SEL pin
ISR(PCINT0_vect)
{
	//on a falling edge, latch the new IO state and simply reboot.
	//  after booting, main() will put us back to sleep
	if(!(PINB & PINMASK_SPI_SEL))
	{
		//commit the new IO state
		PORTB = gIOState;
		DDRB = gIODir;
		
		escape();
	}
	//else do nothing but wake up
}

//our address
u8		gAddress;

void	init(void)
{
	gAddress = BUS_PROBE_ADDRESS;
	
	appInit();
	PORTB = gIOState;
	DDRB = gIODir;
}

//This is the special 32-bit key that, when sent to broadcast address 0xFE, de-enumerates all devices.
//  It's used to reset the bus from any state. 
u8 const kBusResetKey[4] PROGMEM =
{
	0xC1, 0x07, 0x27, 0x01
};

void	core(void)
{
	while(1)
	{
		sei();
		do
		{
			PCMSK |= PINMASK_SPI_SEL;	//interrupt when SEL changes
			GIFR = (1 << PCIF);			//reset SEL interrupt flag
			GIMSK |= (1 << PCIE);		//enable interrupt
			
			set_sleep_mode(SLEEP_MODE_IDLE);
			//we will wake when the SEL pin goes high again
			sleep_mode();
		}
		while(!(PINB & PINMASK_SPI_SEL));	//if SEL is 0, go back to sleep
		
		appReleaseBus();
		DDRB = gIODir;
		PORTB = gIOState;
		
		GIFR = (1 << PCIF);			//reset SEL interrupt flag
		
		SPISetupSlave();
		u8 addr = SPIExchangeSlave(0, 0);
		SPIDisableSlave();
		
		//if the address matches us - our address is initialized to the bus-probe address, so
		//  if we're not enumerated and the probe address is sent, respond to that
		if(addr == gAddress)
		{
			SPIAckGXB();
			if(addr == BUS_PROBE_ADDRESS)
			{
				//exchange the ROM.  If a bus conflict occurs, we're not the highest-ranking unaddressed
				//  device so wait for the next addressing cycle.
				if(ExchangeROM())
				{
					//we're in addressing mode so take an address assignment.
					SPISetupSlave();
					addr = SPIExchangeSlave(0, 0);
					SPIDisableSlave();
					SPIAckGXB();
					cli();	//disable interrupts until the outer loop repeats, so that we're hardened against race conditions
					
					if(addr < BUS_BROADCAST_ADDRESS)	//don't accept invalid addresses (0 is ok, becuase that unassigns us)
						gAddress = addr;
				}
				//else a bus conflict occurred
				
				//done with this cycle, go to sleep
			}
			else	//direct command
			{
				SPISetupSlave();
				u8 method = SPIExchangeSlave(0, 0);
				SPIAckGXB();
				if(method == 0xFF)
				{
					//de-enumerate from the bus
					gAddress = BUS_PROBE_ADDRESS;
				}
				else
					appCommand(method);
				SPIDisableSlave();
			}
		}
		else if(addr == BUS_BROADCAST_ADDRESS)
		{
			SPIAckGXB();
			SPISetupSlave();
			u8 match = 1;
			for(u8 i = 0; i < 4; i++)
			{
				match = match && (pgm_read_byte(kBusResetKey + i) == SPIExchangeSlave(0, 0));
				SPIAckGXB();
			}
			cli();	//harden against race conditions
			SPIDisableSlave();
			
			if(match)
				init();
			//we now return to the initial state, as if core() was called from main (or escape() was called)
		}
		//else go back to sleep
	}
}

int		main(void)
{
	init();	
	core();
}
