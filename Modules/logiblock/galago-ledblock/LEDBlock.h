#ifndef __LOGIBLOCK_LEDBLOCK_H__
#define __LOGIBLOCK_LEDBLOCK_H__

#include <GalagoAPI.h>
using namespace Galago;

namespace Logiblock { namespace AppBoards {

class LEDBlock
{
public:
							LEDBlock(void);
	
	//Initialize the app board.  Returns true if a board was found.
	bool					init(void);
	
	//The refresh frequency is how many PWM cycles there are per second.  The grayscale clock
	//  (master clock) for the TLC5940 will run at 4096 times this rate, often yielding a
	//  GSCLK frequency of 2-4MHz (for about 500-100Hz refresh respectively.)
	bool					start(int refreshFrequency = 500);
	inline bool				stop(void)	{return(start(0));}
	
	//Enable or disable the timing signals, which operate independently of loading new pixel data
	//  into the TLC5940.  If these signals stop, LEDs will freeze in whatever state they're currently
	//  in, so if you wish to stop the display, send dark pixel values to the controller(s) and then
	//  disable timing signals one PWM cycle later (waiting a few milliseconds is enough)
	bool					enableTimingSignals(bool enabled = true);
	inline bool				disableTimingSignals(void)		{return(enableTimingSignals(false));}
	
	//Select the TLC chip(s) on the SPI bus.  This selection is released when further GXB actions occur;
	//  for this module that includes the enable/disableTimingSignals() and start() methods
	bool					select(void);
	
	//Write 16-bit levels (chopped and packed to 12-bit) to the controller.  If you wish to send
	//  raw 12-bit levels, you may call select() and write them to the SPI bus yourself.
	//Use write() to send incremental data and end() to finish the transfer, which schedules a latch
	//  (save) operation for the next refresh cycle.  You may combine a write with end() by passing
	//  arguments to it, this just keeps your code simpler.
	Task					write(unsigned short const* levels, int length);
	Task					end(unsigned short const* levels = 0, int length = 0);
	
	void					pulseLatch(void);
	
private:
	
	static void				queueLatch(void*, Task, bool);
	
	Task					_lastTask;
	Buffer					_buffer;
	byte					_ledAppBoard;
	bool					_latch;
};

extern LEDBlock ledBlock;

} } //namespace Logiblock::AppBoards

#endif //__LOGIBLOCK_LEDBLOCK_H__
