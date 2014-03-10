#include <GalagoAPI.h>
#include <AppBoard.h>
#include "Explorer.h"

using namespace Galago;
using namespace Logiblock;
using namespace Logiblock::AppBoards;

static int const BUFFER_SIZE = 100;

				Explorer::Explorer(void):
					_appBoardAddress(0),
					_buffer(0),
					_lat(0),
					_long(0),
					_altFromGeoid(0),
					_altFromEllipsoid(0),
					_fix(0),
					_sats(0),
					_hdop(0),
					_checksumCount(3)
{
}

bool			Explorer::init(void)
{
	activeLED.bind(io.p2);
	lockLED.bind(io.p6);

	//detect the board and come back with an address
	_appBoardAddress = appBoard.find(0, 0x0b1, 0xac05);
	
	if(_appBoardAddress == 0)
		return(false);
	
	if(_buffer == 0)
		_buffer = new char[BUFFER_SIZE];
	_buffer[0] = 0;
	
	_accelData = Buffer::New(10);
	
	Buffer accelInit = Buffer::New(2);
	accelInit.bytes()[0] = 7;
	accelInit.bytes()[1] = 0x01;	//write 0x01 to register 7
	io.i2c.write(0x98, accelInit);	//init the accelerometer
	
	return(true);
}

bool			Explorer::enableNMEAData(bool enabled)
{
	if(_appBoardAddress == 0)
		return(false);
		
	byte txrxEnabled = enabled? 0x10 : 0x11;
	return(appBoard.write(_appBoardAddress, &txrxEnabled, 1));
}

void			Explorer::processGPSData(char nextChar)
{
	if(_buffer == 0)
		return;
	
	int offset = _buffer[0];
	
	if((nextChar == '$') || (offset >= BUFFER_SIZE))
		offset = 0;
	
	_buffer[++offset] = nextChar;
	
	if(_checksumCount < 3)	_checksumCount++;
	if(nextChar == '*')		_checksumCount = 0;
	
	if(_checksumCount == 2)
	{
		//parse packet and update state
		char const* m = _buffer + 8;
		
		byte checksum = 0;
		char const* cs = _buffer + 2;
		while(*cs != '*')	checksum ^= *cs++;
		
		if(checksum == parseHex(++cs, 2))
		{
			//parse packet and update state
			char const* m = _buffer + 8;
			
			//GPGGA messages are point fix (position) data
			if(compare(_buffer + 2, "GPGGA", 5) == 0)
			{
				parseTime(m);
				_lat = parseLatLong(m, 2);
				_long = parseLatLong(m, 3);
				_fix = parseInt(m);
				_sats = parseInt(m);
				_hdop = parseInt(m);
				_altFromGeoid = parseInt(m);
				skipField(m);	//units, presumed to be metres
				_altFromEllipsoid = _altFromGeoid + parseInt(m);
				
				completeDataTask();
			}
			//GPRMC messages are a general notice of position, heading, date and time
			else if(compare(_buffer + 2, "GPRMC", 5) == 0)
			{
				parseTime(m);
				if(*m == 'A')	//check validity
				{
					m += 2;
					_lat = parseLatLong(m, 2);
					_long = parseLatLong(m, 3);
					_speed = parseInt(m);
					_heading = parseInt(m);
					parseDate(m);
					
					completeDataTask();
				}
			}
		}
		offset = 0;
	}
		
	_buffer[0] = offset;
}

Task			Explorer::updateAccelerometer(void)
{
	if(_accelRead != Task())
		system.completeTask(_accelRead, false);	//cancel the old task
	
	_accelRead = system.createTask();
	
	system.when(io.i2c.read(0x98, _accelData), updateAccelerometerComplete, this);
	
	return(_accelRead);
}

Task			Explorer::newGPSDataReady(void)
{
	if(_newGPSData == Task())
		_newGPSData = system.createTask();
	return(_newGPSData);
}
	
//static
void			Explorer::updateAccelerometerComplete(void* c, Task, bool success)
{
	Explorer* self = (Explorer*)c;
	
	if(success)
		self->parseAccelerometer(self->_accelData.bytes());
	
	system.completeTask(self->_accelRead, success);
	self->_accelRead = Task();
}

void			Explorer::completeDataTask(void)
{
	if(_newGPSData != Task())
	{
		Task t = _newGPSData;
		_newGPSData = Task();
		system.completeTask(t);
	}
}

unsigned int	Explorer::compare(char const* m1, char const* m2, int count)
{
	while(count && *m1 && *m2 && (*m1 == *m2))	{m1++; m2++; count--;}
	return((count == 0)? 0 : (*m2 - *m1));
}
void			Explorer::skipField(char const*& m)
{
	while(*m && (*m != ',') && (*m != '*'))	m++;
	m++;
}
unsigned int	Explorer::parseInt(char const*& m, unsigned int count)
{
	unsigned int v = 0, neg = 0;
	if(*m == '-')
		neg = *m++;
	while(count && *m && (*m != ',') && (*m != '*'))
	{
		if(*m != '.')
			v = (v * 10) + (*m - '0');
		m++;
		count--;
	}
	if(neg)		v = -v;
	if(count)	m++;
	return(v);
}
unsigned int	Explorer::parseHex(char const*& m, int i)
{
	unsigned int v = 0;
	while(i--)
	{
		v <<= 4;
		byte c = (*m++ & ~32) - 16;
		if(c < 10)	v |= c;
		else
		{
			c -= 49;
			if(c < 6)	v |= (c + 10);
			else		break;
		}
	}
	return(v);
}
void			Explorer::parseTime(char const*& m)
{
	_time.hours = parseInt(m, 2);
	_time.minutes = parseInt(m, 2);
	_time.seconds = parseInt(m, 2);
	_time.millis = parseInt(++m);
}
void			Explorer::parseDate(char const*& m)
{
	_date.day = parseInt(m, 2);
	_date.month = parseInt(m, 2);
	_date.year = parseInt(m) + 2000;
}
unsigned int	Explorer::parseLatLong(char const*& m, int degreeDigits)
{
	unsigned int deg = 0;
	
	while(degreeDigits--)
		deg = (deg * 10) + (*m++ - '0');
	
	unsigned int min = parseInt(m);
	min *= 3579;	//convert 0-59 to 22-bit fraction
	min >>= 9;
	
	deg = (deg << 22) | min;
	
	if((*m == 'W') || (*m == 'S'))	//convert to Cartesian coordinates
		deg = -deg;
	
	m += 2;
	return(deg);	//signed 10.22 fixed-point result
}

void			Explorer::parseAccelerometer(byte const* m)
{
	_tiltX = m[0] << 2;
	_tiltY = m[1] << 2;
	_tiltZ = m[2] << 2;
	//...
}

namespace Logiblock { namespace AppBoards {

	Explorer explorer;

}}	//ns
