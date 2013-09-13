#include <GalagoAPI.h>
#include <AppBoard.h>
#include <BlueBlock.h>

using namespace Galago;
using namespace Logiblock;
using namespace Logiblock::AppBoards;

struct BlinkState
{
			BlinkState():
				on(true)
				{}
	
	bool	on;
};

void	blinkLED(void* p, Task t = Task(), bool b = true)
{
	BlinkState* state = (BlinkState*)p;
	
	if(!blueBlock.setLED(state->on))
	{
		appBoard.reset();
		appBoard.detect();
		
		blueBlock.init();
		
		blueBlock.setLED(state->on);
		
		blueBlock.enableBluetooth();
	}
	
	state->on = !state->on;
	
	io.serial.write("Test\n");
	
	system.when(system.delay(500), &blinkLED, p);
}

int main(void)
{
	system.wait(system.delay(5));	//let power stabilize
	
	io.serial.start(9600);
	
	BlinkState state;
	state.on = true;
	blinkLED(&state);
	
	while(true)
		system.sleep();
}
