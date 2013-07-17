#include <LPC13xx.h>

#define STACK_TOP (MEMORY_SRAM_TOP - 4)	//@@todo: get from linker script

#define WEAK_IGNORE __attribute__ ((weak, alias("ignoreInterrupt")))
#define STARTUP	__attribute__ ((section(".startup"), used))

extern "C" int main(void);

extern "C" void ignoreInterrupt(void) INTERRUPT;

extern "C" void _start(void) __attribute__ ((weak, alias("_gaunt_start"), used));
extern "C" void _gaunt_start(void);
extern "C" void _HardFault(void) INTERRUPT;
extern "C" void _SVCall(void) WEAK_IGNORE;
extern "C" void _InternalIRQ_SysTick(void) WEAK_IGNORE;


extern "C" void IRQ_WakeupPIO0_0(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_1(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_2(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_3(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_4(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_5(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_6(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_7(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_8(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_9(void) WEAK_IGNORE;

extern "C" void IRQ_WakeupPIO0_10(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO0_11(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_0(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_1(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_2(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_3(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_4(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_5(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_6(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_7(void) WEAK_IGNORE;

extern "C" void IRQ_WakeupPIO1_8(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_9(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_10(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO1_11(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_0(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_1(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_2(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_3(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_4(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_5(void) WEAK_IGNORE;

extern "C" void IRQ_WakeupPIO2_6(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_7(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_8(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_9(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_10(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO2_11(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO3_0(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO3_1(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO3_2(void) WEAK_IGNORE;
extern "C" void IRQ_WakeupPIO3_3(void) WEAK_IGNORE;

extern "C" void IRQ_I2C(void) WEAK_IGNORE;
extern "C" void IRQ_Timer16_0(void) WEAK_IGNORE;
extern "C" void IRQ_Timer16_1(void) WEAK_IGNORE;
extern "C" void IRQ_Timer32_0(void) WEAK_IGNORE;
extern "C" void IRQ_Timer32_1(void) WEAK_IGNORE;
extern "C" void IRQ_SPI0(void) WEAK_IGNORE;
extern "C" void IRQ_UART(void) WEAK_IGNORE;
extern "C" void IRQ_USB_IRQ(void) WEAK_IGNORE;
extern "C" void IRQ_USB_FIQ(void) WEAK_IGNORE;
extern "C" void IRQ_ADC(void) WEAK_IGNORE;

extern "C" void IRQ_Watchdog(void) WEAK_IGNORE;
extern "C" void IRQ_Brownout(void) WEAK_IGNORE;

extern "C" void IRQ_GPIO_3(void) WEAK_IGNORE;
extern "C" void IRQ_GPIO_2(void) WEAK_IGNORE;
extern "C" void IRQ_GPIO_1(void) WEAK_IGNORE;
extern "C" void IRQ_GPIO_0(void) WEAK_IGNORE;
extern "C" void IRQ_SPI1(void) WEAK_IGNORE;


typedef void (*IRQVector)(void);

//this is named "null" as a debugging enhancement and because nothing should depend on this symbol
extern "C"
IRQVector const null[] __attribute__ ((section(".isr_vector"), used)) =
{
	(IRQVector)STACK_TOP,	// Initial stack pointer value
	&_start,				// Reset vector
	0,						// NMI (not present on this chip)
	&_HardFault,			// Hard fault (bus fault etc.)
	0,						// Reserved
	0,						//  "
	0,						//  "
	(IRQVector)0xFFFFFFFF,	// Checksum, populated by post-build step
	0,						// Reserved
	0,						//  "
	0,						//  "
	&_SVCall,				// Supervisor call
	0,						// Reserved
	0,						//  "
	0,						// Pending supervisor call
	&_InternalIRQ_SysTick,	// Systick interrupt
	
	
	&IRQ_WakeupPIO0_0,
	&IRQ_WakeupPIO0_1,
	&IRQ_WakeupPIO0_2,
	&IRQ_WakeupPIO0_3,
	&IRQ_WakeupPIO0_4,
	&IRQ_WakeupPIO0_5,
	&IRQ_WakeupPIO0_6,
	&IRQ_WakeupPIO0_7,
	&IRQ_WakeupPIO0_8,
	&IRQ_WakeupPIO0_9,

	&IRQ_WakeupPIO0_10,
	&IRQ_WakeupPIO0_11,
	&IRQ_WakeupPIO1_0,
	&IRQ_WakeupPIO1_1,
	&IRQ_WakeupPIO1_2,
	&IRQ_WakeupPIO1_3,
	&IRQ_WakeupPIO1_4,
	&IRQ_WakeupPIO1_5,
	&IRQ_WakeupPIO1_6,
	&IRQ_WakeupPIO1_7,

	&IRQ_WakeupPIO1_8,
	&IRQ_WakeupPIO1_9,
	&IRQ_WakeupPIO1_10,
	&IRQ_WakeupPIO1_11,
	&IRQ_WakeupPIO2_0,
	&IRQ_WakeupPIO2_1,
	&IRQ_WakeupPIO2_2,
	&IRQ_WakeupPIO2_3,
	&IRQ_WakeupPIO2_4,
	&IRQ_WakeupPIO2_5,

	&IRQ_WakeupPIO2_6,
	&IRQ_WakeupPIO2_7,
	&IRQ_WakeupPIO2_8,
	&IRQ_WakeupPIO2_9,
	&IRQ_WakeupPIO2_10,
	&IRQ_WakeupPIO2_11,
	&IRQ_WakeupPIO3_0,
	&IRQ_WakeupPIO3_1,
	&IRQ_WakeupPIO3_2,
	&IRQ_WakeupPIO3_3,

	&IRQ_I2C,
	&IRQ_Timer16_0,
	&IRQ_Timer16_1,
	&IRQ_Timer32_0,
	&IRQ_Timer32_1,
	&IRQ_SPI0,
	&IRQ_UART,
	&IRQ_USB_IRQ,
	&IRQ_USB_FIQ,
	&IRQ_ADC,

	&IRQ_Watchdog,
	&IRQ_Brownout,
	0,
	&IRQ_GPIO_3,
	&IRQ_GPIO_2,
	&IRQ_GPIO_1,
	&IRQ_GPIO_0,
	&IRQ_SPI1
};

extern "C" void STARTUP __attribute__((naked)) _gaunt_start(void)
{
	__asm__ volatile (
	"cpsid		i				\n"		//disable interrupts
	"mov		r0,		#0		\n"
	"ldr		r1,		[r0, #0]\n"		//$sp = *(void**)0	//load SP from address 0. Yes, the hardware does this, but not
	"mov		sp,		r1		\n"							//  if you manually vector to _start during e.g. debugging or soft-reset
	"movs		r2,		#2		\n"
	"msr		CONTROL, r2		\n"		//switch to Process SP
	"mov		sp,		r1		\n"		//set PSP = MSP
	"msr		CONTROL, r0		\n"		//switch back to Main SP
	"ldr		r0,		=0x40048000\n"	//there are some odd scenarios (noticed during debugging) where the ROM bootloader/seed doesn't
	"str		r2,		[r0, #0]\n"		//  set this correctly.
	"isb.w						\n"		//invoke an instruction sync barrier to ensure subsequent stack usage is correct
	"dsb.w						\n"		//a data sync barrier ensures stack-based execution and IO relative to the stack is ok
	
	//annoyingly, I couldn't get this to generate correct asm, hence the inline asm version below
	// void (**p)(void) = __init_end;
	// while(p-- != __init_start)
	//   (*p)();
	
	"ldr		r4,		=__init_end		\n"
	"ldr		r5,		=__init_start	\n"
	"._start_next_ctor:					\n"
	"cmp		r4, r5					\n"
	"beq.n		._start_ctors_done		\n"
	"subs		r4,		4				\n"
	"ldr		r0, [r4, #0]			\n"
	"blx		r0						\n"		//r4 and r5 are chosen above because ATPCS guarantees they won't be trampled here
	"b.n		._start_next_ctor		\n"
	"._start_ctors_done:				\n"
	::: "r0", "r1", "r2", "r4", "r5");
	
	main();
	_HardFault();
}

extern "C" void STARTUP INTERRUPT __attribute__((naked)) _HardFault(void)
{
	//according to the ARM ARMv7-M A.R.M sec. B1-22, the NVIC stores the following
	//  values in these places when vectoring to a handler:
	//
	//  $sp[0] = $r0
	//  $sp[1] = $r1
	//  $sp[2] = $r2
	//  $sp[3] = $r3
	//  $sp[4] = $r12
	//  $sp[5] = $lr
	//  $sp[6] = (returnAddress)	<- in "precise" cases, the faulting address, else one instruction past it*
	//  $sp[7] = $cpsr
	
	//*(im)precision due to the nature of the instruction, execution pipeline and bus activity
	
	//moreover, these values are important:
	//  $sp = ($sp_at_fault - 32)
	//  $cpsr & 0xFF = (fault code)
	//  $lr = 0xFFFFFFF9, or in some cases 0xFFFFFFF1 or 0xFFFFFFFD
	
	//a fault occurred at or near $sp[6]
	
	unsigned int volatile register faultPC;
	unsigned int volatile register faultLR;
	
	__asm__ volatile (
	"ldr	%0, 	[sp, #24]			\n"
	"ldr	%1, 	[sp, #20]			\n"
	: "=r" (faultPC), "=r" (faultLR)
	:
	: );
	
	(void)faultPC;
	(void)faultLR;
	
	__asm__ volatile (
	"bx		lr							\n"
	);
}

/*
extern "C" void STARTUP INTERRUPT _SVCall(void)
{
}
*/

extern "C" void STARTUP Sleep(void)
{
	//@@enter PMU state
	__asm__ volatile ("wfi"::);	//chip is sleeping, waiting for the next event.
	//@@exit PMU state
}

extern "C" void STARTUP Reset(void)
{
	*((unsigned int volatile*)(0xE000ED0C)) = 0x05FA0004;	//invoke a hard reset
}

extern "C" void STARTUP INTERRUPT ignoreInterrupt(void)
{
}

extern void* __attribute__ ((weak, alias("__dso_handle_nostdlib"))) __dso_handle;
void* __dso_handle_nostdlib = 0;

extern "C" int __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
	return(0);	//firmware never exits
}