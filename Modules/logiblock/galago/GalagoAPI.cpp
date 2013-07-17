#include "GalagoAPI.h"
#include "LPC13xx.h"

//strictly a property of the circuit the chip is soldered to
#define HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY (12000000UL)


using namespace LPC1300;
using namespace Galago;

extern int __heap_start__;
extern int __heap_end__;

static unsigned int* const	__heapstart = (unsigned int* const)(&__heap_start__);
static unsigned int* const	__heapend = (unsigned int* const)(&__heap_end__);

namespace Galago
{
	System	system;
	IO		io;
}

////////////////////////////////////////////////////////////////
// Utility functions

// not to be confused with the very similar function, strlen()
int		stringZeroLength(byte const* s)
{
	int length = 0;
	while(*s++)
		length++;
	return(length);
}

extern "C"
void	memcpy(void* dest, void const* source, size_t length)
{
	byte* d = (byte*)dest;
	byte const* s = (byte const*)source;
	while(length--)
		*d++ = *s++;
}

////////////////////////////////////////////////////////////////
// NumberFormatter formats numbers into text

class NumberFormatter
{
public:
	typedef enum
	{
		Hexadecimal = 16,
		DecimalUnsigned = 10,
		DecimalSigned = 11
	} Base;
	
	static int		format(char* output, unsigned int number, int fractionBits, Base base)
	{
		static char const charTable[] = "0123456789ABCDEF";
		char buf[11];
		int len = 0;
		bool neg = false;
		
		if(base == DecimalSigned)
		{
			base = DecimalUnsigned;
			if(((int)number) < 0)
			{
				neg = true;
				number = -number;
			}
		}
		
		do
		{
			buf[len++] = charTable[number % (int)base];
			number /= (int)base;
		}
		while((len < 10) && (number != 0));
		
		if(neg)
			buf[len++] = '-';
		
		for(int i = len; i > 0;)
			*output++ = buf[--i];
		
		return(len);
	}
};

////////////////////////////////////////////////////////////////
// Tasks are promises for functionality

Task&					Task::operator =(Task const& t)
{
	refer(t._t);
	release(_t);
	_t = t._t;
	return(*this);
}

struct TaskUnion
{
	Task			self;
	unsigned short	count;
	
					TaskUnion(unsigned short initialCount = 2):
						count(initialCount)
						{}
};

void	TaskUnion_onCompletion(void* context, Task t, bool success)
{
	TaskUnion* u = (TaskUnion*)context;
	Task self = u->self;
	
	if(!success)
	{
		if(--u->count == 0)
			delete u;
		
		system.completeTask(self, false);
	}
	else if(--u->count == 0)
	{
		delete u;
		
		system.completeTask(self, true);
	}
}

Task				Task::operator +(Task const& r) const
{
	TaskUnion* u = new TaskUnion();
	u->self = system.createTask();
	system.when(*this, &TaskUnion_onCompletion, u);
	system.when(r, &TaskUnion_onCompletion, u);
	
	return(u->self);
}

void				Task::release(InternalTask* t)	//static
{
	if(t && (--t->_rc == 0))
		t->destroy();
}

////////////////////////////////////////////////////////////////
// CircularBuffer is a circular buffer

		CircularBuffer::CircularBuffer(int size)
{
	_start = _head = _tail = new byte[size];
	_end = _start + size;
}
		CircularBuffer::~CircularBuffer(void)
{
	delete[] _start;
}
bool	CircularBuffer::write(byte b)
{
	byte* n = _head + 1;
	if(n == _end)	n = _start;
	if(n == _tail)	return(false);
	*(_head = n) = b;
	return(true);
}
int	CircularBuffer::write(byte const* b, int length)
{
	int count = 0;
	while((length--) && write(*b++))	count++;
	return(count);
}

bool	CircularBuffer::read(byte* b)
{
	if(_tail == _head)	return(false);	//empty
	byte* n = _tail + 1;
	if(n == _end)	n = _start;
	*b = *(_tail = n);
	return(true);
}

int		CircularBuffer::read(byte* b, int length)
{
	int count = 0;
	while((length--) && read(b++))	count++;
	return(count);
}

int		CircularBuffer::bytesUsed(void) const
{
	if(_tail > _head)
		return((_end - _tail) + (_head - _start));
	else
		return(_head - _tail);
}

int		CircularBuffer::bytesFree(void) const
{
	if(_tail > _head)
		return(_head - _tail);
	else
		return((_end - _tail) + (_head - _start));
}


////////////////////////////////////////////////////////////////
// Buffer is a reference-counted mutable string of data perfect for
//   sharing with asynchronous APIs

Buffer&					Buffer::operator =(Buffer const& b)
{
	refer(b._b);
	release(_b);
	_b = b._b;
	return(*this);	
}

Buffer					Buffer::New(char const* cStr)
{
	if(cStr == 0)	return(Buffer());
	size_t length = stringZeroLength((byte const*)cStr);
	InternalBuffer* ib = new(length) InternalBuffer(length);
	memcpy(ib->data, cStr, length + 1);
	return(Buffer(ib));
}

Buffer					Buffer::New(size_t length)
{
	if(length == 0)	return(Buffer());
	InternalBuffer* ib = new(length) InternalBuffer(length);
	return(Buffer(ib));
}

Buffer					Buffer::New(void const* d, size_t length)
{
	if((d == 0) || (length == 0))	return(Buffer());
	InternalBuffer* ib = new(length) InternalBuffer(length);
	memcpy(ib->data, d, length);
	return(Buffer(ib));
}

Buffer					Buffer::operator +(Buffer const& b) const
{
	if(_b == 0)		return(b);
	if(b._b == 0)	return(*this);
	
	size_t totalLength = _b->length + b._b->length;
	InternalBuffer* newBuffer = new(totalLength) InternalBuffer(totalLength);
	memcpy(newBuffer->data, _b->data, _b->length);
	memcpy(newBuffer->data + _b->length, b._b->data, b._b->length);
	
	return(Buffer(newBuffer));
}

Buffer&					Buffer::operator +=(Buffer const& b)
{
	return(operator =(operator +(b)));
}

bool					Buffer::operator ==(char const* cStr) const
{
	if((_b == 0) || (cStr == 0))
		return((_b == 0) && (cStr == 0));
	
	size_t length = stringZeroLength((byte const*)cStr);
	
	return(equals((byte const*)cStr, length));
}

bool					Buffer::startsWith(byte const* str, size_t length) const
{
	if((_b == 0) || (length == 0) || (length > _b->length))
		return(length == 0);
	
	//return(memcmp(_b->data, str, length) == 0);
	for(size_t i = 0; i < length; i++)
		if(*str++ != _b->data[i])
			return(false);
	
	return(true);
}

bool					Buffer::startsWith(char const* cStr) const
{
	size_t length = cStr? stringZeroLength((byte const*)cStr) : 0;
	
	return(startsWith((byte const*)cStr, length));
}

bool					Buffer::equals(byte const* str, size_t length) const
{
	if(_b == 0)
		return(length == 0);
	
	return((length == _b->length) && startsWith(str, length));
}

static bool		isAsciiBinaryChar(char ascii)
{
	return((ascii & ~1) == '0');
}

static bool		isAsciiOctalChar(char ascii)
{
	return((ascii - '0') < 8);
}

static bool		isAsciiDecimalChar(char ascii)
{
	return((ascii - '0') < 10);
}

static int		asciiHexChar(char ascii)
{
	if((ascii - '0') < 10)		return(ascii - '0');
	ascii -= ('a' -'A');
	if((ascii - 'A') < 6)		return(10 + ascii - 'A');
	else						return(-1);
}

unsigned int			Buffer::parseUint(int base)
{
	if(_b == 0)	return(0);
	
	size_t length = _b->length;
	char const* ascii = (char const*)_b->data;
	unsigned int value = 0;
	
	switch(base)
	{
	case 2:	//big-endian binary
		while(isAsciiBinaryChar(*ascii) && (length-- > 0))
		{
			value <<= 1;
			value |= (*ascii++) - '0';
		}
		break;
		
	case 8:
		while(isAsciiOctalChar(*ascii) && (length-- > 0))
		{
			value <<= 3;
			value += (*ascii++) - '0';
		}
		break;
		
	case 10:
		while(isAsciiDecimalChar(*ascii) && (length-- > 0))
		{
			value *= 10;
			value += (*ascii++) - '0';
		}
		break;
		
	case 16:	//big-endian hexadecimal
		{
			int asciiChar;
			while(((asciiChar = asciiHexChar(*ascii++)) >= 0) && (length-- > 0))
				value = (value << 4) | asciiChar;
		}
		break;
	
	default:
		//@@raise exception
		break;
	}
	
	return(value);
}

signed int				Buffer::parseInt(int base)
{
	if(_b == 0)	return(0);
	
	size_t length = _b->length;
	char const* ascii = (char const*)_b->data;
	int value = 0;
	
	switch(base)
	{
	case 10:
		while(isAsciiDecimalChar(*ascii) && (length-- > 0))
		{
			value *= 10;
			value += (*ascii++) - '0';
		}
		break;
	
	default:
		//@@raise exception
		break;
	}
	
	return(value);
}

ssize_t					Buffer::indexOf(byte b, size_t offset)
{
	if(_b == 0)
		return(-1);
	
	if(offset > _b->length)
		return(-1);
	
	for(int i = offset; i < _b->length; i++)
		if(_b->data[i] == b)
			return(i);
	return(-1);
}

/*
//slow but compact implementation with minimum code size
//  ok for small strings
ssize_t					Buffer::IndexOf(Buffer b, size_t offset)
{
	if((_b == 0) || (b._b == 0))
		return(-1);
	
	if((offset > _b->length) || ((offset + b._b->length) > _b->length))
		return(-1);
	
	for(int i = offset, j; i < (_b->length - b._b->length);)
	{
		for(j = 0; (j < b._b->length) && (_b->data[i] == b._b->data[i]); i++) j++;
		if(j == b._b->length)
			return(i - b._b->length);
		i += j;
	}
	return(-1);
}
*/

//much faster (but less compact) implementation based on a modified Boyer-Moore-Horspool algorithm
//  much, much faster when looking for long strings
//  *appears to compile to ~160 bytes of Thumb2, further proving the genius of ARM
ssize_t					Buffer::indexOf(Buffer b, size_t offset)
{
	unsigned char jumpTable[16];
	
	if((_b == 0) || (b._b == 0) || (offset > _b->length) || ((offset + b._b->length) > _b->length))
		return(-1);
	
	unsigned char l = (b._b->length <= 255)? b._b->length : 255;
	for(int i = 0; i < 16; i++)
		jumpTable[i] = l;
	
	size_t last = b._b->length - 1;
	
	for(size_t i = 0; i < last; i++)
	{
		unsigned char* p = jumpTable + (b._b->data[i] & 0xF);
		if((last - i) < *p)
			*p = last - i;
	}
	
	unsigned char const* h = _b->data + offset;
	size_t len = _b->length - offset;
	while(len >= b._b->length)
	{
		for(size_t i = last; h[i] == b._b->data[i]; i--)
			if(i == 0)
				return(h - _b->data);
		size_t jump = jumpTable[h[last] & 0xF];
		len -= jump;
		h += jump;
	}
	
	return(-1);
}

void					Buffer::release(InternalBuffer* b) //static
{
	if(b && (--b->_rc == 0))
		delete b;
}

////////////////////////////////////////////////////////////////
// IOCore is an internal class for managing IO activity

void	System_onSysTickInterruptStub(void);

struct IOCore
{
	struct TaskQueueItem
	{
		TaskQueueItem*	next;
		Task			task;
		
						TaskQueueItem(void):
							next(0)
		{}
	};
	
	struct TimerTask: public TaskQueueItem
	{
		unsigned int	span;	//timespan from now until the task is due, in ms
		
						TimerTask(unsigned int s): span(s)
		{}
	};
	
	struct WriteTask: public TaskQueueItem
	{
		unsigned short	idx;
		unsigned short	len;
		byte			data[1];
		
						WriteTask(int l):
							idx(0),
							len(l)
		{}
						WriteTask(byte const* bytes, int l):
							idx(0),
							len(l)
		{
			memcpy(data, bytes, l);
		}
	};
	
	struct SPITask: public TaskQueueItem
	{
		unsigned short	len;
		unsigned short	writeIdx;
		unsigned short	readIdx;
		byte			is16Bit;
		Buffer			bytesReadBack;
		union
		{
			byte			data[1];
			unsigned short	data16[1];
		};
		
						SPITask(int l, Buffer readBack, bool is16 = false):
							len(l),
							writeIdx(0),
							readIdx(0),
							is16Bit(is16),
							bytesReadBack(readBack)
		{}
						SPITask(byte const* bytes, int l, Buffer readBack):
							len(l),
							writeIdx(0),
							readIdx(0),
							is16Bit(false),
							bytesReadBack(readBack)
		{
			memcpy(data, bytes, l);
		}
						SPITask(unsigned short const* halves, int l, Buffer readBack):
							len(l),
							writeIdx(0),
							readIdx(0),
							is16Bit(true),
							bytesReadBack(readBack)
		{
			for(int i = 0; i < l; i++)
				data16[i] = *halves++;
		}
	};
	
	struct I2CTask: public TaskQueueItem
	{
		unsigned short	len;
		unsigned short	idx;
		Buffer			bytesReadBack;
		byte			data[1];
		
						I2CTask(int l):
							len(l),
							idx(0)
		{
		}
	};
	
	void						(*irqSystick)(void);
	void						(*irqUART)(void);
	void						(*irqSPI)(void);
	void						(*irqI2C)(void);
	
	TimerTask* volatile			timerCurrentTask;
	
	CircularBuffer* volatile	uartReceiveBuffer;
	Task						uartRecvTask;
	WriteTask* volatile			uartCurrentWriteTask;
	
	I2CTask* volatile			i2cCurrentTask;
	
	SPITask* volatile			spiCurrentTask;
	
								IOCore(void):
									irqSystick(System_onSysTickInterruptStub),
									irqUART(0),
									irqSPI(0),
									irqI2C(0),
									timerCurrentTask(0),
									uartReceiveBuffer(0),
									uartCurrentWriteTask(0),
									i2cCurrentTask(0),
									spiCurrentTask(0)
	{
	}
	
	static void		queueItem(TaskQueueItem** head, TaskQueueItem* item)
	{
		while(*head != 0)
			head = &((*head)->next);
		*head = item;
	}
	
	static void		flushQueue(TaskQueueItem** head)
	{
		IOCore::TaskQueueItem* task = *head;
		*head = 0;
		while(task != 0)
		{
			IOCore::TaskQueueItem* t = task;
			task = task->next;
			system.completeTask(t->task, false);	//cancel the task
			delete t;
		}
	}
};

static IOCore IOCore;

////////////////////////////////////////////////////////////////
// System

				System::System(void)
{
	//initialize heap
	*(unsigned int*)__heapstart = (__heapend - __heapstart);
	
	//enable IO configuration
	*ClockControl |= ClockControl_IOConfig;
	
	*InterruptEnableClear0 = (~0);
	*InterruptEnableClear1 = (~0);
	
	//The tickrate is 400kHz
	*SystickClockDivider = 30;	//because the chip starts at 12MHz initially. At 72MHz this would be 180.
	
	InterruptsEnable();	//enable global interrupts
}

static unsigned int	System_getPLLInputFrequency(void)
{
	switch(*PLLSource)
	{
	default:
	case PLLSource_InternalCrystal:
		return(12000000UL);
	case PLLSource_ExternalClock:
		return(HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY);
	}
}

static unsigned int	System_getPLLOutputFrequency(void)
{
	//the PLL on the LPC1xxx parts isn't very well designed, as the fixed relationship between
	//  FCLKOUT, the M-divider and FCLKIN means that the output is always an integer multiple
	//  of the input.  A better design would use relationship: Fcco / M = Fin; Fout = Fcco / D.
	//  (The LPC1xxx uses relationship: Fcco / D / M = Fin; Fout = Fcco / D.)
	
	return(System_getPLLInputFrequency() * ((*PLLControl & PLLControl_MultiplierBitsMask) + 1));
}

unsigned int	System::getMainClockFrequency(void) const
{
	switch(*MainClockSource)
	{
	default:
	case MainClockSource_InternalCrystal:
		return(12000000UL);
	case MainClockSource_PLLInput:
		return(System_getPLLInputFrequency());
	case MainClockSource_WDTOscillator:
		return(3400000UL);		//@@todo: confirm this is a reliable figure
	case MainClockSource_PLLOutput:
		return(System_getPLLOutputFrequency());
	}
}

//this is also the AHB (main high-speed bus) frequency
unsigned int	System::getCoreFrequency(void) const
{
	//note no /0 check, but I would be very alarmed if this code could excecute with a stopped core :-/
	return(getMainClockFrequency() / *MainBusDivider);
}

void			System_strobeClockUpdateEnable(REGISTER updateEnable)
{
	*updateEnable = 1;
	*updateEnable = 0;
	*updateEnable = 1;
	
	while(!(*updateEnable & 1));	//spinwait for the change to take effect before returning
}

//returns a valid divisor that would produce a frequency as close as possible to 'd' from source 'n'
unsigned int	System_divideClockFrequencyRounded(unsigned int n, unsigned int d)
{
	unsigned int q = (((n << 1) / d) + 1) >> 1;
	return(q? q : 1);
}

//returns a dimensionless error value 
unsigned int	System_computeClockError(unsigned int n, unsigned int d, unsigned int desiredFrequency)
{
	int err = ((int)(n / d)) - (int)desiredFrequency;
	return((err < 0)? -err : err);
}

void			System::setCoreFrequency(unsigned int desiredFrequency)
{
	//128KHz is established as a practical minimum
	if(desiredFrequency < 128000UL)
		desiredFrequency = 128000UL;
	if(desiredFrequency > 72000000UL)
		desiredFrequency = 72000000UL;
	
	//ensure the internal crystal is on
	*PowerDownControl &= ~(PowerDownControl_InternalCrystalOutput | PowerDownControl_InternalCrystal);
	
	*MainClockSource = MainClockSource_InternalCrystal;
	System_strobeClockUpdateEnable(MainClockSourceUpdate);
	
	//ensure the external crystal is on, as it's used in both cases.
	*PowerDownControl &= ~PowerDownControl_SystemOscillator;
	
	if(desiredFrequency > 12000000UL)	//must use PLL > 12MHz	@@to MAX(12e6, HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY)
	{
		//turn on the PLL
		*PowerDownControl &= ~PowerDownControl_SystemPLL;
		
		*PLLSource = PLLSource_ExternalClock;
		System_strobeClockUpdateEnable(PLLSourceUpdate);
		
		//pick a multiplier in the range [2..6] and divider such that the error is as small as possible
		unsigned int bestM = 2, bestD = 1, smallestError = 0xFFFFFFFF, busFrequency;
		for(unsigned int m = 2; m < 7; m++)
		{
			busFrequency = HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY * m;
			unsigned int div = System_divideClockFrequencyRounded(busFrequency, desiredFrequency);
			
			int error = (int)desiredFrequency - (int)(busFrequency / div);
			if(error < 0)	error = -error;	//use abs(error)
			
			if(error < smallestError)
			{
				smallestError = error;
				bestM = m;
				bestD = div;
			}
		}
		
		//set the PLL's multiplication factor
		*PLLControl = bestM - 1;
		
		//wait for lock
		while(!(*PLLStatus & PLLStatus_Locked));
		
		//run the core from the PLL
		*MainClockSource = MainClockSource_PLLOutput;
		System_strobeClockUpdateEnable(MainClockSourceUpdate);
		
		//with this divider:
		*MainBusDivider = bestD;
		
		//adjust the SysTick timer to have a nominal rate of 400kHz
		*SystickClockDivider = busFrequency / 400000;
	}
	else
	{
		//run the core from the external clock with a divider
		
		//select the external clock as the input to the PLL
		*PLLSource = PLLSource_ExternalClock;
		System_strobeClockUpdateEnable(PLLSourceUpdate);
		
		//drive the main clock from the PLL input (ignores the PLL)
		*MainClockSource = MainClockSource_PLLInput;
		System_strobeClockUpdateEnable(MainClockSourceUpdate);
		
		//compute the closest division ratio
		*MainBusDivider = System_divideClockFrequencyRounded(HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY, desiredFrequency);
		
		//turn off the PLL if it was on
		*PowerDownControl |= PowerDownControl_SystemPLL;
		
		//adjust the SysTick timer to have a nominal rate of 400kHz
		*SystickClockDivider = HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY / 400000;
	}
}

unsigned int	System::getClockOutputFrequency(void) const
{
	unsigned int fMain;
	switch(*ClockOutputSource)
	{
	case ClockOutputSource_InternalCrystal:
		fMain = 12000000UL;
		break;
	case ClockOutputSource_ExternalOscillator:
		fMain = HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY;
		break;
	case ClockOutputSource_WDTOscillator:
		fMain = 3400000UL;
		break;
	case ClockOutputSource_MainClock:
		fMain = getMainClockFrequency();
		break;
	}
	
	unsigned int div = *ClockOutputDivider;
	return(div? (fMain / div) : 0);
}

void	System::setClockOutputFrequency(unsigned int desiredFrequency)
{
	if(desiredFrequency > 0)
	{
		unsigned int busFrequency = getMainClockFrequency(), divisor, source;
		
		//Note: The WDT clock source was removed because it produces inconsistent results.
		
		//which is closest when divided to the desired frequency?
		//  the internal/external oscillator at 12MHz, the PLL output or the WDT oscillator?
		unsigned int d0 = System_divideClockFrequencyRounded(12000000, desiredFrequency);
		unsigned int d1 = System_divideClockFrequencyRounded(busFrequency, desiredFrequency);
		//unsigned int d2 = System_divideClockFrequencyRounded(3400000, desiredFrequency);
		
		unsigned int e0 = System_computeClockError(12000000, d0, desiredFrequency);
		unsigned int e1 = System_computeClockError(busFrequency, d1, desiredFrequency);
		//unsigned int e2 = System_computeClockError(3400000, d2, desiredFrequency);
		
		//if(e0 < e2)
		if(1)
		{
			if(e0 < e1)
			{
				divisor = d0;
				source = (*MainClockSource == MainClockSource_InternalCrystal)?
					ClockOutputSource_InternalCrystal :
					ClockOutputSource_ExternalOscillator;
			}
			else
				{divisor = d1; source = ClockOutputSource_MainClock;}
		}
		/*
		else
		{
			if(e1 < e2)
				{divisor = d1; source = ClockOutputSource_MainClock;}
			else
			{
				divisor = d2;
				*PowerDownControl &= ~PowerDownControl_WatchdogOscillator;
				source = ClockOutputSource_WDTOscillator;
			}
		}
		*/
		
		*ClockOutputSource = source;
		System_strobeClockUpdateEnable(ClockOutputSourceUpdate);
		
		*ClockOutputDivider = divisor;
	}
	else
		*ClockOutputDivider = 0;	//disable
}


////////////////////////////////////////////////////////////////
// Memory management


static const unsigned int kHeapFlagMask = 0xFF000000;
static const unsigned int kHeapAllocated = 0x80000000;

//static
void*			System::alloc(size_t size)
{
	if(size == 0)
		return(0);	//@@throw?
	
	unsigned int* m = (unsigned int*)__heapstart;
	
	//allocations are (4 + size) bytes, rounded up to the next 4-byte boundary
	unsigned int s = (unsigned int)((size + 7) >> 2);
	
	while(m < __heapend)
	{
		unsigned int bs = *m & ~kHeapFlagMask;
		//look for an unallocated block big enough to fit this allocation
		if(!(*m & kHeapAllocated) && (s < bs))
		{
			if((bs - s) > 1)		//if there's any meaningful remainder,
				m[s] = (bs - s);	//	mark the remainder as free
			else
				s = bs;	//expand allocation
			
			*m = s | kHeapAllocated;	//allocate
			
			for(int i = 1; i < s; i++)	//zero memory
				m[i] = 0;
			
			return(m + 1);
		}
		m += bs;
	}
	__asm volatile("bkpt 7"::);	//@@ Out-of-memory exception
}

//static
void			System::free(void* allocation)
{
	unsigned int* a = (unsigned int*)allocation;
	
	if((a < __heapstart) || (a >= __heapend) || !(a[-1] & kHeapAllocated))
		return;	//@@throw?
	
	a--;	//unwrap
	*a &= ~kHeapAllocated;	//free block

	unsigned int cbs = (*a & ~kHeapFlagMask);	//current block size
	
	for(int i = 1; i < cbs; i++)	//mark memory as free
		a[i] = 0xfeeefeee;
	
	//defragment heap
	unsigned int* n = a + cbs;				//next block
	if(!(*n & kHeapAllocated))						//if the next block is free,
		cbs += (*n & ~kHeapFlagMask);				//	account for it when freeing
	
	for(unsigned int* p = (unsigned int*)__heapstart; p < a;)
	{
		unsigned int pbs = *p & ~kHeapFlagMask;
		if(((p + pbs) == a) && !(*p & kHeapAllocated))	//predecessor is free
		{
			a = p;
			cbs += pbs;
		}
		p += pbs;
	}
	
	*a = cbs;	//merge free blocks if contiguous
}


////////////////////////////////////////////////////////////////
// Tasks, continued

namespace Galago {

struct InternalTaskCallback
{
	void	(*f)(void* context, Task, bool success);
	void*	c;
};

}

void			InternalTask::destroy(void)
{
	if((_flags & 0x3FFF) > 0)
		delete[] _c;
	delete this;
}

Task			System::createTask(void)
{
	//when tasks are created, they're created as unresolved promises with no callback capability
	InternalTask* task = new InternalTask();
	task->_rc = task->_flags = 0;
	task->_c = 0;
	
	return(Task(task));
}

bool			System::completeTask(Task t, bool success)
{
	if((t._t == 0) || (t._t->_flags > 0x3FFF))	//if invalid or already resolved, fail
		return(false);
	
	t._t->_flags |= (success? (1 << 14) : (1 << 15));	//status
	
	//@@defer until not in the NVIC stack any longer, then call user callbacks
	InternalTaskCallback* callbacks = t._t->_c;
	if(callbacks != 0)
	{
		int numCallbacks = t._t->_flags & 0x3FFF;
		t._t->_flags &= ~0x3FFF;
		for(int i = 0; i < numCallbacks; i++)
			callbacks[i].f(callbacks[i].c, t, success);
		
		delete[] callbacks;
		t._t->_c = 0;
	}
	
	return(true);
}

//remember, on ARM Cortex-M (especially NXP parts), optimize for small RAM and program size because the CPU is
//  comparitively disproportionally, insanely, stupidly fast. (Remember 75MHz Pentiums in the '90s with 2000x the RAM
//  these microcontrollers have?
bool			System::when(Task t, void (*completion)(void* context, Task, bool success), void* context)
{
	if((t._t == 0) || (completion == 0))	//if invalid, fail
		return(false);
	
	if(t._t->_flags > 0x3FFF)	//already resolved, act on promise
	{
		completion(context, t, (t._t->_flags < 0x8000));
		return(true);
	}
	
	//if the callback:context tuple is already present, do nothing
	InternalTaskCallback* callbacks = t._t->_c;
	int numCallbacks = t._t->_flags & 0x3FFF;
	for(int i = 0; i < numCallbacks; i++)
		if((callbacks[i].f == completion) && (callbacks[i].c == context))
			return(true);
	
	//add the callback:context tuple to the task
	InternalTaskCallback* newCallbacks = new InternalTaskCallback[numCallbacks + 1];
	
	for(int i = 0; i < numCallbacks; i++)
		newCallbacks[i] = callbacks[i];
	
	newCallbacks[numCallbacks].f = completion;
	newCallbacks[numCallbacks].c = context;
	
	t._t->_c = newCallbacks;
	t._t->_flags++;	//count the new callback
	
	if(numCallbacks)
		delete[] callbacks;
	
	return(true);
}

void			System::sleep(void) const
{
	//@@deep-sleep and interrupt arming as appropriate
	Sleep();
	//@@deep-sleep and interrupt disarming
}

bool			System::wait(Task t)
{
	if(t._t == 0)	return(false);
	
	InternalTask volatile* task = t._t;
	
	//a mini event-loop!
	unsigned short flags;
	while((flags = task->_flags) < 0x4000)	//while not resolved
		sleep();	//service interrupts and the like
	
	return(flags < 0x8000);
}

static void		System_wakeFromSpan(unsigned int point)
{
	//take the current systick down-counter value
	//subtract that value from the span
	//subtract the span from each queued task
	
	unsigned int span = *SystickReload - point, newSpan = 0;
	for(IOCore::TimerTask* timer = IOCore.timerCurrentTask; timer != 0; timer = (IOCore::TimerTask*)timer->next)
	{
		timer->span = (timer->span > span)? (timer->span - span) : 0;
		if((newSpan == 0) && (timer->span > 0))
			newSpan = timer->span;
	}
	
	*SystickReload = newSpan;
	*SystickValue = 0;
	*SystickControl = newSpan? (SystickControl_Enable | SystickControl_InterruptEnabled) : 0;
}

void	System_onSysTickInterrupt(void);

Task			System_delayTicks(unsigned int ticks)
{
	//insert a task into the queue
	Task task = system.createTask();
	IOCore::TimerTask* timer = new IOCore::TimerTask(ticks);
	timer->task = task;
	
	//needs special deadline-order queueing
	InterruptsDisable();
		
		System_wakeFromSpan(*SystickValue);
		
		IOCore::TimerTask** p = (IOCore::TimerTask**)&IOCore.timerCurrentTask;	//ok, inside critical section
		if(*p != 0)
		{
			while((*p != 0) && ((*p)->span < ticks))
				p = (IOCore::TimerTask**)&((*p)->next);
			timer->next = *p;
		}
		*p = timer;
		
		IOCore.irqSystick = System_onSysTickInterrupt;
		
		System_wakeFromSpan(*SystickValue);
		
	InterruptsEnable();
	
	return(task);
}

Task			System::delayMicro(unsigned int microseconds)
{
	return(System_delayTicks((microseconds << 1) / 5));	//convert to ticks (400kHz tickrate)
}
Task			System::delay(unsigned int milliseconds)
{
	return(System_delayTicks(milliseconds * 400));	//convert to ticks (400kHz tickrate)
}

void	System_onSysTickInterruptStub(void)
{
}

void	System_onSysTickInterrupt(void)
{
	System_wakeFromSpan(0);	//defeat SysTick's auto-reloading by forcing it to zero
	
	//if we're at a deadline, deadline = dequeue the top task(s)
	for(IOCore::TimerTask* timer = IOCore.timerCurrentTask; timer != 0;)
	{
		if(timer->span == 0)
		{
			Task complete = timer->task;
			IOCore::TimerTask* old = timer;
			
			//this works because we can only dequeue timer tasks from the beginning of the list
			IOCore.timerCurrentTask = timer = (IOCore::TimerTask*)timer->next;
			
			delete old;
			
			system.completeTask(complete, true);
		}
		else
			timer = (IOCore::TimerTask*)timer->next;
	}
}

//tickrate is 400kHz
extern "C"
INTERRUPT void		_InternalIRQ_SysTick(void)
{
	if(IOCore.irqSystick)
		IOCore.irqSystick();
}


////////////////////////////////////////////////////////////////
//IO Pins

//Pins must be optimized for these operations in this priority order:

//Output logic level, input logic level
//  requires fast lookup to an IO port num and the pin's bit position
//Setting mode
//  I/O direction r/w should be superfast, requires IO port num and pin bit position
//  requires lookup of an IO configuration address for the pin
//Read analog level, where supported

//therefore, I/O Pin objects should store the port number [0,3], pin number [0,11] and a dense pin ID
//  io port mask:	0xFF000000
//	pin num mask:	0x00FF0000
//	pin id mask:	0x0000FFFF

enum
{
	PIN_P0,	PIN_P1, PIN_P2, PIN_P3, PIN_P4, PIN_P5, PIN_P6,
	
	PIN_DP, PIN_DM,
	
	PIN_RTS, PIN_CTS, PIN_TXD, PIN_RXD,
	
	PIN_SCL, PIN_SDA,
	
	PIN_SCK, PIN_SEL, PIN_MISO, PIN_MOSI,
	
	PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A5, PIN_A7,
	
	PIN_LED,
};


//Pin feature and mode lookup tables
static unsigned int const kPinSupportsGPIO = (0xFFFFFFF & ~(_BIT(PIN_DP) | _BIT(PIN_DM)));
static unsigned int const kPinSupportsADC =	(_BIT(PIN_A0) | _BIT(PIN_A1) | _BIT(PIN_A2) | _BIT(PIN_A3)
	| _BIT(PIN_A5) | _BIT(PIN_A7));
static unsigned int const kPinSupportsI2C =	(_BIT(PIN_SCL) | _BIT(PIN_SDA));
static unsigned int const kPinSupportsSPI =	(_BIT(PIN_SCK) | _BIT(PIN_SEL) | _BIT(PIN_MISO) | _BIT(PIN_MOSI));
static unsigned int const kPinSupportsPWM =	(_BIT(PIN_P1) | _BIT(PIN_P5) | _BIT(PIN_TXD) | _BIT(PIN_RXD) |
	_BIT(PIN_MISO) | _BIT(PIN_MOSI) | _BIT(PIN_A0) | _BIT(PIN_A2) | _BIT(PIN_A3) | _BIT(PIN_A5) | _BIT(PIN_LED));
static unsigned int const kPinSupportsUART =	(_BIT(PIN_RTS) | _BIT(PIN_CTS) | _BIT(PIN_TXD) | _BIT(PIN_RXD));
static unsigned int const kPinSupportsSpecial =	(_BIT(PIN_P0) | _BIT(PIN_P1) | _BIT(PIN_DP) | _BIT(PIN_DM)
	| _BIT(PIN_A5));	//P0 is !reset, P1 is clkout, A5 is wakeup
static unsigned int const kPinFunc1IsGPIO =	(_BIT(PIN_P0) | _BIT(PIN_A0) | _BIT(PIN_A1)
	| _BIT(PIN_A2) | _BIT(PIN_A3));	//otherwise it's func 0
static unsigned int const kPinFunc1IsADC =	(_BIT(PIN_A5) | _BIT(PIN_A7));		//otherwise it's func 2
//I2C is always func 1
//SPI is always func 1 except for gpio0.6, where it's 2
static unsigned int const kPinFunc2IsPWM =	(_BIT(PIN_P1) | _BIT(PIN_MISO) | _BIT(PIN_MOSI) | _BIT(PIN_A5)
	| _BIT(PIN_TXD) | _BIT(PIN_RXD) | _BIT(PIN_LED));	//otherwise it's func 3 except for gpio1.9 where it's 1
//UART is always func 1
//Special is always 1 except for gpio0.0 where it's 0


#define PIN_STATE(pinID, ioPort, ioPinNumber)		((ioPort << 8) | (ioPinNumber))
#define PIN_ID(v)					(v & 0xFF)
#define PIN_IO_PORT(v)				(v >> 24)
#define PIN_IO_PIN_NUM(v)			((v >> 16) & 0xFF)
#define PIN_GPIO_DATA_PORT(port)	REGISTER_ADDRESS(0x50000000 | (port << 16))
#define PIN_GPIO_DIR_PORT(port)		REGISTER_ADDRESS(0x50008000 | (port << 16))
static unsigned short const kIOPinChart[] =
{
	PIN_STATE(PIN_P0, 0, 0),	//P0
	PIN_STATE(PIN_P1, 0, 1),	//P1
	PIN_STATE(PIN_P2, 1, 8),	//P2
	PIN_STATE(PIN_P3, 0, 2),	//P3
	PIN_STATE(PIN_P4, 0, 3),	//P4
	PIN_STATE(PIN_P5, 1, 9),	//P5
	PIN_STATE(PIN_P6, 3, 2),	//P6
	
	PIN_STATE(PIN_DM, 3, 4),	//D-
	PIN_STATE(PIN_DP, 3, 5),	//D+

	PIN_STATE(PIN_RTS, 1, 5),	//RTS
	PIN_STATE(PIN_CTS, 0, 7),	//CTS
	PIN_STATE(PIN_TXD, 1, 7),	//TXD
	PIN_STATE(PIN_RXD, 1, 6),	//RXD

	PIN_STATE(PIN_SDA, 0, 5),	//SDA
	PIN_STATE(PIN_SCL, 0, 4),	//SCL

	PIN_STATE(PIN_SCK, 0, 6),	//SCK
	PIN_STATE(PIN_SEL, 2, 0),	//SEL
	PIN_STATE(PIN_MISO, 0, 8),	//MISO
	PIN_STATE(PIN_MOSI, 0, 9),	//MOSI

	PIN_STATE(PIN_A0, 0, 11),	//A0
	PIN_STATE(PIN_A1, 1, 0),	//A1
	PIN_STATE(PIN_A2, 1, 1),	//A2
	PIN_STATE(PIN_A3, 1, 2),	//A3
	PIN_STATE(PIN_A5, 1, 4),	//A5
	PIN_STATE(PIN_A7, 1, 11),	//A7

	PIN_STATE(PIN_LED, 1, 10),	//led	AD6	GPIO1_10	T16_1/M1
};

#define PIN_IOCONFIG_ADDRESS(pinID)	REGISTER_ADDRESS(0x40044000 + IO_ioConfigForPin[pinID])
static unsigned char const IO_ioConfigForPin[] =
{
	0x0C, 0x10, 0x14, 0x1C, 0x2C, 0x38, 0x3C,	//P0, P1, P2, P3, P4, P5, P6
	0x48, 0x9C,									//D-, D+
	0xA0, 0x50, 0xA8, 0xA4,						//RTS, CTS, TXD, RXD
	0x34, 0x30,									//SDA, SCL
	0x4C, 0x08, 0x60, 0x64,						//SCK, SEL, MISO, MOSI
	0x74, 0x78, 0x7C, 0x80, 0x94, 0x98,			//A0, A1, A2, A3, A5, A7
	0x6C,										//LED
};

				IO::IO(void):
					p0(true), p1(true), p2(true), p3(true), p4(true), p5(true), p6(true),
					dminus(true), dplus(true),
					rts(true), cts(true), txd(true), rxd(true),
					sda(true), scl(true),
					sck(true), sel(true), miso(true), mosi(true),
					a0(true), a1(true), a2(true), a3(true), a5(true), a7(true),
					led(true)
{
	*IOConfigSCKLocation = IOConfigSCKLocation_PIO0_6;	//put SCK0 on pin pio0.6, labeled 'SCK' on Galago
	
	//@@find a way to defer this via refcount with debouncing to avoid unnecessary power transitions and warm-up
	*PowerDownControl &= ~PowerDownControl_ADC;	//power on the ADC
	
	//set ADC divider to yield a clock rate under 4.5MHz (12MHz / 4.5MHz)
	*ADCControl = (3 << 8) | ADCControl_10BitSample_11Clocks;
	
	Pin* p = &p0;
	for(int i = PIN_P0; i < (PIN_LED + 1); i++)
	{
		p->v = ((((unsigned int)kIOPinChart[i]) << 16) | i);
		p->setMode(Pin::Default);
		p++;
	}
	
	led = 1;	//!LED deasserted (unlit) initially
}

int				IO::Pin::read(void) const
{
	return(!!PIN_GPIO_DATA_PORT(PIN_IO_PORT(v))[1 << PIN_IO_PIN_NUM(v)]);
}
void			IO::Pin::write(int value)
{
	PIN_GPIO_DATA_PORT(PIN_IO_PORT(v))[1 << PIN_IO_PIN_NUM(v)] = (value ? (~0) : 0);
}

unsigned int	IO::Pin::readAnalog(void) const
{
	*ClockControl |= ClockControl_ADC;
	
	int adcChannel;
	switch(PIN_ID(v))
	{
	case PIN_A0:	adcChannel = 0;	break;
	case PIN_A1:	adcChannel = 1;	break;
	case PIN_A2:	adcChannel = 2;	break;
	case PIN_A3:	adcChannel = 3;	break;
	case PIN_A5:	adcChannel = 5;	break;
	case PIN_A7:	adcChannel = 7;	break;
	default:
		return(0);	//no analog on this pin
	}
	
	(void)*ADCData;
	*ADCControl = (*ADCControl & ~ADCControl_ChannelSelectBitmask) | (1 << adcChannel) | ADCControl_StartNow;
	unsigned int sample;
	while(!((sample = *ADCData) & ADCData_Done));	//whoa, a spinwait! Chosen because the typ. time is 2.5us
	
	return(sample & 0xFFFF);
}

unsigned int	IO::Pin::analogValue(void) const
{
	unsigned int sample;
	switch(PIN_ID(v))
	{
	case PIN_A0:	sample = *ADC0Data;	break;
	case PIN_A1:	sample = *ADC1Data;	break;
	case PIN_A2:	sample = *ADC2Data;	break;
	case PIN_A3:	sample = *ADC3Data;	break;
	case PIN_A5:	sample = *ADC5Data;	break;
	case PIN_A7:	sample = *ADC7Data;	break;
	default:		sample = 0;			break;
	}
	return(sample & 0xFFFF);
}

void			IO::Pin::setMode(Mode mode, Feature feature)
{
	unsigned int id = PIN_ID(v);
	unsigned int mask = (1 << id);
	
	REGISTER pinConfig = PIN_IOCONFIG_ADDRESS(id);
	
	//no longer true:
		//on Galago, the default for any pin is as a GPIO input except the LED, which defaults to an output, initially deasserted
	if(mode == IO::Pin::Default)
		mode = (id == PIN_LED)? IO::Pin::DigitalOutput : IO::Pin::DigitalInput;
	
	unsigned int newValue = *pinConfig & ~0x43F;	//clear the mode, pullup/down state, hysteresis and pseudo-open-drain flags
	
	newValue |= ((feature & 3) << 3);
	if(!(feature & IO::Pin::Sensitive))
		newValue |= (1 << 5);	//sensitive is defined as !hysteresis
	
	if(!(feature & IO::Pin::OpenDrain))
		newValue |= (1 << 10);
	
	switch(mode)
	{
	case IO::Pin::DigitalInput:
	case IO::Pin::DigitalOutput:
		{
			if(kPinSupportsGPIO & mask)
			{
				if(kPinSupportsADC & mask)	//analog-capable pins need to have their digital I/O (re)enabled
					newValue |= 0x80;
				
				if(kPinFunc1IsGPIO & mask)	*pinConfig = newValue | 1;	//for some pins, GPIO is mode 1
				else						*pinConfig = newValue;		//else mode is 0
				
				unsigned int bit = (1 << PIN_IO_PIN_NUM(v));
				REGISTER gpioDirPort = PIN_GPIO_DIR_PORT(PIN_IO_PORT(v));
				
				if(mode == IO::Pin::DigitalOutput)	*gpioDirPort |= bit;
				else								*gpioDirPort &= ~bit;
			}
		}
		break;
	case IO::Pin::AnalogInput:
		if(kPinSupportsADC & mask)
		{
			*pinConfig = (newValue & ~0x80) | ((kPinFunc1IsADC & mask)? 1 : 2);	//the additional 0x80 disables digital I/O too
		}
		break;
	case IO::Pin::SPI:
		if(kPinSupportsSPI & mask)
		{
			*pinConfig = newValue | ((id == PIN_SCK)? 2 : 1);
		}
		break;
	case IO::Pin::I2C:
		if(kPinSupportsI2C & mask)
		{
			*pinConfig = newValue | 1;
		}
		break;
	case IO::Pin::UART:
		if(kPinSupportsUART & mask)
		{
			*pinConfig = newValue | 1;
		}
		break;
	case IO::Pin::PWM:
		if(kPinSupportsPWM & mask)
		{
			if(id == PIN_P5)	*pinConfig = newValue | 1;
			else				*pinConfig = newValue | ((kPinFunc2IsPWM & mask)? 2 : 3);
		}
		break;

	case IO::Pin::Reset:
		if(id == PIN_P0)	//only gpio0.0 has reset ability
			//it so happens mode 0 is reset on gpio0.0, so leave it
			*pinConfig = newValue;
		break;
	case IO::Pin::ClockOutput:
		if(id == PIN_P1)
			*pinConfig = newValue | 1;
		break;
	case IO::Pin::Manual:
		//do nothing because this implementation is stateless anyway
		break;
	}
}

////////////////////////////////////////////////////////////////
// Hardware SPI, exposed on the SCK, MISO and MOSI pins

void	IO_onSPIInterrupt(void);

void			IO::SPI::start(int bitRate, Mode mode, Role role)
{
	*PeripheralnReset &= ~PeripheralnReset_SPI0;	//assert reset
	
	if(bitRate > 0)
	{
		*ClockControl |= ClockControl_SPI0;	//enable SPI0 clock

		*SPI0ClockDivider = 1;
		
		*PeripheralnReset |= PeripheralnReset_SPI0;	//deassert reset
		
		//This finds three prescalers, A, B and C such that (Fcpu / (A + 1) / B / C) = bitRate, where C is an even number 2 to 254.
		// so as to avoid factoring, we cheat here by extracting an 8-bit mantissa (A) and computing 2^exponent, expressed in B and C
		
		unsigned int divisor = (system.getCoreFrequency() / bitRate) >> 1;
		unsigned int magnitude = 32 - __builtin_clz(divisor);
		unsigned int scale = 0, prescale = 2;
		
		if(magnitude > 8)
		{
			if(magnitude > 16)
			{
				prescale = 1 << (magnitude - 15);
				magnitude = 16;
			}
			divisor >>= (magnitude - 8); //set mantissa
			
			scale = ((1 << (magnitude - 8)) - 1);
		}
		else
			scale = 0;
		
		*SPI0Control0 = (scale << 8) | mode;
		*SPI0Control1 = SPI0Control1_Enable;
		*SPI0ClockPrescaler = prescale;
		*SPI0ClockDivider = divisor - 1;
		
		io.sck.setMode(IO::Pin::SPI);
		io.miso.setMode(IO::Pin::SPI);
		io.mosi.setMode(IO::Pin::SPI);
		
		IOCore.irqSPI = IO_onSPIInterrupt;
		
		*InterruptEnableSet1 = Interrupt1_SPI0;
	}
	else
	{
		*ClockControl &= ~ClockControl_SPI0;	//disable SPI0 clock
		
		*InterruptEnableClear1 = Interrupt1_SPI0;
		*SPI0InterruptClear = 0x0F;	//disable interrupts
		
		//set pin modes back
		io.sck.setMode(IO::Pin::Default);
		io.miso.setMode(IO::Pin::Default);
		io.mosi.setMode(IO::Pin::Default);
		
		InterruptsDisable();
			
			IOCore.flushQueue((IOCore::TaskQueueItem**)&IOCore.spiCurrentTask);
			
		InterruptsEnable();
	}
}


void			IO_SPI_queueTask(IOCore::SPITask* spiTask)
{
	InterruptsDisable();
		IOCore.queueItem((IOCore::TaskQueueItem**)&IOCore.spiCurrentTask, spiTask);
		*SPI0InterruptEnable |= SPI0Interrupt_TransmitFIFOHalfEmpty;
	InterruptsEnable();
}

Task			IO::SPI::write(byte b, int length, Buffer bytesReadBack)
{
	Task task = system.createTask();
	IOCore::SPITask* newTask = new(length) IOCore::SPITask(length, bytesReadBack);
	newTask->task = task;

	for(int i = 0; i < length; i++)
		newTask->data[i] = b;
	
	IO_SPI_queueTask(newTask);
	return(task);
}

Task			IO::SPI::write(unsigned short h, int length, Buffer bytesReadBack)
{
	Task task = system.createTask();
	IOCore::SPITask* newTask = new(length * 2) IOCore::SPITask(length, bytesReadBack, true);
	newTask->task = task;
	
	for(int i = 0; i < length; i++)
		newTask->data16[i] = h;
	
	IO_SPI_queueTask(newTask);
	return(task);
}

Task			IO::SPI::write(byte const* s, int length, Buffer bytesReadBack)
{
	if(length < 0)
		length = stringZeroLength(s);
	
	Task task = system.createTask();
	IOCore::SPITask* newTask = new(length) IOCore::SPITask(s, length, bytesReadBack);
	newTask->task = task;
	
	IO_SPI_queueTask(newTask);
	return(task);
}
Task			IO::SPI::write(unsigned short const* s, int length, Buffer bytesReadBack)
{
	Task task = system.createTask();
	IOCore::SPITask* newTask = new(length * 2) IOCore::SPITask(s, length, bytesReadBack);
	newTask->task = task;
	
	IO_SPI_queueTask(newTask);
	return(task);
}

void	IO_onSPIInterrupt(void)
{
	IOCore::SPITask* currentTask;
	
	while((currentTask = IOCore.spiCurrentTask) != 0)
	{
		while((currentTask->readIdx < currentTask->writeIdx) && (*SPI0Status & SPI0Status_ReceiveFIFONotEmpty))
		{
			if(currentTask->is16Bit)
				currentTask->data16[currentTask->readIdx++] = *SPI0Data;
			else
				currentTask->data[currentTask->readIdx++] = (byte)*SPI0Data;
		}
		
		while((currentTask->writeIdx < currentTask->len) && (*SPI0Status & SPI0Status_TransmitFIFONotFull))
		{
			if(currentTask->is16Bit)
				*SPI0Data = currentTask->data16[currentTask->writeIdx++];
			else
				*SPI0Data = currentTask->data[currentTask->writeIdx++];
		}
		
		if(currentTask->readIdx == currentTask->len)
		{
			if(currentTask->bytesReadBack)
				memcpy(currentTask->bytesReadBack.bytes(), currentTask->data, (currentTask->is16Bit)? (currentTask->len * 2) : currentTask->len);
			
			IOCore.spiCurrentTask = (IOCore::SPITask*)currentTask->next;
			system.completeTask(currentTask->task);
			delete currentTask;
			
			continue;
		}
		return;
	}
	
	//stop being notified on FIFO ready
	*SPI0InterruptEnable &= ~SPI0Interrupt_TransmitFIFOHalfEmpty;
}

INTERRUPT void		IRQ_SPI0(void)
{
	IOCore.irqSPI();
}

////////////////////////////////////////////////////////////////
// Hardware UART, exposed on the TXD and RXD pins

void	IO_onUARTInterrupt(void);

void		IO::UART::start(int baudRate, Mode mode)
{
	if(baudRate > 0)
	{
		int q = system.getCoreFrequency() / (16 * baudRate);
		int n = 0;
		int d = 1;
		
		//@@ It remains a point of debate whether peripherals should change pin state...
		io.txd.setMode(IO::Pin::UART);
		io.rxd.setMode(IO::Pin::UART);
		
		if(IOCore.uartReceiveBuffer == 0)
			IOCore.uartReceiveBuffer = new(32) CircularBuffer(32);	//make parametric?
		
		IO::UART::startWithExplicitRatio(q, n, d, mode);
	}
	else	//else shut down the UART
	{
		*UARTInterrupts = 0;
		*InterruptEnableClear1 = Interrupt1_UART;	//disable UART interrupt in the interrupt controller
		
		*UARTFIFOControl = (UARTFIFOControl_RxReset | UARTFIFOControl_TxReset);	//disable and reset
		
		*ClockControl &= ~ClockControl_UART;
		
		//@@ It remains a point of debate whether peripherals should change pin state...
		io.txd.setMode(IO::Pin::Default);
		io.rxd.setMode(IO::Pin::Default);
		
		InterruptsDisable();
			
			delete IOCore.uartReceiveBuffer;
			IOCore.uartReceiveBuffer = 0;
			system.completeTask(IOCore.uartRecvTask, false);
			
			//empty the queue
			IOCore.flushQueue((IOCore::TaskQueueItem**)&IOCore.uartCurrentWriteTask);
			
		InterruptsEnable();
	}
}
void		IO::UART::startWithExplicitRatio(int divider, int fracN, int fracD, Mode mode)
{
	*ClockControl |= ClockControl_UART;
	*UARTClockDivider = 1;
	
	*UARTLineControl = (mode & 0xFF) | UARTLineControl_DivisorLatch;	//apply mode and enter DLAB state
	
	*UARTDivisorLow = divider & 0xFF;		//write divisor bytes
	*UARTDivisorHigh = (divider >> 8) & 0xFF;
	
	*UARTLineControl = (mode & 0xFF);	//exit DLAB state
	*UARTModemControl = (mode >> 8) & 0xC7;
	
	*UARTFIFOControl |= (UARTFIFOControl_Enable | UARTFIFOControl_RxReset | UARTFIFOControl_TxReset | UARTFIFOControl_RxInterrupt1Char);
	
	(void)*UARTLineStatus;	//clear status
	
	IOCore.irqUART = IO_onUARTInterrupt;
	
	//set up UART interrupt
	*UARTInterrupts = (UARTInterrupts_ReceivedData | UARTInterrupts_TxBufferEmpty | UARTInterrupts_RxLineStatus);	//enable conditions for the UART to interrupt
	*InterruptEnableSet1 = Interrupt1_UART;	//enable UART interrupts in the interrupt controller
}

int			IO::UART::bytesAvailable(void) const
{
	return((IOCore.uartReceiveBuffer != 0)? IOCore.uartReceiveBuffer->bytesUsed() : 0);
}

//return the current UART receive task, creating one if none exists
Task		IO::UART::bytesReceived(void)
{
	if(IOCore.uartRecvTask == Task())
		IOCore.uartRecvTask = system.createTask();
	return(IOCore.uartRecvTask);
}

void IO_UART_startTransmission(void)
{
	//do we have bytes to send?
	IOCore::WriteTask* writeTask;
	while((writeTask = IOCore.uartCurrentWriteTask) != 0)
	{
		//push chars only while there's room.
		int count = 16;	//stupid '550 uart
		while(		(writeTask->idx < writeTask->len)	//while there are chars to send
					&& count--	//and the FIFO isn't full
				)
			*UARTData = writeTask->data[writeTask->idx++];	//push chars into the FIFO
		
		if(writeTask->idx == writeTask->len)	//if finished
		{
			IOCore.uartCurrentWriteTask = (IOCore::WriteTask*)writeTask->next;
			system.completeTask(writeTask->task);
			delete writeTask;
			
			if(IOCore.uartCurrentWriteTask == 0)
				break;
		}
		else
			return;		//come back around when there are FIFO bytes free
	}
	*UARTInterrupts &= ~UARTInterrupts_TxBufferEmpty;	//stop being notified on TX ready
}

void	IO_onUARTInterrupt(void)
{
	bool received = false;
	
	unsigned int iid = 0;
	
	//the UARTInterruptID_TxBufferEmpty ("THRE" in the datasheet) reason doesn't fire until alfter bytes have flowed through the FIFO
	//  so prime the pump here.
	//if((*UARTLineStatus & (UARTLineStatus_TxHoldingRegisterEmpty | UARTLineStatus_TransmitterEmpty))
	//	== (UARTLineStatus_TxHoldingRegisterEmpty | UARTLineStatus_TransmitterEmpty))
	//	goto initialTransmit;	//will loop through for legit iid reasons after
	
	while(((iid = *UARTInterruptID) & UARTInterruptID_InterruptPending) == 0)
	{
		switch(iid & UARTInterruptID_ReasonMask)
		{
		case UARTInterruptID_ReceiveException:
			*UARTScratch = *UARTLineStatus;
			break;
			
		case UARTInterruptID_DataAvailable:
		case UARTInterruptID_ReceiveTimeout:
			//receive any available bytes
			while(*UARTLineStatus & UARTLineStatus_ReceiverDataReady)
			{
				if(!IOCore.uartReceiveBuffer->write(*UARTData))
					break;
				
				received = true;
			}
			break;
				
		case UARTInterruptID_TxBufferEmpty:
			IO_UART_startTransmission();
			break;
			
		case UARTInterruptID_Modem:
			//don't care
			break;
		}
	}
	
	if(received && (IOCore.uartRecvTask != Task()))
	{
		Task recvTask = IOCore.uartRecvTask;
		IOCore.uartRecvTask = Task();
		system.completeTask(recvTask);
	}
}

INTERRUPT void		IRQ_UART(void)
{
	IOCore.irqUART();
}

int		IO::UART::read(byte* s, int length)
{
	//read bytes from the RX buffer.  Synchronous and nonblocking
	if(IOCore.uartReceiveBuffer == 0)
		return(0);
	
	return(IOCore.uartReceiveBuffer->read(s, length));
}

Task		IO::UART::write(unsigned int w, IO::UART::Format format)
{
	char buffer[11];
	int length = 0;
	if(format == Character)
	{
		buffer[0] = (char)w;
		length = 1;
	}
	else
	{
		length = NumberFormatter::format(	buffer, w,
											((int)format) & ~1,
											(((int)format) & 1)? NumberFormatter::DecimalSigned : NumberFormatter::DecimalUnsigned
										);
	}
	return(write((byte const*)buffer, length));
}

Task		IO::UART::write(byte const* s, int length)
{
	if(length < 0)
		length = stringZeroLength(s);
	
	Task task = system.createTask();
	IOCore::WriteTask* writeTask = new(length) IOCore::WriteTask(s, length);
	writeTask->task = task;
	
	InterruptsDisable();
		IOCore.queueItem((IOCore::TaskQueueItem**)&IOCore.uartCurrentWriteTask, writeTask);
		*UARTInterrupts |= UARTInterrupts_TxBufferEmpty;	//notify when we can send bytes
		
		if((*UARTLineStatus & (UARTLineStatus_TxHoldingRegisterEmpty | UARTLineStatus_TransmitterEmpty))
			== (UARTLineStatus_TxHoldingRegisterEmpty | UARTLineStatus_TransmitterEmpty))
			IO_UART_startTransmission();
		
	InterruptsEnable();
	
	return(task);
}

////////////////////////////////////////////////////////////////
// Hardware I2C, exposed on the SCL and SDA pins

bool				IO_I2C_repeatedStart(IOCore::I2CTask* currentTask)
{
	return(		(currentTask->next != 0)
				&& ((((IOCore::I2CTask*)(currentTask->next))->data[0] | 1) == (currentTask->data[0] | 1))	//same slave?
			);
}

void				IO_I2C_completePacket(IOCore::I2CTask* currentTask, bool success)
{
	//if read, copy data to bytesReadBack
	if(success && (currentTask->data[0] & 1) && currentTask->bytesReadBack)
	{
		int l = currentTask->len - 1;
		if(currentTask->bytesReadBack.length() < l)
			l = currentTask->bytesReadBack.length();
		memcpy(currentTask->bytesReadBack.bytes(), currentTask->data + 1, l);
	}
	
	IOCore.i2cCurrentTask = (IOCore::I2CTask*)currentTask->next;
	system.completeTask(currentTask->task, success);
	delete currentTask;
	
	if(IOCore.i2cCurrentTask != 0)
		*I2CControlSet = I2CControlSet_StartCondition;
}

void	IO_onI2CInterrupt(void)
{
	int status = *I2CStatus;
	IOCore::I2CTask* currentTask = IOCore.i2cCurrentTask;
	switch(status)
	{
	case 0x08:	//start bit sent
	case 0x10:	//repeated start
		*I2CData = currentTask->data[0];
		*I2CControlClear = I2CControlSet_StartCondition;
		break;
		
	case 0x20:	//write address NACKed, stop
	case 0x48:	//read address NACKed, stop
		*I2CControlSet = I2CControlSet_StopCondition;
		IO_I2C_completePacket(currentTask, false);
		break;
		
	case 0x40:	//read address ACKed, ready to read
		if(currentTask->len > 1)	*I2CControlSet = I2CControlSet_Ack;
		break;
	
	case 0x18:	//write address ACKed, ready to write
	case 0x28:	//byte sent, ACK received
		if(++currentTask->idx < currentTask->len)
		{
			*I2CData = currentTask->data[currentTask->idx];
			break;
		}
		//(intentional flow to the following case)
		
	case 0x58:	//byte received, NACK sent
		if(IO_I2C_repeatedStart(currentTask))	*I2CControlSet = I2CControlSet_StartCondition;
		else									*I2CControlSet = I2CControlSet_StopCondition;
		IO_I2C_completePacket(currentTask, true);
		break;
		
	case 0x30:	//byte sent, NACK received
		*I2CControlClear = I2CControlSet_StopCondition;
		IO_I2C_completePacket(currentTask, false);
		break;
		
	case 0x50:	//byte received, ACK sent
		currentTask->data[++currentTask->idx] = *I2CData;
		
		// +2 accounts for the address byte and the fact that we have to decide the response one byte ahead
		if((currentTask->idx + 2) < currentTask->len)	*I2CControlSet = I2CControlSet_Ack;
		else											*I2CControlClear = I2CControlSet_Ack;
		
		break;
	
	default:
	case 0x00:
	case 0xF8:
		//protocol errors!
		
	case 0x38:	//arbitration loss, abort
		IO_I2C_completePacket(currentTask, false);
		break;
	}
	
	*I2CControlClear = I2CControlSet_Interrupt;
}

INTERRUPT void		IRQ_I2C(void)
{
	IOCore.irqI2C();
}

void	IO::I2C::start(int bitRate, IO::I2C::Role role)
{
	*PeripheralnReset &= ~PeripheralnReset_I2C;	//assert reset
	
	InterruptsDisable();

	//empty the queue in any case
	IOCore.flushQueue((IOCore::TaskQueueItem**)&IOCore.i2cCurrentTask);
	
	if(bitRate > 0)
	{
		*ClockControl |= ClockControl_I2C;
		*PeripheralnReset |= PeripheralnReset_I2C;	//deassert reset
		
		io.scl.setMode(IO::Pin::I2C);
		io.sda.setMode(IO::Pin::I2C);
		
		*I2CControlClear = I2CControlSet_Ack | I2CControlSet_Interrupt
							| I2CControlSet_StartCondition | I2CControlSet_EnableI2C;
		
		unsigned int bitHalfPeriod = (system.getCoreFrequency() / bitRate) >> 1;
		//@@depending on the time-constant of the bus (1 / (pull-up resistance * capacitance)),
		//  the low-time should be smaller and the high-time should be higher
		*I2CClockHighTime = bitHalfPeriod;
		*I2CClockLowTime = bitHalfPeriod;
		
		IOCore.irqI2C = IO_onI2CInterrupt;
		
		//enable interrupt before enabling I2C state machine:
		*InterruptEnableSet1 = Interrupt1_I2C;
		*I2CControlSet = I2CControlSet_EnableI2C;
	}
	else
	{
		io.scl.setMode(IO::Pin::Default);
		io.sda.setMode(IO::Pin::Default);
		
		*I2CControlClear = I2CControlSet_Ack | I2CControlSet_Interrupt
							| I2CControlSet_StartCondition | I2CControlSet_EnableI2C;
		
		//shut down I2C clock
		*ClockControl &= ~ClockControl_I2C;
		*InterruptEnableClear1 = Interrupt1_I2C;
	}
	InterruptsEnable();
}

Task	IO::I2C::write(byte address, Buffer s, IO::I2C::RepeatedStartSetting repeatedStart)
{
	Task task = system.createTask();
	if(!(*PeripheralnReset | PeripheralnReset_I2C))	//if in reset mode, I2C system is inactive and this job should fail.
	{
		system.completeTask(task, false);	//synchronous failure
		return(task);
	}
	
	IOCore::I2CTask* i2cTask = new(1 + s.length()) IOCore::I2CTask(1 + s.length());
	i2cTask->data[0] = address;
	
	//if reading, retain Buffer 's'; else if writing, copy Buffer 's' to the task's data property
	if(address & 1)
		i2cTask->bytesReadBack = s;
	else
		memcpy(i2cTask->data + 1, s.bytes(), s.length());
	
	i2cTask->task = task;
	
	InterruptsDisable();
		bool mustStart = (IOCore.i2cCurrentTask == 0);
		IOCore.queueItem((IOCore::TaskQueueItem**)&IOCore.i2cCurrentTask, i2cTask);
		
		if(mustStart)
			*I2CControlSet = I2CControlSet_StartCondition;
		
	InterruptsEnable();
	
	return(task);
}
