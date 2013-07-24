#include <GalagoAPI.h>

unsing namespace Galago;

namespace Logiblock { namespace AppBoards {

class Shield
{
public:
	IO::Pin	p0_rx;
	IO::Pin	p1_tx;
	IO::Pin	p2;
	IO::Pin	p3;
	IO::Pin	p4;
	IO::Pin	p5;
	IO::Pin	p6;
	IO::Pin	p7;
	IO::Pin	p8;
	IO::Pin	p9;
	IO::Pin	p10;
	IO::Pin	p11;
	IO::Pin	p12;
	IO::Pin	p13;

	IO::Pin	scl;
	IO::Pin	sda;
	IO::Pin	a3;
	IO::Pin	a2;
	IO::Pin	a1;
	IO::Pin	a0;

	IO::Pin	rst;

			Shield(void);

	void	init(void);
};

extern Shield shield;

}} //ns