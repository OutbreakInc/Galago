#include <GalagoAPI.h>
using namespace Galago;

int main(void)
{
	io.spi.start(2000000, IO::SPI::CharsAre9Bit);
	
	unsigned short words[3] = {0, '\r', '\n'};
	unsigned short count = 0;
	while(true)
	{
		words[0] = count;
		count++;
		
		io.spi.write(words, 3);
		
		system.wait(system.delay(10));
	}
}
