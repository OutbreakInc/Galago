#include <GalagoAPI.h>
#include <AppBoard.h>
#include <LEDBlock.h>
#include <LPC13xx.h>

using namespace Galago;
using namespace Logiblock::AppBoards;


//global singleton
namespace Logiblock { namespace AppBoards { LEDBlock ledBlock; } }

			LEDBlock::LEDBlock(void):
				_ledAppBoard(0),
				_latch(0)
{
}

bool		LEDBlock::init(void)
{
	return((_ledAppBoard = appBoard.find(0, 0x0b1, 0xac03)) != 0);
}


bool		LEDBlock::start(int refreshFrequency)
{
	//shutdown
	*LPC1300::Timer1Control = LPC1300::TimerControl_Reset;
	*LPC1300::Timer1MatchControl = 0;
	*LPC1300::ClockControl &= ~LPC1300::ClockControl_Timer1;
	
	_buffer = Buffer();
	
	if((_ledAppBoard != 0) && (refreshFrequency > 0))
	{
		io.p1.setMode(IO::Pin::ClockOutput);
		io.p3.setOutput();
		io.p5.setPWM();		//we use p5 as a precision PWM output for the BLANK signal
		
		system.setClockOutputFrequency(refreshFrequency << 12);
		
		//set up PWM on p5 (timer1 match0) and interrupt for latch pulses, if scheduled
		*LPC1300::ClockControl |= LPC1300::ClockControl_Timer1;
		
		*LPC1300::Timer1Control = (LPC1300::TimerControl_Enable | LPC1300::TimerControl_Reset);
		
		//lock the timer frequency to the clock output frequency so that we can send a pulse every 4096 cycles
		*LPC1300::Timer1Prescaler = *LPC1300::ClockOutputDivider;
		
		*LPC1300::Timer1PWMControl = LPC1300::TimerPWMControl_Match0 | LPC1300::TimerPWMControl_Match1;
		
		*LPC1300::Timer1MatchControl =	LPC1300::TimerMatchControl_Match0Interrupt
										| LPC1300::TimerMatchControl_Match1Reset;
		
		*LPC1300::Timer1Match0 = 4096;	//at 4096 GSCLKs, pulse BLANK and fire the interrupt
		*LPC1300::Timer1Match1 = 4116;	//at 4096 + Tblank GSCLKs, drop BLANK and reset the cycle
		
		*LPC1300::Timer1Control = LPC1300::TimerControl_Enable;
		
		*LPC1300::InterruptEnableSet1 = LPC1300::Interrupt1_Timer1;
		InterruptsEnable();
	}
}

bool		LEDBlock::enableTimingSignals(bool enabled)
{
	byte const enableTiming = enabled? 0x10 : 0x11;
	return((_ledAppBoard != 0) && appBoard.write(_ledAppBoard, &enableTiming, 1));
}

bool		LEDBlock::select(void)
{
	byte const selectCommand = 0x00;
	return((_ledAppBoard != 0) && appBoard.write(_ledAppBoard, &selectCommand, 1));
}

Task		LEDBlock::write(unsigned short const* levels, int length)
{
	if(((length * 3) >> 2) > _buffer.length())	//grow
		_buffer = Buffer::New((length * 3) >> 2);
	
	byte* p = _buffer.bytes();
	
	//convert 2 x 16bpp into 2 x 12bpp packed in 3 bytes
	for(int i = 0; i < (length >> 1); i += 2)
	{
		*p++ = levels[i] >> 4;
		*p++ = (levels[i] << 4) | (levels[i + 1] >> 8);
		*p++ = levels[i + 1];
	}
	if(length & 1)	//handle odd numbers of samples.
	{
		*p++ = levels[length - 1] >> 4;
		*p++ = levels[length - 1] << 4;
		*p++ = 0;
	}
	
	//write
	io.spi.write(_buffer.bytes(), _buffer.length());
}

Task		LEDBlock::end(unsigned short const* levels, int length)
{
	Task t;
	if(levels != 0)
		t = write(levels, length);
	else
	{
		//a zero-byte write is more appropriate than lock/unlock right now
		t = io.spi.write((byte const*)0, 0);
	}
	
	system.when(t, &LEDBlock::queueLatch, this);
}


//static
void		LEDBlock::queueLatch(void* p, Task, bool)
{
	LEDBlock volatile* self = (LEDBlock volatile*)p;
	
	self->_latch = true;
}


void		LEDBlock::pulseLatch(void)
{
	if(_latch)
	{
		_latch = false;
		
		//strobe latch
		io.p3 = true;
		io.p3 = false;
		
		//io.spi.unlock();	//if I go back to the lock/unlock pattern
	}
	
	//acknowledge interrupt
	*LPC1300::Timer1Interrupts = LPC1300::TimerInterrupts_Match0Flag;
}
