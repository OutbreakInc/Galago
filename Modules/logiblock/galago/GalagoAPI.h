/* -*- tab-width: 4; column-number-mode: 1; fill-column: 80; -*- */
/*                                                                         |
   Logiblock.com  Galago     (c) Outbreak, Inc. 2010-2013                  |
   Sources released under Creative Commons CC-BY-SA-3.0 unless noted       |
   License details at: http://creativecommons.org/licenses/by-sa/3.0/      |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   GalagoAPI.h is the primary programming interface (library) for Galago   |
   and contains both software and hardware device abstractions.  To use:   |
                                                                           |
     #include <GalagoAPI.h>                                                |
                                                                           |
     using namespace Galago;                                               |
                                                                           |
                                                                           |
   ## API Objects                                                          |
                                                                           |
   GalagoAPI uses two main API singletons (objects of one global instance) |
   `system` and `io`.  `system` is in charge of memory, time, clocks and   |
   Tasks.  `io` handles all IO peripherals, like the UART, I2C, SPI,       |
   ADC and GPIO.                                                           |
                                                                           |
   Several other objects, like Tasks and Buffers, are used throughout the  |
   API and are implemented as reference objects, so they may be passed     |
   by value without concern for memory management.  This is particularly   |
   useful for Buffer objects.                                              |
                                                                           |
                                                                           |
   ## Tasks                                                                |
                                                                           |
   A fundamental concept in GalagoAPI is the Task, which is a type of      |
   promise, a value that represents ongoing work until it is completed.    |
   When completed, a task becomes either successful or failed.  Many       |
   asynchonous methods return Tasks, which indicate that they're waiting   |
   for hardware features in the background.  For a full description of     |
   Tasks, see the `system` API below.                                      |
                                                                           |
                                                                           |
==========================================================================*/


#ifndef __GALAGO_H__
#define __GALAGO_H__

typedef unsigned int	size_t;
typedef signed int		ssize_t;
typedef unsigned char	byte;

namespace Galago {

//Internal classes, do not use these directly
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

//end internal classes



/*                                                                         |
   Galago Task object                                                      |
                                                                           |
   Tasks represent asynchronous work that has either not yet completed,    |
   has succeeded or has failed.  Tasks are a type of promise that make     |
   writing asynchronous code simple while being immune to race conditions, |
   deadlocks and other nuisances when used correctly.                      |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Creating and passing Tasks                                           |
                                                                           |
   Tasks are reference objects so you don't need to worry about how they   |
   refer to state.  To create tasks, use the `System::createTask()`        |
   method (see System API documentation) and pass and assign them as       |
   normal value objects:                                                   |
                                                                           |
     Task t;  // null reference                                            |
                                                                           |
     t = system.createTask();                                              |
                                                                           |
     Task t2 = t;                                                          |
                                                                           |
     someMethod(t2);                                                       |
                                                                           |
     t = doAsyncAction(500);                                               |
                                                                           |
   Tasks may be compared with the == and != operators.  This compares      |
   references and, by extension, value:                                    |
                                                                           |
     if(t == Task())  // is t a null Task reference?                       |
                                                                           |
   You may combine Tasks to create a new Task that's only complete when    |
   all its constituent Tasks are complete, or is considered failed when    |
   the first of its constituent Tasks fails:                               |
                                                                           |
     Task t3 = t + t2;                                                     |
                                                                           |
     system.when(t3, &someAction);                                         |
                                                                           |
   Please find the main Task documentation in the System API.              |
                                                                           |
                                                                           |
==========================================================================*/

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

/*                                                                         |
   Galago CircularBuffer object                                            |
                                                                           |
   CircularBuffer is a utility object that implements a simple circular    |
   buffer - that is, a section of memory into which bytes can be written   |
   and read in a first-in-first-out (FIFO) fashion.                        |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Creating and referencing CircularBuffers                             |
                                                                           |
   CircularBuffers are regular objects, so creation and ownershop follows  |
   normal C++ rules:                                                       |
                                                                           |
     CircularBuffer* b = new CircularBuffer(24);                           |
                                                                           |
     CircularBuffer* b2 = b;                                               |
                                                                           |
     delete b2;  // caution, leaves b reference dangling                   |
                                                                           |
   ## Reading and writing bytes                                            |
                                                                           |
   To read and write bytes, use the `.read()` and `.write()` methods.  For |
   example, with single bytes a bool is returned indicating success:       |
                                                                           |
     b.write(1);  // returns true if the CircularBuffer is not full        |
     b.write(2);  //  "                                                    |
     b.write(3);  //  "                                                    |
                                                                           |
     byte a;                                                               |
     b.read(&a);  // reads 1 into a, returns true                          |
     b.read(&a);  // reads 2 into a, returns true                          |
     b.read(&a);  // reads 3 into a, returns true                          |
     b.read(&a);  // b is empty, returns false.  'a' is unchanged          |
                                                                           |
   For multiple bytes, the number of bytes read or written is returned.    |
   If the entire byte string cannot be written, as much of it as possible  |
   is written and that length is returned.  By extension, if a             |
   CircularBuffer is full, 0 is returned.  For reads, if the requested     |
   read length isn't available, as much as possible is read and that       |
   length is returned.  To determine how many bytes are available or       |
   occupied, use the `.bytesFree()` and `.bytesUsed()` methods,            |
   respectively.                                                           |
                                                                           |
                                                                           |
==========================================================================*/

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

/*                                                                         |
   Galago Buffer object                                                    |
                                                                           |
   Buffers are reference-counted blocks of memory that support many high-  |
   level operations common on String objects in other languages.           |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Creating and passing Buffers                                         |
                                                                           |
   To create a buffer, call the static method `Buffer::New()`:             |
                                                                           |
     Buffer b;  // a null reference                                        |
                                                                           |
     Buffer b = Buffer::New("a string");  // null-terminated char string   |
     Buffer b = Buffer::New(24);  // a size                                |
     Buffer b = Buffer::New(data, length);  // arbitrary bytes             |
                                                                           |
   References can be copied - both `b` and `b2` point at the same buffer:  |
                                                                           |
     Buffer b = Buffer::New("example");                                    |
     Buffer b2 = b;                                                        |
                                                                           |
   A Buffer's memory is freed when no references point at it anymore:      |
                                                                           |
     b2 = b = Buffer();  // set b and b2 to null references                |
                                                                           |
   You may treat a Buffer as a bool to see if it has a non-null reference: |
                                                                           |
     if(b)  doSomethingWith(b);                                            |
                                                                           |
                                                                           |
   ## Buffer contents                                                      |
                                                                           |
   Buffers can be compared with == and != operators.  Equivalent           |
   references and equal contents are both considered equal.                |
                                                                           |
     if(b == b2) theyAreEqual();                                           |
                                                                           |
   Contents can be further compared with `.equals()` and `.startsWith()`   |
   methods:                                                                |
                                                                           |
     if(b.equals("hello", 5)) wasHello();                                  |
                                                                           |
     if(b.startsWith("../", 3)) handleThisCase();                          |
                                                                           |
   The contents and length are accessible with the `.bytes()` and the      |
   `.length()` methods respectively:                                       |
                                                                           |
     io.serial.write(b.bytes(), b.length());                               |
                                                                           |
     b.bytes()[4] = 'a';                                                   |
                                                                           |
   Concatentation is possible with + and += operators:                     |
                                                                           |
     b += Buffer::New(" more string");                                     |
                                                                           |
     Buffer b3 = b2 + b;                                                   |
                                                                           |
   A substring can be extracted with `.slice()`:                           |
                                                                           |
     Buffer b4 = b.slice(0, 7);                                            |
                                                                           |
   A single character or a substring can be found in a Buffer with the     |
   `.indexOf()` method:                                                    |
                                                                           |
     ssize_t position = b.indexOf('a');                                    |
                                                                           |
     position = b.indexOf(b3, 6);  // optional start offset passed         |
                                                                           |
   The `.parseInt()` and `.parseUint()` methods attempt to interpret the   |
   Buffer's contents as an integer or unsigned integer, respectively:      |
                                                                           |
     int value = Buffer::New("50").parseInt();  // value would be 50       |
                                                                           |
     value = Buffer::New("a").parseInt();  // value would be 0             |
                                                                           |
                                                                           |
==========================================================================*/

class Buffer
{
public:
	inline					Buffer(void): _b(0) {}
	inline					Buffer(Buffer const& b): _b(b._b)	{refer(_b);}
	Buffer&					operator =(Buffer const& b);
	inline 					~Buffer(void)				{release(_b); _b = 0;}
	
	//access the length and contents of a Buffer
	inline size_t			length() const	{return(_b? _b->length : 0);}
	inline byte*			bytes()			{return(_b? _b->data : 0);}
	inline byte const*		bytes() const	{return(_b? _b->data : 0);}
	
	//create a Buffer from a C string, bytes or empty with a specified length
	static Buffer			New(char const* cStr);
	static Buffer			New(size_t length);
	static Buffer			New(void const* b, size_t length);
	
	//concatenate Buffers
	Buffer					operator +(Buffer const& b) const;
	Buffer&					operator +=(Buffer const& b);
	
	//compare the contents of to Buffers.  Two references to the same memory are of course equal too
	inline bool				operator ==(Buffer const& b) const		{return((b._b == _b) || (b._b && equals(b._b->data, b._b->length)));}
	bool					operator ==(char const* cStr) const;
	inline bool				operator !=(Buffer const& b) const		{return((b._b != _b) || (!b._b) || !equals(b._b->data, b._b->length));}
	inline bool				operator !=(char const* cStr) const		{return(!operator == (cStr));}
	inline					operator bool(void) const				{return(_b != 0);}
	
	//parse the Buffer as a unsigned integer. Failure to parse returns 0
	unsigned int			parseUint(int base = 10);
	signed int				parseInt(int base = 10);
	
	//determine if the Buffer begins with another Buffer, a C string or an array of bytes
	inline bool				startsWith(Buffer const& b) const	{return((b._b != 0) && startsWith(b._b->data, b._b->length));}
	bool					startsWith(byte const* str, size_t length) const;
	bool					startsWith(char const* cStr) const;
	
	//compare the Buffer to an array of bytes
	bool					equals(byte const* str, size_t length) const;
	
	//safely read bytes without risk of out-of-bounds access
	inline byte				operator[](size_t offset) const		{return((_b && (offset < _b->length))? _b->data[offset] : 0);}
	
	//extract a section of the Buffer.  Uses start <= end notation rather than (start, length)
	Buffer					slice(size_t start, size_t end);
	ssize_t					indexOf(byte b, size_t offset = 0);
	ssize_t					indexOf(Buffer b, size_t offset = 0);
	
private:
	inline					Buffer(InternalBuffer* b): _b(b) {refer(b);}
	inline static void		refer(InternalBuffer* b)	{if(b)	b->_rc++;}
	static void				release(InternalBuffer* b);
	InternalBuffer*			_b;
};

/*                                                                         |
   Galago IO API: `Galago::io`                                             |
                                                                           |
   IO is responsible for all IO peripherals, like the UART, I2C, SPI, ADC  |
   and GPIO.                                                               |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Pin                                                                  |
                                                                           |
   Central to the concepts in the IO class is the Pin object.  A Pin       |
   represents a single i/o pin and reflects its capabilities.  Pins can be |
   used directly for general purpose IO and the ADC, but also control      |
   which other features, such as I2C, UART and SPI, are mapped to the pin. |
                                                                           |
                                                                           |
   ## SPI (3-wire/4-wire synchronous serial interface)                     |
                                                                           |
   SPI is a three-wire bus with duplex serial lanes and out-of-band device |
   selection.  Unlike I2C, it has no standard data size, clock polarity,   |
   arbitration, acknowledgement or other high-level features.  What it     |
   lacks in features it makes up for in speed, simplicity and flexibility. |
                                                                           |
                                                                           |
   ## I2C bus (Inter-IC bus)                                               |
                                                                           |
   I2C is a two-wire bus that supports multiple masters and slaves with    |
   addressing, collision avoidance, acknowledgement and slave bit delays.  |
   It's widely used for a large range of peripherals including sensors,    |
   memory devices, human interfaces and displays.                          |
                                                                           |
                                                                           |
   ## UART (Asynchronous serial port)                                      |
                                                                           |
   The UART is a very common serial interface traditionally used to        |
   connect computers, modems, dissimilar microcontrollers and industrial   |
   equipment, communications devices and countless other applications.     |
   Unlike SPI and I2C it's asynchronous and therefore relies on accurate   |
   absolute time references on both the sending and receiving ends.        |
                                                                           |
                                                                           |
==========================================================================*/

class IO
{
public:
/*
                                                                           |
   ## Pin                                                                  |
                                                                           |
   Central to the concepts in the IO class is the Pin object.  A Pin       |
   represents a single i/o pin and reflects its capabilities.  Logically,  |
   Pins behave like constant references to real pins, so you can use `=`   |
   to set the state of a Pin rather than change its reference.  To set     |
   the reference, use `Pin.bind()`:                                        |
                                                                           |
     Pin doorSolenoid;                                                     |
     doorSolenoid.bind(io.p5);  // bind to io.p5                           |
                                                                           |
   ### Pin modes                                                           |
                                                                           |
   Pins have a `setMode()` method, which allows configuration of the mode  |
   and features of the pin, plus shorthand methods `setOutput()`,          |
   `setInput()`, `setAnalog()` and `setPWM()`.  The supported modes vary   |
   per pin, so check the hardware documentation for what each pin can do.  |
   For the full list of supported modes, see the inline documentation for  |
   the `IO::Pin::Mode` enum below.                                         |
                                                                           |
   ### Pins as GPIO                                                        |
                                                                           |
   To use Pins as general-purpose i/o (GPIO), call `.setOutput()`,         |
   `.setInput()` or `.setMode()` with the appropriate options and          |
   `.read()`, `.write()` to read or write a digital value (respectively.)  |
   For example:                                                            |
                                                                           |
     io.p2.write(1);  // set pin p2 to digital high                        |
     io.p2 = 1;       // equivalent                                        |
     io.p2 = true;    // equivalent                                        |
     io.p2 = io.p4;   // set pin p2 to the value read from p4              |
                                                                           |
     int value = io.p2.read();  // read the digital value from pin p2      |
                                                                           |
     bool buttonPressed = io.p2;  // digital values are bools too          |
                                                                           |
     if(io.p2)  // you can read pins and use them as bool expressions      |
     {                                                                     |
       ...                                                                 |
     }                                                                     |
                                                                           |
   Like other microcontroller platforms, you may read a gpio value while   |
   the Pin is set for output mode, and if you write a value while it's an  |
   input, no level will be driven on the pin.                              |
                                                                           |
   For analog values, ensure the Pin is in analog mode and then read it:   |
                                                                           |
     io.a3.setAnalog();                                                    |
                                                                           |
     unsigned int value = io.readAnalog();                                 |
                                                                           |
   As a convenience, you may access the last analog value with the         |
   `.analogValue()` method.                                                |
                                                                           |
                                                                           |
==========================================================================*/
	
	class Pin
	{
		friend class IO;

	public:
		typedef enum
		{
			DigitalInput,		//a GPIO input
			DigitalOutput,		//a GPIO output
			AnalogInput,		//ADC feature
			
			Reset,				//!reset mode
			SPI,				//Pin carries SPI signals
			I2C,				//I2C mode, electrically open-drain
			UART,				//UART peripheral on this pin
			PWM,				//connected to the timer/PWM system
			USB,				//the Pin is in USB interface mode
			
			ClockOutput,		//clock output signal is driven on the pin
			Wakeup,				//(not currently supported)
			
			Manual = 0xFE,
			Default,
		} Mode;
		
		//not all parts feature these pin modes
		typedef enum
		{
			Normal		= 0,	//the pin does not have internal pull-up or pull-down resistors
			PullDown	= 1,	//an internal resistor pulls the value down when not driven
			PullUp		= 2,	//like PullDown but the value is pulled up to Vdd when not driven
			
			Sensitive	= 4,	//Sensistive mode implies hysteresis/Schmitt-triggers are disabled for the pin

			//OpenDrain means a logic high results in a high-impedance (un-driven) pin
			//  and a logic low drives the pin low
			OpenDrain	= 8,
		} FeatureSetting;
		typedef int Feature;
		
		inline			Pin(void): v(~0)					{}
		
		//make a Pin refer to the same hardware as another Pin
		inline	Pin&	bind(Pin const& p)		{v = p.v; return(*this);}
		
		//set the Pin's state (driving output if applicable)
		inline	Pin&	operator =(bool value)	{write(value? 1 : 0); return(*this);}
		inline	Pin&	operator =(int value)	{write(value); return(*this);}
		inline	Pin&	operator =(Pin& p)		{write(p.read()); return(*this);}
		void			write(int value);
		
		//read the Pin's digital state
		inline			operator bool(void)		{return((bool)read());}
		int				read(void) const;
		
		//read an analog value via the ADC for Pins that support it
		unsigned int	readAnalog(void) const;
		unsigned int	analogValue(void) const;
		
		//set the Pin's I/O mode and features, with shorthand methods for common modes
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

/*                                                                         |
                                                                           |
   ## SPI (3-wire/4-wire synchronous serial interface)                     |
                                                                           |
   SPI is a three-wire bus with duplex serial lanes and out-of-band device |
   selection.  Unlike I2C, it has no standard data size, clock polarity,   |
   arbitration, acknowledgement or other high-level features.  However     |
   this simplicity also gives SPI great flexibility.  Read more here:      | 
   http://en.wikipedia.org/wiki/Serial_Peripheral_Interface                |
                                                                           |
   The SPI class gives you access to this interface.  To enable it, call   |
   `.start()` with the bitrate and options you desire.  With no arguments  |
   specified, the class picks reasonable defaults:                         |
                                                                           |
     io.spi.start();  // uses reasonable defaults: 2MHz, 8-bit, mode 0     |
                                                                           |
     // perhaps you have an unusual application that needs 12-bit mode 2:  |
     io.spi.start(10000000UL, IO::SPI::Mode2 | IO::SPI::CharsAre12Bit);    |
                                                                           |
   To stop the interface, call `.stop()`.  Existing queued reads and       |
   and writes will be dropped, and their tasks will complete as failed.    |
                                                                           |
   Reading and writing data on an SPI interface is simultaneous, so many   |
   methods are offered for reads and writes of different data types        |
   to be used according to your input and output needs:                    |
                                                                           |
   * `.read()` methods to read while writing repetitive or dummy data      |
   * `.write()` methods to write dummy or repetitive data while reading    |
   * `.readAndWrite()` methods to simultaneously read and write data       |
   * `.write()` methods to write data with optional reading                |
                                                                           |
   There's intentionally a high degree of overlap between the methods to   |
   facilitate writing expressive code, and all methods default to the most |
   obvious behaviour if parameters are omitted.                            |
                                                                           |
   The SPI subsystem also has `.lock()` and `.unlock()` methods comprising |
   a mutex interface, which you can use for strict multiplexing.  At the   |
   present time this interface is considered experimental, please don't    |
   rely on it yet.                                                         |
                                                                           |
                                                                           |
==========================================================================*/
	
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
		
		Task			lock(void);
		bool			unlock(void);
	};

/*                                                                         |
                                                                           |
   ## I2C bus (Inter-IC bus)                                               |
                                                                           |
   I2C is a two-wire bus that supports multiple masters and slaves with    |
   addressing, collision avoidance, acknowledgement and slave bit delays.  |
   It's widely used for a large range of peripherals including sensors,    |
   memory devices, human interfaces and displays.  Read more here:         |
   http://en.wikipedia.org/wiki/I2C                                        |
                                                                           |
   I2C is implemented with an elegant interface consisting of just four    |
   methods: `.start()` to start the interface, `.stop()` to stop it,       |
   `.write()` to request a write packet and `.read()` to request a read.   |
                                                                           |
   To start, call `.start()` and pass a bitrate.  If not specified, that   |
   defaults to the I2C standard of 100KHz.  400KHz and 1MHz are supported  |
   on certain slave devices, and slower speeds are always possible.  Call  |
   `.stop()` to stop the interface, which will drop pending I2C tasks and  |
   complete their corresponding tasks as failed.                           |
                                                                           |
   To read data from a slave, use the `.read()` method.  Pass the slave    |
   address and a Buffer object.  The length of the read is implicit from   |
   the length of the Buffer.  For example:                                 |
                                                                           |
     struct Context                                                        |
     {                                                                     |
       Buffer i2cRead;                                                     |
     };                                                                    |
                                                                           |
     ...                                                                   |
                                                                           |
     // create a context object and a 10-byte buffer to read into          |
     Context* c = new Context();                                           |
     c->i2cRead = Buffer::New(10);                                         |
                                                                           |
     // read 10 bytes from address 0x99                                    |
     Task t = io.i2c.read(0x99, c->b);                                     |
                                                                           |
     system.when(t, &onComplete, c);                                       |
                                                                           |
     ...                                                                   |
                                                                           |
     void onComplete(void* c, Task t, bool success)                        |
     {                                                                     |
       Context* context = (Context*)c;                                     |
                                                                           |
       // parameter 'success' indicates if the operation succeeded         |
       // context->i2cRead contains the read bytes                         |
     }                                                                     |
                                                                           |
   Exactly the same approach can be used for I2C write operations.         |
                                                                           |
                                                                           |
==========================================================================*/

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

/*                                                                         |
                                                                           |
   ## UART (Asynchronous serial port)                                      |
                                                                           |
   UARTs are very common serial interfaces traditionally used to connect   |
   connect computers, modems, communications devices and countless other   |
   applications.  Unlike SPI and I2C it's asynchronous and therefore       |
   relies on accurate absolute time references on both the sending and     |
   receiving ends.  Various baud-rates and encodings are common.  Read     |
   more here:                                                              |
   http://en.wikipedia.org/wiki/UART                                       |
                                                                           |
   The UART is accessed through the io.serial object, and manages queues   |
   and buffers for sending and receiving data.  To enable it, use the      |
   `.start()` method:                                                      |
                                                                           |
     io.serial.start();  // starts with default settings, 9600 8-n-1       |
                                                                           |
     // 38400 baud, 7-bit, no parity, one stop bit                         |
     io.serial.start(38400, IO::UART::CharsAre7Bit);                       |
                                                                           |
   The UART is a fully asynchronous device, so data can be sent and        |
   received in an independent fashion.  Building on GalagoAPI constructs,  |
   the UART exposes a Task that allows you to respond to data when it has  |
   been received without having to poll (constantly check for it):         |
                                                                           |
     system.when(io.serial.bytesReceived(), &onSerialData);                |
                                                                           |
   Task semantics are maintained so when data is available, the task       |
   completes.  A new, unresolved task is created the following time        |
   `.bytesReceived()` is called, which is ready for the next time data     |
   comes in.                                                               |
                                                                           |
   Reading from the UART is done with `.read()` methods, which read from   |
   the internal CircularBuffer inside the UART instance.  The number of    |
   available bytes can be determined with the `.bytesAvailable()` method.  |
                                                                           |
   Writing to the serial port is done with the `.write()` methods, which   |
   are offered in different versions for different data types.  The ones   |
   that take a single datum as their first argument also take a formatting |
   option from the `IO::UART::Format` enum to control how it's rendered    |
   to human-readable characters (e.g. 97 => "97" or 97 => "a").            |
                                                                           |
                                                                           |
==========================================================================*/
	
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

/*                                                                         |
   Galago System API: `Galago::system`                                     |
                                                                           |
   System is responsible for the CPU, memory, clocks, time and Tasks.      |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Tasks                                                                |
                                                                           |
   A fundamental concept in GalagoAPI is the Task, which is a type of      |
   promise, a value that represents ongoing work until it is completed.    |
   When completed, a task becomes either successful or failed.  Many       |
   asynchonous methods return Tasks, which indicate that they're waiting   |
   for hardware features in the background.  This is not the same as       |
   threads or processes in high-level operating systems, but more like a   |
   signal that something has completed on an independent timeline.         |
                                                                           |
   ### Properties of Tasks (important!)                                    |
                                                                           |
   *  They are created initially in an unresolved state                    |
   *  They can only be resolved (to true or false) once.                   |
   *  When Tasks are resolved, any completion callbacks are called         |
   *  If completions are added after resolution, they are still called     |
   *  All completions are only called once per Task they're bound to       |
                                                                           |
   These properties make Tasks impervious to race conditions, which        |
   enables asynchronous programming without many normal frustrations.      |
                                                                           |
   ### Completions                                                         |
                                                                           |
   If you need to do something when a Task has completed, there are two    |
   methods, responding and blocking.  Responding is done as follows:       |
                                                                           |
     system.when(aTask, &aCompletionFunction, optionalContext);            |
                                                                           |
     ...                                                                   |
                                                                           |
     void aCompletionFunction(void* context, Task t, bool success)         |
     {                                                                     |
       // t is the Task we're responding to                                |
       // success is the reslution of the Task, true or false              |
       // context is the optionalContext from above (defaults to null)     |
     }                                                                     |
                                                                           |
   You may call `system.when()` multiple times to assign multiple          |
   completions to a Task.                                                  |
                                                                           |
   In some cases, you way wish to simply wait for a task to finish.        |
   This stops the flow of your program, and is thus known as blocking:     |
                                                                           |
     system.wait(aTask);                                                   |
                                                                           |
   When waiting for a task, the completion callbacks for other concurrent  |
   tasks are still called, which prevents deadlock conditions.             |
                                                                           |
   ### Summation                                                           |
                                                                           |
   Because Tasks are objects, you pass them by value.  Moreover,           |
   you may add them to create Tasks that are only complete when all        |
   constituent Tasks are complete, or fail when the first constituent      |
   fails:                                                                  |
                                                                           |
     Task allDone = shutdownWirelessTask + closeFileTask;                  |
                                                                           |
     system.when(allDone, &powerDown);                                     |
                                                                           |
   Of course, the properties of Tasks allow you to add them both before    |
   and after they're resolved.                                             |
                                                                           |
   ### Your own Tasks                                                      |
                                                                           |
   To create a Task, use the following:                                    |
                                                                           |
     Task myTask = system.createTask();                                    |
                                                                           |
   To complete it, for example as a result of other asynchronous actions:  |
                                                                           |
     system.completeTask(myTask);                                          |
     // or:                                                                |
     system.completeTask(myTask, false);  // if it failed                  |
                                                                           |
   You may also abort a task by calling:                                   |
                                                                           |
     system.completeTask(taskToAbort, false);                              |
                                                                           |
   Be aware, however, that this will call completions with a failed task   |
   but it doesn't guarantee that the underlying operations, such as I/O,   |
   will stop.  In most practical cases, however, it doesn't matter.        |
                                                                           |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Time                                                                 |
                                                                           |
   Time intervals in GalagoAPI are requested using `system.delay` and      |
   `system.delayMicro` and return Tasks:                                   |
                                                                           |
     Task oneSecond = system.delay(1000);  // time in milliseconds         |
                                                                           |
   With a Task, you may respond when the time is up:                       |
                                                                           |
     system.when(oneSecond, &turnOnLamp);  // turnOnLamp() after 1 second  |
                                                                           |
   Time Tasks can also be used in concert with other asynchronous actions  |
   like I/O or user input:                                                 |
                                                                           |
     // this combined task will be done when "hello!" is sent, but no      |
     //   faster than twice per second (once per 500ms.)                   |
     Task t = io.serial.write("hello!") + system.delay(500);               |
                                                                           |
   Lots of sophisticated behaviours are possible with time and Tasks,      |
   so be creative!                                                         |
                                                                           |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Clocks                                                               |
                                                                           |
   System is also in control of core clocks.  The core CPU frequency and   |
   the clock output frequency may be adjusted.  In both cases, the         |
   nearest possible frequency is chosen - it may be slightly higher or     |
   lower than desired, but it will be the closest of the two.  To find     |
   the actual frequency, call the `getCoreFrequency()` or                  |
   `getClockOutputFrequency()` method.  To stop a clock, set the frequency |
   to 0.                                                                   |
                                                                           |
                                                                           |
   ---------------------------------------------------------------------   |
                                                                           |
   ## Memory                                                               |
                                                                           |
   System exposes `alloc()` and `free()` methods that work like their C    |
   standard library counterparts.  Their current implementation is         |
   optimized for low code and memory overhead, not speed.  However,        |
   remember that Cortex-M chips usually have disproportionally high CPU    |
   speeds compared to memory and code size, so it's appropriate.           |
                                                                           |
   Most memory allocation can be done with C++ operator new / delete, but  |
   these methods exist for flexibility.                                    |
                                                                           |
   Failure to allocate memory currently causes a debugger exception,       |
   which is a crash when no debugger is attached.  Overriding the          |
   HardFault exception handler will let you catch it.                      |
                                                                           |
                                                                           |
==========================================================================*/

class System
{
public:

	//returns allocated memory of the desired size.  Memory is always 4-byte-aligned.  Faults on out-of-memory.
	static void*	alloc(size_t size);
	
	//frees memory allocated with alloc().  Does nothing on already freed, non-dynamic or invalid memory.
	static void		free(void* allocation);
	
	unsigned int	getMainClockFrequency(void) const;
	
	//gets or sets the core CPU frequency.  Values too high or low are constrained.
	unsigned int	getCoreFrequency(void) const;
	void			setCoreFrequency(unsigned int desiredFrequency);
	
	//gets or sets the clock output frequency.  The appropriate IO pin must be set to clock output mode to use.
	//  set frequency to 0 to turn off.
	unsigned int	getClockOutputFrequency(void) const;
	void			setClockOutputFrequency(unsigned int desiredFrequency);
	
	//sleep until the next hardware task is complete.  The system may wake for any reason at any time.
	//  If no hardware or async actions are underway, or if nothing is set to wake the core, it would
	//  never wake up.
	void			sleep(void) const;
	
	//create a Task in the unresolved state.
	Task			createTask(void);
	
	//complete (resolve) an unresolved Task with the specified (optional) success value, which defaults to true.
	bool			completeTask(Task t, bool success = true);
	
	//The following methods use the minimum power by sleeping when possible:

	//asynchronously respond when a task is complete.  See the Task documentation above.
	bool			when(Task t, void (*completion)(void* context, Task, bool success), void* completionContext = 0);
	
	//synchronously wait for a task to complete. Completion callbacks for other tasks will be called from within this
	//  method, making it not strictly blocking (preventing deadlocks.)  See the Task documentation above.
	bool			wait(Task t);
	
	//create a task that is complete in the specified number of milliseconds.  See the Time documentation above.
	Task			delay(unsigned int milliseconds);
	//...or microseconds.  See the Time documentation above.
	Task			delayMicro(unsigned int microseconds);

	//constructor.  Don't call this, as there's only one system object.
					System(void);
private:
	static void		invokeDeferredCallbacks(void);
};

extern IO		io;
extern System	system;

}	//ns Galago


//C++ memory methods

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