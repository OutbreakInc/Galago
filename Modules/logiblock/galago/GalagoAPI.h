#ifndef __GALAGO_H__
#define __GALAGO_H__

//#include <stddef.h>

typedef unsigned int	size_t;
typedef signed int		ssize_t;
typedef unsigned char	byte;

namespace Galago {

class Task;
struct InternalTaskCallback;
struct InternalTask
{
private:
	friend class Task;
	friend class System;
	unsigned short			_flags;
	unsigned short			_rc;
	InternalTaskCallback*	_c;
	
	void					destroy(void);
};

class Buffer;
struct InternalBuffer
{
private:
	friend class Buffer;
	
					InternalBuffer(size_t l): length(l), _rc(0) {}
	size_t			length;
	unsigned short	_rc;
	byte			data[1];
};

class Task
{
public:
	friend class System;
	
	inline 					Task(void): _t(0)	{}
	inline 					Task(Task const& t): _t(t._t)	{refer(_t);}
	inline 					~Task(void)				{release(_t); _t = 0;}
	
	Task&					operator =(Task const& t);
	
	inline bool operator	==(Task const& t) const	{return(t._t == _t);}
	inline bool operator	!=(Task const& t) const	{return(t._t != _t);}
	
	Task operator			+(Task const& r) const;
	
private:
	inline 					Task(InternalTask* t): _t(t)	{refer(_t);}
	inline static void		refer(InternalTask* t)	{if(t)	t->_rc++;}
	static void				release(InternalTask* t);
	InternalTask*			_t;
};

class CircularBuffer
{
public:
			CircularBuffer(int size);
			~CircularBuffer(void);

	bool	write(byte b);
	int		write(byte const* b, int length);
	bool	read(byte* b);
	int		read(byte* b, int length);
	
	int		bytesUsed(void) const;
	int		bytesFree(void) const;
	
private:
	byte*	_start;
	byte*	_end;
	byte*	_head;
	byte*	_tail;
};


class Buffer
{
public:
	inline					Buffer(void): _b(0) {}
	inline					Buffer(Buffer const& b): _b(b._b)	{refer(_b);}
	Buffer&					operator =(Buffer const& b);
	inline 					~Buffer(void)				{release(_b); _b = 0;}
	
	inline size_t			length() const	{return(_b? _b->length : 0);}
	inline byte*			bytes()			{return(_b? _b->data : 0);}
	inline byte const*		bytes() const	{return(_b? _b->data : 0);}
	
	static Buffer			New(char const* cStr);
	static Buffer			New(size_t length);
	static Buffer			New(void const* b, size_t length);
	
	Buffer					operator +(Buffer const& b) const;
	Buffer&					operator +=(Buffer const& b);
	
	inline bool				operator ==(Buffer const& b) const		{return((b._b == _b) || (b._b && equals(b._b->data, b._b->length)));}
	bool					operator ==(char const* cStr) const;
	inline bool				operator !=(Buffer const& b) const		{return((b._b != _b) || (!b._b) || !equals(b._b->data, b._b->length));}
	inline bool				operator !=(char const* cStr) const		{return(!operator == (cStr));}
	inline					operator bool(void) const				{return(_b != 0);}
	
	unsigned int			parseUint(int base = 10);
	signed int				parseInt(int base = 10);
	
	bool					startsWith(byte const* str, size_t length) const;
	bool					startsWith(char const* cStr) const;
	bool					equals(byte const* str, size_t length) const;
	
	inline byte				operator[](size_t offset) const		{return((_b && (offset < _b->length))? _b->data[offset] : 0);}
	
	Buffer					slice(size_t start, size_t end);
	ssize_t					indexOf(byte b, size_t offset = 0);
	ssize_t					indexOf(Buffer b, size_t offset = 0);
	
private:
	inline					Buffer(InternalBuffer* b): _b(b) {refer(b);}
	inline static void		refer(InternalBuffer* b)	{if(b)	b->_rc++;}
	static void				release(InternalBuffer* b);
	InternalBuffer*			_b;
};

class IO
{
public:

	class Pin
	{
		friend class IO;

	public:
		typedef enum
		{
			DigitalInput,
			DigitalOutput,
			AnalogInput,
			
			Reset,
			SPI,
			I2C,
			UART,
			PWM,
			USB,
			
			ClockOutput,
			Wakeup,
			
			Manual = 0xFE,
			Default,
		} Mode;
		
		//not all parts feature these pin modes
		typedef enum
		{
			Normal		= 0,
			PullDown	= 1,
			PullUp		= 2,
			
			Sensitive	= 4,	//Sensistive mode implies hysteresis/Schmitt-triggers are disabled for the pin

			//OpenDrain means a logic high results in a high-impedance (un-driven) pin
			//  and a logic low drives the pin low
			OpenDrain	= 8,
		} FeatureSetting;
		typedef int Feature;
		
		inline			Pin(void): v(~0)					{}
		
		inline	Pin&	bind(Pin const& p)		{v = p.v; return(*this);}
		
		inline	Pin&	operator =(bool value)	{write(value? 1 : 0); return(*this);}
		inline	Pin&	operator =(int value)	{write(value); return(*this);}
		inline	Pin&	operator =(Pin& p)		{write(p.read()); return(*this);}
		inline			operator bool(void)		{return((bool)read());}
		
		int				read(void) const;
		unsigned int	readAnalog(void) const;
		unsigned int	analogValue(void) const;
		void			write(int value);
		
		inline	void	setOutput(Feature feature = Normal)		{setMode(DigitalOutput, feature);}
		inline	void	setInput(Feature feature = PullUp)		{setMode(DigitalInput, feature);}
		inline	void	setAnalog(Feature feature = Sensitive)	{setMode(AnalogInput, feature);}
		inline	void	setPWM(Feature feature = Normal)		{setMode(PWM, feature);}
		
		void			setMode(Mode mode, Feature feature = Normal);
		
	private:
		inline			Pin(Pin const& p): v(p.v)			{}
		explicit inline	Pin(bool)							{}
		
		unsigned int	v;
	};

	class SPI
	{
	public:
		typedef enum
		{
			Master,
			Slave,
		} Role;
		
		enum
		{
			CharsAre4Bit	=	(0x03),
			CharsAre5Bit	=	(0x04),
			CharsAre6Bit	=	(0x05),
			CharsAre7Bit	=	(0x06),
			CharsAre8Bit	=	(0x07),
			CharsAre9Bit	=	(0x08),
			CharsAre10Bit	=	(0x09),
			CharsAre11Bit	=	(0x0A),
			CharsAre12Bit	=	(0x0B),
			CharsAre13Bit	=	(0x0C),
			CharsAre14Bit	=	(0x0D),
			CharsAre15Bit	=	(0x0E),
			CharsAre16Bit	=	(0x0F),
			
			Mode0			= (0x00 << 6),	//SCK idles low, data changed on SCK's falling edge, read on rising edge.
			Mode1			= (0x02 << 6),	//SCK idles low, data changed on SCK's rising edge, read on falling edge.
			Mode2			= (0x01 << 6),	//SCK idles high, data changed on SCK's falling edge, read on rising edge.
			Mode3			= (0x03 << 6),	//SCK idles high, data changed on SCK's rising edge, read on falling edge.
			
			Default			= (CharsAre8Bit | Mode0),
		};
		typedef int Mode;
		
		void			start(int bitRate = 2000000UL, Mode mode = Default, Role role = Master);
		inline void		stop(void)	{start(0);}
		
		bool			bytesAvailable(void) const;
		
		inline Task		read(int length, Buffer bytesReadBack = Buffer(), byte writeChar = 0)	{return(write(writeChar, length, bytesReadBack));}
		inline Task		read(int length, Buffer bytesReadBack, unsigned short writeChar)		{return(write(writeChar, length, bytesReadBack));}
		
		inline Task		readAndWrite(char const* s, int length, Buffer bytesReadBack = Buffer())		{return(write((byte const*)s, length, bytesReadBack));}
		inline Task		readAndWrite(byte const* s, int length, Buffer bytesReadBack = Buffer())		{return(write(s, length, bytesReadBack));}
		inline Task		readAndWrite(unsigned short const* s, int length, Buffer bytesReadBack = Buffer()) {return(write(s, length, bytesReadBack));}
		
		inline Task		write(char c, int length = 1, Buffer bytesReadBack = Buffer())				{return(write((byte)c, length, bytesReadBack));}
		Task			write(byte b, int length = 1, Buffer bytesReadBack = Buffer());
		inline Task		write(short h, int length = 1, Buffer bytesReadBack = Buffer())				{return(write((unsigned short)h, length, bytesReadBack));}
		Task			write(unsigned short h, int length = 1, Buffer bytesReadBack = Buffer());
		
		inline Task		write(char const* s, int length = -1, Buffer bytesReadBack = Buffer())		{return(write((byte const*)s, length, bytesReadBack));}
		Task			write(byte const* s, int length, Buffer bytesReadBack = Buffer());
		Task			write(unsigned short const* s, int length, Buffer bytesReadBack = Buffer());
		
		Task			setSelect(bool select);
	};

	class I2C
	{
	public:
		typedef enum
		{
			Master,
			Slave,
		} Role;
		
		typedef enum
		{
			No,
			RepeatedStart,
		} RepeatedStartSetting;
		
		void			start(int bitRate = 100000UL, Role role = Master);
		inline void		stop(void)	{start(0);}
		
		Task			write(byte address, Buffer s, RepeatedStartSetting repeatedStart = No);
		inline Task		read(byte address, Buffer s, RepeatedStartSetting repeatedStart = No)
							{return(write(address | 1, s, repeatedStart));}
		
		void			end(void);
	};

	class UART
	{
	public:
		enum
		{
			CharsAre5Bit			=	(0x00),
			CharsAre6Bit			=	(0x01),
			CharsAre7Bit			=	(0x02),
			CharsAre8Bit			=	(0x03),
			
			OneStopBit				=	(0x00),
			TwoStopBits				=	(0x04),
			
			NoParity				=	(0x00),
			UseOddParity			=	(0x08 | (0x00 << 4)),
			UseEvenParity			=	(0x08 | (0x01 << 4)),
			UseConstant1Parity		=	(0x08 | (0x02 << 4)),
			UseConstant0Parity		=	(0x08 | (0x03 << 4)),
			
			Default  				=	(CharsAre8Bit | NoParity | OneStopBit)
		};
		typedef int		Mode;
		
		void			start(int baudRate = 9600, Mode mode = Default);
		void			startWithExplicitRatio(int divider, int fracN, int fracD, Mode mode);
		inline void		stop(void)	{start(0);}
		
		int				bytesAvailable(void) const;
		
		//returns a task which is completed when the next byte(s) are received.
		//  To listen for more bytes, call this again and wait/call-back on the new task it returns.
		Task			bytesReceived(void);
		
		//these functions are synchronous and nonblocking, returning only what's in the buffer (and not waiting for data)
		inline int		read(char* s, int length)	{return(read((byte*)s, length));}
		int				read(byte* s, int length);

		typedef enum
		{
			Character,
			UnsignedByte,
			SignedByte,
			UnsignedInteger16,
			SignedInteger16,
			UnsignedInteger32,
			SignedInteger32
		} Format;
		
		Task			write(unsigned int w, Format format = UnsignedInteger32);
		inline Task		write(byte b, Format format = UnsignedByte)					{return(write((unsigned int)b, format));}
		inline Task		write(char c, Format format = Character)					{return(write((unsigned int)c, format));}
		inline Task		write(unsigned short h, Format format = UnsignedInteger16)	{return(write((unsigned int)h, format));}
		inline Task		write(short h, Format format = SignedInteger16)				{return(write((unsigned int)h, format));}
		inline Task		write(int w, Format format = SignedInteger32)				{return(write((unsigned int)w, format));}

		inline Task		write(char const* s, int length = -1)	{return(write((byte const*)s, length));}
		Task			write(byte const* s, int length = -1);
	};

	Pin				p0;
	Pin				p1;
	Pin				p2;
	Pin				p3;
	Pin				p4;
	Pin				p5;
	Pin				p6;
	
	Pin				dminus;
	Pin				dplus;
	
	Pin				rts;
	Pin				cts;
	Pin				txd;
	Pin				rxd;
	
	Pin				sda;
	Pin				scl;
	
	Pin				sck;
	Pin				sel;
	Pin				miso;
	Pin				mosi;
	
	Pin				a0;
	Pin				a1;
	Pin				a2;
	Pin				a3;
	Pin				a5;
	Pin				a7;
	
	Pin				led;
	
	SPI				spi;
	
	I2C				i2c;
	
	UART			serial;

					IO(void);
private:
	unsigned int	v;
};

class System
{
public:
	static void*	alloc(size_t size);
	static void		free(void* allocation);
	
	unsigned int	getMainClockFrequency(void) const;
	unsigned int	getCoreFrequency(void) const;
	void			setCoreFrequency(unsigned int desiredFrequency);
	unsigned int	getClockOutputFrequency(void) const;
	void			setClockOutputFrequency(unsigned int desiredFrequency);
	
	void			sleep(void) const;
	
	Task			createTask(void);
	
	bool			completeTask(Task t, bool success = true);
	
	//These methods use the minimum power by sleeping when possible:

	//asynchronously respond when a task is complete.
	bool			when(Task t, void (*completion)(void* context, Task, bool success), void* completionContext = 0);
	
	//synchronously wait for a task to complete. Completion callbacks for other tasks will be called from within this
	//  method, making it not strictly blocking.
	bool			wait(Task t);
	
					System(void);
	
	Task			delay(unsigned int milliseconds);
	Task			delayMicro(unsigned int microseconds);
};

extern IO		io;
extern System	system;

}	//ns Galago

inline void*		operator new(size_t size)	{return(Galago::System::alloc(size));}

inline void*		operator new[](size_t size)	{return(Galago::System::alloc(size));}

inline void*		operator new[](size_t size, unsigned int extra)	{return(Galago::System::alloc(size + extra));}

inline void*		operator new(size_t size, unsigned int extra)	{return(Galago::System::alloc(size + extra));}

inline void			operator delete(void* p)
{
	if(((unsigned int)(size_t)p) & 0x3)	return;	//@@throw
	Galago::System::free((unsigned int*)p);
}

inline void			operator delete[](void* p)
{
	if(((unsigned int)(size_t)p) & 0x3)	return;	//@@throw
	Galago::System::free((unsigned int*)p);
}

#endif //defined __GALAGO_H__