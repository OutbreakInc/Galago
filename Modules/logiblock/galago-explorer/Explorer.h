#ifndef __LOGIBLOCK_EXPLORER_H__
#define __LOGIBLOCK_EXPLORER_H__

#include <GalagoAPI.h>
using namespace Galago;

namespace Logiblock { namespace AppBoards {

class Explorer
{
public:
	inline unsigned int		yearUTC() const					{return(_date.year);}
	inline unsigned int		monthUTC() const				{return(_date.month);}
	inline unsigned int		dayUTC() const					{return(_date.day);}
	
	inline unsigned int		hoursUTC() const				{return(_time.hours);}
	inline unsigned int		minutesUTC() const				{return(_time.minutes);}
	inline unsigned int		secondsUTC() const				{return(_time.seconds);}
	
	inline int				latitude() const				{return(_lat);}
	inline int				longitude() const				{return(_long);}
	inline int				altitudeFromEllipsoid() const	{return(_altFromEllipsoid);}
	inline int				altitudeFromSeaLevel() const	{return(_altFromGeoid);}
	
	inline int				latitude() const				{return(_lat);}
	inline int				longitude() const				{return(_long);}
	inline int				altitudeFromEllipsoid() const	{return(_altFromEllipsoid);}
	inline int				altitudeFromSeaLevel() const	{return(_altFromGeoid);}
	
	inline int				temperatureInCelsius() const	{return(_temperature);}

							Explorer(void);
	
	//initialize the app board.  Returns true if a board was found.
	bool					init(void);
	
	//enable or disable the NMEA (GPS) data on the RXD/TXD pins.  This occupies the UART if enabled.
	bool					enableNMEAData(bool enable = true);
	inline bool				disableNMEAData(void)	{return(enableNMEAData(false));}
	
	bool					selectSDCard(void);
	
	//process the NMEA data one character at a time.  Typically you would feed received characters
	//  (or batchs thereof) to this function as they arrive.
	void					processGPSData(char nextChar);
	
	//poll the accelerometer for new data
	Task					updateAccelerometer(void);
	
	Task					updateTemperature(void);
	
	Task					newGPSDataReady(void);
	
private:
	static void		updateAccelerometerComplete(void* c, Task, bool success);
	void			completeDataTask(void);
	unsigned int	compare(char const* m1, char const* m2, int count);
	void			skipField(char const*& m);
	unsigned int	parseInt(char const*& m, unsigned int count = -1);
	unsigned int	parseHex(char const*& m, int i);
	void			parseTime(char const*& m);
	void			parseDate(char const*& m);
	unsigned int	parseLatLong(char const*& m, int degreeDigits);
	
	void			parseAccelerometer(byte const* m);
	
	Task			_newGPSData;
	Task			_accelRead;
	Buffer			_accelData;
	
	char*			_buffer;
	
	struct
	{
		unsigned char hours;
		unsigned char minutes;
		unsigned char seconds;
		unsigned char millis;
	} _time;
	struct
	{
		unsigned short year;
		unsigned char month;
		unsigned char day;
	} _date;
	
	signed int		_lat;
	signed int		_long;
	signed short	_altFromGeoid;		// == tideless sea level if the Earth was covered in ocean
	signed short	_altFromEllipsoid;
	unsigned short	_speed;
	unsigned short	_heading;	//0-359 degrees
	unsigned char	_fix, _sats, _hdop;
	
	signed char		_tiltX, _tiltY, _tiltZ;
	
	byte			_checksumCount;
	byte			_appBoardAddress;
};

extern Explorer explorer;

}}	//ns

#endif //__LOGIBLOCK_EXPLORER_H__
