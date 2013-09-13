#include <GalagoAPI.h>
#include <LPC13xx.h>
#include <AppBoard.h>
#include <LEDBlock.h>

using namespace Galago;
using namespace Logiblock;
using namespace Logiblock::AppBoards;

INTERRUPT void	IRQ_Timer1(void)
{
	ledBlock.pulseLatch();
}

struct Context
{
	byte const* data;
	int offset;
};
void			sendData(void* p = 0, Task t = Task(), bool b = false)
{
	Context* c = (Context*)p;
	
	if(!ledBlock.select())
	{
		appBoard.reset();
		appBoard.detect();
		
		ledBlock.init();
		
		ledBlock.start();
		
		ledBlock.enableTimingSignals();
		
		ledBlock.select();
	}
	
	io.spi.write(c->data + c->offset, 24);
	ledBlock.end();
	
	c->offset = (c->offset)? 0 : 24;
	
	system.when(system.delay(1000), &sendData, p);
}

int main(void)
{
	io.spi.start(2000000);
	
	
	Context c;
	c.data = (byte const*)	"\xFF\xF0\x00\xFF\xF0\x00\xFF\xF0\x00\xFF\xF0\x00\xFF\xF0\x00\xFF\xF0\x00\xFF\xF0\x00\xFF\xF0\x00"
							"\x00\x0F\xFF\x00\x0F\xFF\x00\x0F\xFF\x00\x0F\xFF\x00\x0F\xFF\x00\x0F\xFF\x00\x0F\xFF\x00\x0F\xFF";
	c.offset = 0;
	sendData(&c);
	
	while(true)
		system.sleep();
}
