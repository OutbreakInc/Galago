#ifndef __LOGIBLOCK_AUDIOBLOCK_H__
#define __LOGIBLOCK_AUDIOBLOCK_H__

#include <GalagoAPI.h>
using namespace Galago;

namespace Logiblock { namespace AppBoards {

class AudioBlock
{
	friend void				IRQ_Timer16_0(void);
	
public:
							AudioBlock(void);
	void					init(void);
	
	typedef enum
	{
		Mono,
		Stereo,
		LeftOnly,
		RightOnly,
	} ChannelMode;
	
	//at 48kHz 16-bit stereo, a 10ms buffer (default) is almost 2KB. Use sparingly!
	void					begin(		int sampleRate = 48000,
										int bitsPerSample = 16,
										ChannelMode channelMode = Stereo,
										bufferLength_ms = 10
									);
	
	inline void				end(void)	{begin(0);}
	
	int						playSamples(Buffer b);
	int						playSamples(void const* samples);
	
	//triggers when the sample buffer is half depleted
	Task					needSamplesTask(void);
	
private:
	void					requestSamples(void);
	
	ChannelMode				_mode;
	CircularBuffer*			_buffer;
};

extern AudioBlock audioBlock;

} } //namespace Logiblock::AppBoards

#endif //__LOGIBLOCK_AUDIOBLOCK_H__
