#include <GalagoAPI.h>
#include <AppBoard.h>
#include <BlueBlock.h>

using namespace Galago;
using namespace Logiblock;
using namespace Logiblock::AppBoards;

struct BlinkState
{
			BlinkState():
				on(true),
				enabled(true)
				{}
	
	bool	enabled;
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
	
	if(state->enabled)
		state->on = !state->on;
	
	io.serial.write("Test\n");
	
	system.when(system.delay(500), &blinkLED, p);
}

void onBytesReceived(void* p, Task t, bool success)
{
	BlinkState* state = (BlinkState*)p;
	
	bool prevState = state->on;
	char c;
	
	//read bytes one-at-a-time and change control state accordingly
	while(io.serial.read(&c, 1) > 0)
	{
		switch(c)
		{
		case '1':	// 1 enables blinking
			state->enabled = true;
			break;
		case '0':	// 0 disables it
			state->enabled = false;
			break;
		case 'x':	// x turns the LED on
			state->on = true;
			break;
		case 'z':	// z turns it off
			state->on = false;
			break;
		}
	}
	
	if(prevState != state->on)
		blueBlock.setLED(state->on);
	
	system.when(io.serial.bytesReceived(), &onBytesReceived, p);
}

int main(void)
{
	system.wait(system.delay(5));	//let power stabilize
	
	io.serial.start(9600);
	
	BlinkState state;
	state.on = true;

	system.when(io.serial.bytesReceived(), &onBytesReceived, &state);

	blinkLED(&state);
	
	while(true)
		system.sleep();
}
