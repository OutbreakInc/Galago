#include <GalagoAPI.h>
#include <AudioBlock.h>
#include <LPC13xx.h>

using namespace Galago;
using namespace Logiblock::AppBoards;

extern "C" INTERRUPT void		IRQ_Timer1(void)
{
	audioBlock.processAudioInterrupt();	//very time-sensitive!
}

int main(void)
{
	while(true)
		system.sleep();
}
