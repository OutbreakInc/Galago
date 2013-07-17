#include <GalagoAPI.h>
#include "shield.h"

using namespace Galago;
using namespace Logiblock::AppBoards;

namespace Logiblock { namespace AppBoards {

Shield shield;


		Shield::Shield(void)
{
	init();
}

void	Shield::init(void)
{
	p0_rx.bind(io.rxd);
	p1_tx.bind(io.rxd);
	p2.bind(io.p2);
	p3.bind(io.p1);
	p4.bind(io.p3);
	p5.bind(io.p5);
	p6.bind(io.a5);
	p7.bind(io.p4);
	p8.bind(io.p6);
	p9.bind(io.sel);
	p10.bind(io.a7);
	p11.bind(io.mosi);
	p12.bind(io.miso);
	p13.bind(io.sck);

	scl.bind(io.scl);
	sda.bind(io.sda);
	a3.bind(io.a3);
	a2.bind(io.a2);
	a1.bind(io.a1);
	a0.bind(io.a0);

	rst.bind(io.p0);
	rst.setMode(IO::Pin::Reset);	//like the Arduino
}

}} //ns
