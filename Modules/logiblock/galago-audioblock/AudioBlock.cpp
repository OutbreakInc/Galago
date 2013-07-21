#include <GalagoAPI.h>
#include <AppBoard.h>
#include <LPC13xx.h>
#include <AudioBlock.h>

using namespace Galago;
using namespace Logiblock::AppBoards;

//The Audioblock (Audio App Board) has an NXP UDA1334ATS, which has accepts up to 24bits per sample per channel
//  and feeds it to a '1-bit' delta-sigma DAC core, with internal processing (seemingly) similar to pulse-density
//  modulation.  Sample clocking and analog generation is handled by a built-in PLL which is disciplined by the
//  WS signal (aka the word clock, the left-right clock or the sample clock) so passing a high-frequency master
//  clock is not necessary.  Sample clocks as high as 100kHz are supported, enabling a massive 4.8Mbit/s stream.

//When playing back audio, timing and consistency are crucial.  For the left-justified stream that we send to
//  the DAC, one sample (either for the left or right channel) needs to be transfered per half-period of the sample
//  clock.  The DAC has no FIFO so we cannot send data in bursts, nor can we use any hardware features in the LPC13xx
//  mcu to generate the signal without appropriating the SPI interface, which we need for reading audio data off an
//  SD card.  To that end, a PWM/bit-bang approach was chosen that uses the P2, P3 and P5* pins.  P5* supports
//  hardware PWM on Timer1, so we use that to generate the very-low-jitter sample clock that steers the DAC's PLL.
//  We also set it to generate an interrupt which we use to write the next sample with the P2 and P3 pins.

//The lack of a hardware I2S or second SSP/SPI port on Galago, combined with the lack of DMA on this chip means that
//  sending audio data to the DAC is costly.  Please be aware of the timing and cycle demands if using high-bitrate
//  streams.

For many use cases, reading audio samples from (e.g.) an SD card while playing them

unsigned int	System_divideClockFrequencyRounded(unsigned int n, unsigned int d)
{
	unsigned int q = (((n << 1) / d) + 1) >> 1;
	return(q? q : 1);
}

extern "C"
INTERRUPT void		IRQ_Timer1(void)
{
	//hardware has already toggled the BCK signal, so output the next sample of left-justified audio data right away
	
	int sample = 0, bits;
	audioBlock._buffer->read(((byte*)&sample) + 1);
	if(audioBlock._bytesPerSample == 1)
		bits = 8;
	else
	{
		audioBlock._buffer->read((byte*)sample);
		bits = 16;
	}
	
	//squirt out the bits as fast as possible. At 72MHz a 16-bit sample can be written in under 4us.
	while(bits--)
	{
		LPC1300::GPIO0[1 << 2] = 0;
		__asm__ volatile ("nop"::);
		LPC1300::GPIO1[1 << 8] = (sample & 0x8000)? (1 << 8) : 0;
		sample <<= 1;
		LPC1300::GPIO0[1 << 2] = (1 << 2);
	}
	__asm__ volatile ("nop"::);
	LPC1300::GPIO0[1 << 2] = 0;
	
	*LPC1300::Timer1Interrupts = LPC1300::TimerInterrupts_Match0Flag;
}


			AudioBlock::AudioBlock(void):
				_buffer(0)
{
}

			AudioBlock::init(void)
{
}
	
bool		AudioBlock::begin(		int sampleRate = 48000,
									int bitsPerSample = 16,
									ChannelMode channelMode = Stereo,
									bufferLength_ms = 10
								)
{
	if(_buffer != 0)
	{
		delete _buffer;
		_buffer = 0;
	}
	
	//shutdown
	*LPC1300::Timer1Control = LPC1300::TimerControl_Reset;
	*LPC1300::Timer1MatchControl = 0;
	*LPC1300::ClockControl &= ~LPC1300::ClockControl_Timer1;
	
	if(sampleRate != 0)
	{
		if((bufferLength_ms <= 2) || (bitsPerSample > 24))
			return(false);
		
		switch(channelMode)
		{
		case Stereo:
			bitsPerSample <<= 1;
			break;
		case Mono:
		case LeftOnly:
		case RightOnly:
			break;
		default:
			return(false);
		}
		
		io.p2.setOutput();	//serial data (SIN)
		io.p3.setOutput();	//serial clock (BCK)
		io.p5.setPWM();		//monotonic sample clock (aka left-right clock, word select)
		
		_bytesPerSample = bitsPerSample >> 3;
		_mode = channelMode;
		_buffer = new CircularBuffer(sampleRate * _bytesPerSample * bufferLength_ms / 1000);
		
		_divisor = System_divideClockFrequencyRounded(system.getMainClockFrequency(), sampleRate * 4);
		_sampleIndex = 0;
		
		//set up PWM on p5 (timer1 match0) and interrupt for audio samples
		*LPC1300::ClockControl |= LPC1300::ClockControl_Timer1;
		
		*LPC1300::Timer1Control = (LPC1300::TimerControl_Enable | LPC1300::TimerControl_Reset);
		
		*LPC1300::Timer1Prescaler = _divisor;
		*LPC1300::Timer1MatchControl =	LPC1300::TimerMatchControl_Match0Interrupt
										| LPC1300::TimerMatchControl_Match0Reset;
		*LPC1300::Timer1ExternalMatch = LPC1300::TimerExternalMatch_Match0Toggle;
		*LPC1300::Timer1Match0 = 1;
		
		*LPC1300::Timer1Control = LPC1300::TimerControl_Enable;
		
		*LPC1300::InterruptEnableSet1 = LPC1300::Interrupt1_Timer1;
		InterruptsEnable();
	}
}

int			AudioBlock::playSamples(Buffer b)
{
	return(_buffer->write(b.bytes(), b.length()));
}

int			AudioBlock::playSamples(void const* samples, int length)
{
	return(_buffer->write((byte const*)samples, length));
}

Task		AudioBlock::needSamplesTask(void)
{
	if(_needSamples == Task())
		_needSamples = system.createTask();
	return(_needSamples);
}

void		AudioBlock::requestSamples(void)
{
	if(_needSamples != Task())
	{
		Task t = _needSamples;
		_needSamples = Task();
		system.completeTask(t);
	}
}
