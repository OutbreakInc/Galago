#ifndef __LOGIBLOCK_AUDIOBLOCK_H__
#define __LOGIBLOCK_AUDIOBLOCK_H__

#include <GalagoAPI.h>
using namespace Galago;

namespace Logiblock { namespace AppBoards {

class AudioBlock
{
public:
							AudioBlock(void);
	
	//initialize the app board.  Returns true if a board was found.
	bool					init(void);
	
	bool					setOutput(bool outputOn);
	
	inline bool				mute(void)		{return(setOutput(false));}
	inline bool				unmute(void)	{return(setOutput(true));}
	
	bool					selectSDCard(void);
	bool					enableDACPins(bool enable = true);
	inline bool				disableDACPins(void)	{return(enableDACPins(false));}
	
	typedef enum
	{
		Mono,
		Stereo,
		LeftOnly,
		RightOnly,
	} ChannelMode;
	
	//this is the very timing-sensitive interrupt handler, and it must be called
	//  (2 * the sample rate) times per second.  To use, declare a Timer1 interrupt
	//  handler in your application and simply call this method from it (and nothing else.)
	//
	//  extern "C" INTERRUPT void		IRQ_Timer1(void)
	//  {
	//    audioBlock.processAudioInterrupt();
	//  }
	//
	void					processAudioInterrupt(void);
	
	//at 48kHz 16-bit stereo, a 10ms buffer (default) is almost 2KB. Use sparingly!
	bool					start(		int sampleRate = 48000,
										int bitsPerSample = 16,
										ChannelMode channelMode = Stereo,
										int bufferLength_ms = 10
									);
	
	inline void				end(void)	{start(0);}
	
	int						playSamples(Buffer b);
	int						playSamples(void const* samples, int length);
	
	//triggers when the sample buffer is half depleted
	Task					needSamplesTask(void);
	
private:
	void					requestSamples(void);
	
	Task					_needSamples;
	CircularBuffer*			_buffer;
	ChannelMode				_mode;
	byte					_bytesPerSample;
	byte					_audioAppBoard;
};

extern AudioBlock audioBlock;

} } //namespace Logiblock::AppBoards

#endif //__LOGIBLOCK_AUDIOBLOCK_H__
