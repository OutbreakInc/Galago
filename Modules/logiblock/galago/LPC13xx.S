/*////////////////////////////////////////////////////////////////
//
//	This file is valid and maintained but no longer used.
//  Its replacement is LPC13xx.cpp
//
////////////////////////////////////////////////////////////////*/

#include "LPC13xx.h"

#define STACK_TOP (MEMORY_SRAM_TOP - 4)

	.code	16
	.syntax unified
	.text
	
	
	.global _vectors
	.thumb_func
	.global _start
	.type	_start, %function
	.global _HardFault
	.type	_HardFault, %function
	.global _SVCall
	.type	_SVCall, %function
	.global _Sleep
	.type	_Sleep, %function
	.extern _mainCRTStartup
	.type	_mainCRTStartup, %function
	.extern _InternalIRQ_SysTick
	.type	_InternalIRQ_SysTick, %function
	.extern main
	.type	main, %function

	//weakly linked IRQ handlers
	.extern IRQ_WakeupPIO0_0
	.extern IRQ_WakeupPIO0_1
	.extern IRQ_WakeupPIO0_2
	.extern IRQ_WakeupPIO0_3
	.extern IRQ_WakeupPIO0_4
	.extern IRQ_WakeupPIO0_5
	.extern IRQ_WakeupPIO0_6
	.extern IRQ_WakeupPIO0_7
	.extern IRQ_WakeupPIO0_8
	.extern IRQ_WakeupPIO0_9
	.extern IRQ_WakeupPIO0_10
	.extern IRQ_WakeupPIO0_11
	.extern IRQ_WakeupPIO1_0
	.extern IRQ_WakeupPIO1_1
	.extern IRQ_WakeupPIO1_2
	.extern IRQ_WakeupPIO1_3
	.extern IRQ_WakeupPIO1_4
	.extern IRQ_WakeupPIO1_5
	.extern IRQ_WakeupPIO1_6
	.extern IRQ_WakeupPIO1_7
	.extern IRQ_WakeupPIO1_8
	.extern IRQ_WakeupPIO1_9
	.extern IRQ_WakeupPIO1_10
	.extern IRQ_WakeupPIO1_11
	.extern IRQ_WakeupPIO2_0
	.extern IRQ_WakeupPIO2_1
	.extern IRQ_WakeupPIO2_2
	.extern IRQ_WakeupPIO2_3
	.extern IRQ_WakeupPIO2_4
	.extern IRQ_WakeupPIO2_5
	.extern IRQ_WakeupPIO2_6
	.extern IRQ_WakeupPIO2_7
	.extern IRQ_WakeupPIO2_8
	.extern IRQ_WakeupPIO2_9
	.extern IRQ_WakeupPIO2_10
	.extern IRQ_WakeupPIO2_11
	.extern IRQ_WakeupPIO3_0
	.extern IRQ_WakeupPIO3_1
	.extern IRQ_WakeupPIO3_2
	.extern IRQ_WakeupPIO3_3
	//.extern IRQ_CAN
	//.extern IRQ_SPI1
	.extern IRQ_I2C
	.extern IRQ_Timer16_0
	.extern IRQ_Timer16_1
	.extern IRQ_Timer32_0
	.extern IRQ_Timer32_1
	.extern IRQ_SPI0
	.extern IRQ_UART
	.extern IRQ_USB_IRQ
	.extern IRQ_USB_FIQ
	.extern IRQ_ADC
	.extern IRQ_Watchdog
	.extern IRQ_Brownout
	//reserved
	.extern IRQ_GPIO_3
	.extern IRQ_GPIO_2
	.extern IRQ_GPIO_1
	.extern IRQ_GPIO_0
	.extern IRQ_SPI1
	
	
	//exception vector table
	#.org	0x00000000
	.section .isr_vector
	.balign  2
	.type	_vectors, %object
	.size	_vectors, 0x40

_vectors:

	//this is present as a debugging aid, where gdb will show addresses at 0x0 as "Null"
	.global Null
	.type	Null, %object
	.size	Null, 0x4
Null:

	//exceptions
	.word 	STACK_TOP								//stack top
	.word 	_start + 1								//Reset
	.word 	0										//NMI (not implemented on LPC11xx)
	.word 	_HardFault + 1							//HardFault
	.word 	0										//Reserved
	.word 	0										// "
	.word 	0										// "
	.word 	(-STACK_TOP - 0x129 - _HardFault + 0x1B)	// ", inverse checksum of vector table entries 0-6, such that the (sum % 2^32) of the first 8 words is 0. Sneaky method here.
	.word 	0										//Reserved
	.word 	0										// "
	.word 	0										// "
	.word 	_SVCall + 1								//SVCall
	.word 	0										//Reserved
	.word 	0										// "
	.word 	0										//PendSV
	.word 	_InternalIRQ_SysTick					//SysTick

	//interrupt (IRQ) vectors
	
	.word 	IRQ_WakeupPIO0_0		//16
	.word 	IRQ_WakeupPIO0_1
	.word 	IRQ_WakeupPIO0_2
	.word 	IRQ_WakeupPIO0_3
	.word 	IRQ_WakeupPIO0_4
	.word 	IRQ_WakeupPIO0_5
	.word 	IRQ_WakeupPIO0_6
	.word 	IRQ_WakeupPIO0_7
	.word 	IRQ_WakeupPIO0_8
	.word 	IRQ_WakeupPIO0_9
	
	.word 	IRQ_WakeupPIO0_10
	.word 	IRQ_WakeupPIO0_11
	.word 	IRQ_WakeupPIO1_0
	.word 	IRQ_WakeupPIO1_1
	.word 	IRQ_WakeupPIO1_2
	.word 	IRQ_WakeupPIO1_3
	.word 	IRQ_WakeupPIO1_4		//32
	.word 	IRQ_WakeupPIO1_5
	.word 	IRQ_WakeupPIO1_6
	.word 	IRQ_WakeupPIO1_7
	
	.word 	IRQ_WakeupPIO1_8
	.word 	IRQ_WakeupPIO1_9
	.word 	IRQ_WakeupPIO1_10
	.word 	IRQ_WakeupPIO1_11
	.word 	IRQ_WakeupPIO2_0
	.word 	IRQ_WakeupPIO2_1
	.word 	IRQ_WakeupPIO2_2
	.word 	IRQ_WakeupPIO2_3
	.word 	IRQ_WakeupPIO2_4
	.word 	IRQ_WakeupPIO2_5
	
	.word 	IRQ_WakeupPIO2_6
	.word 	IRQ_WakeupPIO2_7
	.word 	IRQ_WakeupPIO2_8		//48
	.word 	IRQ_WakeupPIO2_9
	.word 	IRQ_WakeupPIO2_10
	.word 	IRQ_WakeupPIO2_11
	.word 	IRQ_WakeupPIO3_0
	.word 	IRQ_WakeupPIO3_1
	.word 	IRQ_WakeupPIO3_2
	.word 	IRQ_WakeupPIO3_3
	
	.word 	IRQ_I2C					//56
	.word 	IRQ_Timer16_0
	.word 	IRQ_Timer16_1
	.word 	IRQ_Timer32_0
	.word 	IRQ_Timer32_1
	.word 	IRQ_SPI0
	.word 	IRQ_UART
	.word 	IRQ_USB_IRQ			//no function on non-USB-capable parts
	.word 	IRQ_USB_FIQ			// "	64
	.word 	IRQ_ADC
	
	.word 	IRQ_Watchdog
	.word 	IRQ_Brownout
	.word 	0					//reserved
	.word 	IRQ_GPIO_3
	.word 	IRQ_GPIO_2
	.word 	IRQ_GPIO_1
	.word 	IRQ_GPIO_0
	.word 	IRQ_SPI1			//no function on 33-pin ICs
	
	//reset entrypoint
	//.org	0x000000E0
	.code 	16
	.balign  2
	.thumb_func
	.type	_gaunt_start, %function

_gaunt_start:
	//the CPU has already loaded MSP with flash[0x00000000]
	
	cpsid		i				//disable interrupts
	mov			r0,		sp
	movs		r1,		#2
	msr			CONTROL, r1		//switch to Process SP
	mov			sp,		r0		//set PSP = MSP
	movs		r2,		#0
	msr			CONTROL, r2		//switch back to Main SP
	isb.w						//invoke an instuction sync barrier to ensure subsequent stack usage is correct
	bl			_mainCRTStartup
	
freeze:
	cpsid		i	//disable interrupts
	bkpt		1	//@@need constants
	b			freeze
	
	
	//HardFault exception handler
	.code 	16
	.balign  2
	.thumb_func
	.type	_HardFault, %function
	
_HardFault:
	cpsid		i	//disable interrupts
	bkpt		2	//@@need constants
	b			_HardFault
	
	
	//SVCall exception handler
	.code 	16
	.balign  2
	.thumb_func
	.type	_SVCall, %function
	
_SVCall:
	cpsid		i	//disable interrupts
	bkpt		3	//@@need constants
	b			_SVCall
	
	
	
	
	
	//Sleep function
	.code 	16
	.balign  2
	.thumb_func
	.type	_Sleep, %function
	
_Sleep:
	//@@write PMU state
	
	wfi
	
	//@@restore PMU state
	bx			lr

////////////////////////////////

	.text
	.balign	2
	.global	ignoreInterrupt
	.code	16
	.thumb_func
	.type	ignoreInterrupt, %function
	
ignoreInterrupt:
	bx		lr
	//mrs			r0,		ipsr			
	//movs		r1,		#1				
	//lsls		r1,		r0				
	//ldr			r0,		=0xE000E180		
	//str			r1,		[r0, #0]		
	
	.code	16
	.size	ignoreInterrupt, .-ignoreInterrupt
	.weak	_InternalIRQ_SysTick
	.thumb_set _InternalIRQ_SysTick,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_0
	.thumb_set IRQ_WakeupPIO0_0,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_1
	.thumb_set IRQ_WakeupPIO0_1,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_2
	.thumb_set IRQ_WakeupPIO0_2,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_3
	.thumb_set IRQ_WakeupPIO0_3,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_4
	.thumb_set IRQ_WakeupPIO0_4,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_5
	.thumb_set IRQ_WakeupPIO0_5,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_6
	.thumb_set IRQ_WakeupPIO0_6,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_7
	.thumb_set IRQ_WakeupPIO0_7,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_8
	.thumb_set IRQ_WakeupPIO0_8,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_9
	.thumb_set IRQ_WakeupPIO0_9,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_10
	.thumb_set IRQ_WakeupPIO0_10,ignoreInterrupt
	.weak	IRQ_WakeupPIO0_11
	.thumb_set IRQ_WakeupPIO0_11,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_0
	.thumb_set IRQ_WakeupPIO1_0,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_1
	.thumb_set IRQ_WakeupPIO1_1,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_2
	.thumb_set IRQ_WakeupPIO1_2,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_3
	.thumb_set IRQ_WakeupPIO1_3,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_4
	.thumb_set IRQ_WakeupPIO1_4,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_5
	.thumb_set IRQ_WakeupPIO1_5,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_6
	.thumb_set IRQ_WakeupPIO1_6,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_7
	.thumb_set IRQ_WakeupPIO1_7,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_8
	.thumb_set IRQ_WakeupPIO1_8,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_9
	.thumb_set IRQ_WakeupPIO1_9,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_10
	.thumb_set IRQ_WakeupPIO1_10,ignoreInterrupt
	.weak	IRQ_WakeupPIO1_11
	.thumb_set IRQ_WakeupPIO1_11,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_0
	.thumb_set IRQ_WakeupPIO2_0,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_1
	.thumb_set IRQ_WakeupPIO2_1,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_2
	.thumb_set IRQ_WakeupPIO2_2,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_3
	.thumb_set IRQ_WakeupPIO2_3,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_4
	.thumb_set IRQ_WakeupPIO2_4,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_5
	.thumb_set IRQ_WakeupPIO2_5,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_6
	.thumb_set IRQ_WakeupPIO2_6,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_7
	.thumb_set IRQ_WakeupPIO2_7,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_8
	.thumb_set IRQ_WakeupPIO2_8,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_9
	.thumb_set IRQ_WakeupPIO2_9,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_10
	.thumb_set IRQ_WakeupPIO2_10,ignoreInterrupt
	.weak	IRQ_WakeupPIO2_11
	.thumb_set IRQ_WakeupPIO2_11,ignoreInterrupt
	.weak	IRQ_WakeupPIO3_0
	.thumb_set IRQ_WakeupPIO3_0,ignoreInterrupt
	.weak	IRQ_WakeupPIO3_1
	.thumb_set IRQ_WakeupPIO3_1,ignoreInterrupt
	.weak	IRQ_WakeupPIO3_2
	.thumb_set IRQ_WakeupPIO3_2,ignoreInterrupt
	.weak	IRQ_WakeupPIO3_3
	.thumb_set IRQ_WakeupPIO3_3,ignoreInterrupt
	.weak	IRQ_CAN
	.thumb_set IRQ_CAN,ignoreInterrupt
	.weak	IRQ_SPI1
	.thumb_set IRQ_SPI1,ignoreInterrupt
	.weak	IRQ_I2C
	.thumb_set IRQ_I2C,ignoreInterrupt
	.weak	IRQ_Timer16_0
	.thumb_set IRQ_Timer16_0,ignoreInterrupt
	.weak	IRQ_Timer16_1
	.thumb_set IRQ_Timer16_1,ignoreInterrupt
	.weak	IRQ_Timer32_0
	.thumb_set IRQ_Timer32_0,ignoreInterrupt
	.weak	IRQ_Timer32_1
	.thumb_set IRQ_Timer32_1,ignoreInterrupt
	.weak	IRQ_SPI0
	.thumb_set IRQ_SPI0,ignoreInterrupt
	.weak	IRQ_UART
	.thumb_set IRQ_UART,ignoreInterrupt
	.weak	IRQ_USB_IRQ
	.thumb_set IRQ_USB_IRQ,ignoreInterrupt
	.weak	IRQ_USB_FIQ
	.thumb_set IRQ_USB_FIQ,ignoreInterrupt
	.weak	IRQ_ADC
	.thumb_set IRQ_ADC,ignoreInterrupt
	.weak	IRQ_Watchdog
	.thumb_set IRQ_Watchdog,ignoreInterrupt
	.weak	IRQ_Brownout
	.thumb_set IRQ_Brownout,ignoreInterrupt
	.weak	IRQ_GPIO_3
	.thumb_set IRQ_GPIO_3,ignoreInterrupt
	.weak	IRQ_GPIO_2
	.thumb_set IRQ_GPIO_2,ignoreInterrupt
	.weak	IRQ_GPIO_1
	.thumb_set IRQ_GPIO_1,ignoreInterrupt
	.weak	IRQ_GPIO_0
	.thumb_set IRQ_GPIO_0,ignoreInterrupt
	.weak	IRQ_SPI1
	.thumb_set IRQ_SPI1,ignoreInterrupt
	
	.weak	_start
	.thumb_set _start,_gaunt_start
	.weak	_mainCRTStartup
	.thumb_set _mainCRTStartup,_mainNoCRTStartup

////////////////////////////////
	.text
	.balign	2
	.global	_mainNoCRTStartup
	.code	16
	.thumb_func
	.type	_mainNoCRTStartup, %function
	
_mainNoCRTStartup:
	push	{r4-r6,lr}
	ldr		r4,		=__init_end
	ldr		r5,		=__init_start
Lnext:
	cmp		r4, r5
	beq.n	Ldone
	subs	r4,		4
	ldr		r6, [r4, #0]
	blx		r6
	b.n		Lnext
Ldone:
	bl		main
	b		freeze	//must not return from main
	.balign	4

////////////////////////////////
