#include <GalagoAPI.h>
#include <AppBoard.h>
#include <Explorer.h>

using namespace Galago;
using namespace Logiblock;
using namespace Logiblock::AppBoards;

void	onGPSFixData(void*, Task, bool)
{
	io.led = !io.led;

	system.when(explorer.newGPSDataReady(), onGPSFixData);
}

void	onBytesReceived(void*, Task, bool)
{
	char c;
	while(io.serial.read(&c, 1))
		explorer.processGPSData(c);

	system.when(io.serial.bytesReceived(), onBytesReceived);
}

void	updateScreen(void*, Task, bool)
{
	io.serial.write(explorer.latitude());
	io.serial.write(explorer.longitude());
}

void	updateAccel(void*, Task, bool)
{
	system.when(explorer.updateAccelerometer(), updateScreen);
	system.when(system.delay(1000), updateAccel);
}

int		main(void)
{
	io.serial.start(9600);
	io.i2c.start();

	appBoard.reset();
	appBoard.detect();
	explorer.init();

	explorer.enableNMEAData(true);
	
	system.when(explorer.newGPSDataReady(), onGPSFixData);

	system.when(io.serial.bytesReceived(), onBytesReceived);

	system.when(system.delay(1000), updateAccel);

	while(true)
		system.sleep();
}