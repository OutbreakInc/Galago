#ifndef __LPC1300_SERIES_INCLUDED__
#define __LPC1300_SERIES_INCLUDED__

	#if !defined(PART)
		#error PART not defined!  Must be one of the following literal values: {lpc1313, lpc1343}
	#elif PART == lpc1313	//32K Flash, 8K RAM
	#elif PART == lpc1343	//32K Flash, 8K RAM, USB
	#else
		#error PART must be one of the following literal values: {lpc1313, lpc1343}
	#endif

	#define		lpc1313_301		(0x13130000)
	#define		lpc1343_101		(0x13430000)

//References:
//	#UserManual = "UM10375 LPC13xx User manual", revision 2, 7 July 2010

////////////////////////////////////////////////////////////////
//Definition utilities
	
	#define RESERVED_bits(x) _RESERVED_b(u32, __LINE__, x)
	#define RESERVED_u8(x) _RESERVED_(u8, __LINE__, x)
	#define RESERVED_u32(x) _RESERVED_(u32, __LINE__, x)
	
	#define _RESERVED_(t, l, x) __RESERVED_(t, l, x)
	#define __RESERVED_(t, l, x) t __reserved_ ## l [x]
	
	#define _RESERVED_b(t, l, x) __RESERVED_b(t, l, x)
	#define __RESERVED_b(t, l, x) t __reserved_ ## l : x
	
////////////////////////////////////////////////////////////////
//Architectural definitions
	
	#ifndef __ASSEMBLER__
	
		#ifdef __cplusplus
			namespace LPC1300{
		#endif //c++
		
		typedef unsigned char		u8;
		typedef signed char			s8;
		typedef unsigned short		u16;
		typedef signed short		s16;
		typedef unsigned int		u32;
		typedef signed int			s32;
		typedef unsigned long long	u64;
		typedef signed long long	s64;
	
	#endif //!__ASSEMBLER__
	
	
	
////////////////////////////////////////////////////////////////
//Utility

	#define					_BIT(n)		(1 << (n))
	
	
////////////////////////////////////////////////////////////////
//Memory locations [see #UserManual pp.9]

		#define	MEMORY_FLASH_BOTTOM				(0x00000000)
		#define	MEMORY_SRAM_BOTTOM				(0x10000000)
	#if PART == lpc1313			//32K Flash, 8K RAM
		#define	MEMORY_SRAM_TOP					(0x10002000)
		#define	MEMORY_FLASH_TOP				(0x00008000)
	#elif PART == lpc1343		//32K Flash, 8K RAM, USB
		#define	MEMORY_SRAM_TOP					(0x10002000)
		#define	MEMORY_FLASH_TOP				(0x00008000)
	#else
		#error Cannot set memory definitions because PART is not defined!
	#endif

	#define REGISTER				u32 volatile* const
	#define REGISTER_ADDRESS(x)		((u32 volatile*)(x))
	
	#ifndef __ASSEMBLER__
	
	REGISTER	IOConfigPIO2_6 =		REGISTER_ADDRESS(0x40044000);
		enum IOConfigPIO2_6
		{
			IOConfigPIO2_6_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_6_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_6_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_6_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_6_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_0 =		REGISTER_ADDRESS(0x40044008);
		enum IOConfigPIO2_0
		{
			IOConfigPIO2_0_Function_PIO 	=	0x00,
			IOConfigPIO2_0_Function_nDTR 	=	0x01,
			IOConfigPIO2_0_Function_SSEL1 	=	0x02,
			
			IOConfigPIO2_0_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_0_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_0_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_0_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_0_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_0 =		REGISTER_ADDRESS(0x4004400C);
		enum IOConfigPIO0_0
		{
			IOConfigPIO0_0_Function_nRESET 	=	0x00,
			IOConfigPIO0_0_Function_PIO 	=	0x01,
			
			IOConfigPIO0_0_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_0_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_0_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_0_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_0_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_1 =		REGISTER_ADDRESS(0x40044010);
		enum IOConfigPIO0_1
		{
			IOConfigPIO0_1_Function_PIO 	=	0x00,
			IOConfigPIO0_1_Function_CLKOUT 	=	0x01,
			IOConfigPIO0_1_Function_T2_M2 	=	0x02,
			
			IOConfigPIO0_1_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_1_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_1_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_1_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_1_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_8 =		REGISTER_ADDRESS(0x40044014);
		enum IOConfigPIO1_8
		{
			IOConfigPIO1_8_Function_PIO 	=	0x00,
			IOConfigPIO1_8_Function_T1_CAP 	=	0x01,
			
			IOConfigPIO1_8_PullDown		 	=	(0x01 << 3),
			IOConfigPIO1_8_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_8_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_8_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_8_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_2 =		REGISTER_ADDRESS(0x4004401C);
		enum IOConfigPIO0_2
		{
			IOConfigPIO0_2_Function_PIO 	=	0x00,
			IOConfigPIO0_2_Function_SSEL0 	=	0x01,
			IOConfigPIO0_2_Function_T0_CAP 	=	0x02,
			
			IOConfigPIO0_2_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_2_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_2_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_2_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_2_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_7 =		REGISTER_ADDRESS(0x40044020);
		enum IOConfigPIO2_7
		{
			IOConfigPIO2_7_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_7_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_7_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_7_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_7_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_8 =		REGISTER_ADDRESS(0x40044024);
		enum IOConfigPIO2_8
		{
			IOConfigPIO2_8_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_8_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_8_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_8_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_8_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_1 =		REGISTER_ADDRESS(0x40044028);
		enum IOConfigPIO2_1
		{
			IOConfigPIO2_1_Function_PIO 	=	0x00,
			IOConfigPIO2_1_Function_nDSR 	=	0x01,
			IOConfigPIO2_1_Function_SCK1 	=	0x02,
			
			IOConfigPIO2_1_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_1_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_1_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_1_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_1_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_3 =		REGISTER_ADDRESS(0x4004402C);
		enum IOConfigPIO0_3
		{
			IOConfigPIO0_3_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_3_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_3_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_3_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_3_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_4 =		REGISTER_ADDRESS(0x40044030);
		enum IOConfigPIO0_4
		{
			IOConfigPIO0_4_Function_PIO 	=	0x00,
			IOConfigPIO0_4_Function_I2C_SCL	=	0x01,
			
			IOConfigPIO0_4_Mode_I2C		 	=	(0x00 << 8),
			IOConfigPIO0_4_Mode_PIO 		=	(0x01 << 8),
			IOConfigPIO0_4_Mode_I2C_Fast	=	(0x02 << 8),
		};
	REGISTER	IOConfigPIO0_5 =		REGISTER_ADDRESS(0x40044034);
		enum IOConfigPIO0_5
		{
			IOConfigPIO0_5_Function_PIO 	=	0x00,
			IOConfigPIO0_5_Function_I2C_SDA	=	0x01,
			
			IOConfigPIO0_5_Mode_I2C		 	=	(0x00 << 8),
			IOConfigPIO0_5_Mode_PIO 		=	(0x01 << 8),
			IOConfigPIO0_5_Mode_I2C_Fast	=	(0x02 << 8),
		};
	REGISTER	IOConfigPIO1_9 =		REGISTER_ADDRESS(0x40044038);
		enum IOConfigPIO1_9
		{
			IOConfigPIO1_9_Function_PIO 	=	0x00,
			IOConfigPIO1_9_Function_T1_M0 	=	0x01,
			
			IOConfigPIO1_9_PullDown		 	=	(0x01 << 3),
			IOConfigPIO1_9_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_9_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_9_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_9_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO3_4 =		REGISTER_ADDRESS(0x4004403C);
		enum IOConfigPIO3_4
		{
			IOConfigPIO3_4_PullDown		 	=	(0x01 << 3),
			IOConfigPIO3_4_PullUp 			=	(0x02 << 3),
			IOConfigPIO3_4_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO3_4_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO3_4_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_4 =		REGISTER_ADDRESS(0x40044040);
		enum IOConfigPIO2_4
		{
			IOConfigPIO2_4_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_4_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_4_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_4_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_4_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_5 =		REGISTER_ADDRESS(0x40044044);
		enum IOConfigPIO2_5
		{
			IOConfigPIO2_5_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_5_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_5_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_5_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_5_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO3_5 =		REGISTER_ADDRESS(0x40044048);
		enum IOConfigPIO3_5
		{
			IOConfigPIO3_5_PullDown		 	=	(0x01 << 3),
			IOConfigPIO3_5_PullUp 			=	(0x02 << 3),
			IOConfigPIO3_5_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO3_5_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO3_5_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_6 =		REGISTER_ADDRESS(0x4004404C);
		enum IOConfigPIO0_6
		{
			IOConfigPIO0_6_Function_PIO 	=	0x00,
			IOConfigPIO0_6_Function_SCK0 	=	0x02,
			
			IOConfigPIO0_6_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_6_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_6_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_6_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_6_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_7 =		REGISTER_ADDRESS(0x40044050);
		enum IOConfigPIO0_7
		{
			IOConfigPIO0_7_Function_PIO 	=	0x00,
			IOConfigPIO0_7_Function_nCTS 	=	0x01,
			
			IOConfigPIO0_7_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_7_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_7_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_7_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO20_7_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_9 =		REGISTER_ADDRESS(0x40044054);
		enum IOConfigPIO2_9
		{
			IOConfigPIO2_9_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_9_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_9_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_9_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_9_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_10 =		REGISTER_ADDRESS(0x40044058);
		enum IOConfigPIO2_10
		{
			IOConfigPIO2_10_PullDown		=	(0x01 << 3),
			IOConfigPIO2_10_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_10_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_10_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_10_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_2 =		REGISTER_ADDRESS(0x4004405C);
		enum IOConfigPIO2_2
		{
			IOConfigPIO2_2_Function_PIO 	=	0x00,
			IOConfigPIO2_2_Function_nDCD 	=	0x01,
			IOConfigPIO2_2_Function_MISO1 	=	0x02,
			
			IOConfigPIO2_2_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_2_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_2_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_2_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_2_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_8 =		REGISTER_ADDRESS(0x40044060);
		enum IOConfigPIO0_8
		{
			IOConfigPIO0_8_Function_PIO 	=	0x00,
			IOConfigPIO0_8_Function_MISO0 	=	0x01,
			IOConfigPIO0_8_Function_T0_M0 	=	0x02,
			
			IOConfigPIO0_8_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_8_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_8_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_8_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_8_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_9 =		REGISTER_ADDRESS(0x40044064);
		enum IOConfigPIO0_9
		{
			IOConfigPIO0_9_Function_PIO 	=	0x00,
			IOConfigPIO0_9_Function_MOSI0 	=	0x01,
			IOConfigPIO0_9_Function_T0_M1 	=	0x02,
			
			IOConfigPIO0_9_PullDown		 	=	(0x01 << 3),
			IOConfigPIO0_9_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_9_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_9_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_9_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_10 =		REGISTER_ADDRESS(0x40044068);
		enum IOConfigPIO0_10
		{
			IOConfigPIO0_10_Function_SWCLK 	=	0x00,
			IOConfigPIO0_10_Function_PIO 	=	0x01,
			IOConfigPIO0_10_Function_SCK0 	=	0x02,
			IOConfigPIO0_10_Function_T0_M2 	=	0x03,
			
			IOConfigPIO0_10_PullDown		=	(0x01 << 3),
			IOConfigPIO0_10_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_10_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_10_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_10_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_10 =		REGISTER_ADDRESS(0x4004406C);
		enum IOConfigPIO1_10
		{
			IOConfigPIO1_10_Function_PIO 	=	0x00,
			IOConfigPIO1_10_Function_AD6 	=	0x01,
			IOConfigPIO1_10_Function_T1_M1 	=	0x02,
			
			IOConfigPIO1_10_PullDown		=	(0x01 << 3),
			IOConfigPIO1_10_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_10_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_10_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_10_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_10_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_10_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_11 =		REGISTER_ADDRESS(0x40044070);
		enum IOConfigPIO2_11
		{
			IOConfigPIO2_11_Function_PIO 	=	0x00,
			IOConfigPIO2_11_Function_SCK0 	=	0x01,
			
			IOConfigPIO2_11_PullDown		=	(0x01 << 3),
			IOConfigPIO2_11_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_11_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_11_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_11_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO0_11 =		REGISTER_ADDRESS(0x40044074);
		enum IOConfigPIO0_11
		{
			IOConfigPIO0_11_Function_None 	=	0x00,
			IOConfigPIO0_11_Function_PIO 	=	0x01,
			IOConfigPIO0_11_Function_AD0 	=	0x02,
			IOConfigPIO0_11_Function_T2_M3 	=	0x03,
			
			IOConfigPIO0_11_PullDown		=	(0x01 << 3),
			IOConfigPIO0_11_PullUp 			=	(0x02 << 3),
			IOConfigPIO0_11_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO0_11_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO0_11_AnalogMode		=	(0x00 << 7),
			IOConfigPIO0_11_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO0_11_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_0 =		REGISTER_ADDRESS(0x40044078);
		enum IOConfigPIO1_0
		{
			IOConfigPIO1_0_Function_None 	=	0x00,
			IOConfigPIO1_0_Function_PIO 	=	0x01,
			IOConfigPIO1_0_Function_AD1 	=	0x02,
			IOConfigPIO1_0_Function_T3_CAP	=	0x03,
			
			IOConfigPIO1_0_PullDown			=	(0x01 << 3),
			IOConfigPIO1_0_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_0_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_0_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_0_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_0_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_0_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_1 =		REGISTER_ADDRESS(0x4004407C);
		enum IOConfigPIO1_1
		{
			IOConfigPIO1_1_Function_None 	=	0x00,
			IOConfigPIO1_1_Function_PIO 	=	0x01,
			IOConfigPIO1_1_Function_AD2 	=	0x02,
			IOConfigPIO1_1_Function_T3_M0	=	0x03,
			
			IOConfigPIO1_1_PullDown			=	(0x01 << 3),
			IOConfigPIO1_1_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_1_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_1_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_1_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_1_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_1_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_2 =		REGISTER_ADDRESS(0x40044080);
		enum IOConfigPIO1_2
		{
			IOConfigPIO1_2_Function_None 	=	0x00,
			IOConfigPIO1_2_Function_PIO 	=	0x01,
			IOConfigPIO1_2_Function_AD3 	=	0x02,
			IOConfigPIO1_2_Function_T3_M1	=	0x03,
			
			IOConfigPIO1_2_PullDown			=	(0x01 << 3),
			IOConfigPIO1_2_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_2_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_2_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_2_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_2_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_2_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO3_0 =		REGISTER_ADDRESS(0x40044084);
		enum IOConfigPIO3_0
		{
			IOConfigPIO3_0_Function_PIO 	=	0x00,
			IOConfigPIO3_0_Function_nDTR 	=	0x01,
			
			IOConfigPIO3_0_PullDown		 	=	(0x01 << 3),
			IOConfigPIO3_0_PullUp 			=	(0x02 << 3),
			IOConfigPIO3_0_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO3_0_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO3_0_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO3_1 =		REGISTER_ADDRESS(0x40044088);
		enum IOConfigPIO3_1
		{
			IOConfigPIO3_1_Function_PIO 	=	0x00,
			IOConfigPIO3_1_Function_nDSR 	=	0x01,
			
			IOConfigPIO3_1_PullDown		 	=	(0x01 << 3),
			IOConfigPIO3_1_PullUp 			=	(0x02 << 3),
			IOConfigPIO3_1_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO3_1_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO3_1_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO2_3 =		REGISTER_ADDRESS(0x4004408C);
		enum IOConfigPIO2_3
		{
			IOConfigPIO2_3_Function_PIO 	=	0x00,
			IOConfigPIO2_3_Function_nRI 	=	0x01,
			IOConfigPIO2_3_Function_MOSI1 	=	0x02,
			
			IOConfigPIO2_3_PullDown		 	=	(0x01 << 3),
			IOConfigPIO2_3_PullUp 			=	(0x02 << 3),
			IOConfigPIO2_3_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO2_3_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO2_3_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_3 =		REGISTER_ADDRESS(0x40044090);
		enum IOConfigPIO1_3
		{
			IOConfigPIO1_3_Function_SWDIO 	=	0x00,
			IOConfigPIO1_3_Function_PIO 	=	0x01,
			IOConfigPIO1_3_Function_AD4 	=	0x02,
			IOConfigPIO1_3_Function_T3_M2	=	0x03,
			
			IOConfigPIO1_3_PullDown			=	(0x01 << 3),
			IOConfigPIO1_3_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_3_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_3_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_3_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_3_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_3_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_4 =		REGISTER_ADDRESS(0x40044094);
		enum IOConfigPIO1_4
		{
			IOConfigPIO1_4_Function_PIO 	=	0x00,
			IOConfigPIO1_4_Function_AD5 	=	0x01,
			IOConfigPIO1_4_Function_T3_M3 	=	0x02,
			
			IOConfigPIO1_4_PullDown			=	(0x01 << 3),
			IOConfigPIO1_4_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_4_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_4_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_4_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_4_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_4_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_11 =		REGISTER_ADDRESS(0x40044098);
		enum IOConfigPIO1_11
		{
			IOConfigPIO1_11_Function_PIO 	=	0x00,
			IOConfigPIO1_11_Function_AD7 	=	0x01,
			
			IOConfigPIO1_11_PullDown		=	(0x01 << 3),
			IOConfigPIO1_11_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_11_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_11_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_11_AnalogMode		=	(0x00 << 7),
			IOConfigPIO1_11_DigitalMode		=	(0x01 << 7),
			
			IOConfigPIO1_11_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO3_2 =		REGISTER_ADDRESS(0x4004409C);
		enum IOConfigPIO3_2
		{
			IOConfigPIO3_2_Function_PIO 	=	0x00,
			IOConfigPIO3_2_Function_nDCD 	=	0x01,
			
			IOConfigPIO3_2_PullDown		 	=	(0x01 << 3),
			IOConfigPIO3_2_PullUp 			=	(0x02 << 3),
			IOConfigPIO3_2_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO3_2_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO3_2_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_5 =		REGISTER_ADDRESS(0x400440A0);
		enum IOConfigPIO1_5
		{
			IOConfigPIO1_5_Function_PIO 	=	0x00,
			IOConfigPIO1_5_Function_nRTS	=	0x01,
			IOConfigPIO1_5_Function_T2_CAP	=	0x02,
			
			IOConfigPIO1_5_PullDown		 	=	(0x01 << 3),
			IOConfigPIO1_5_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_5_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_5_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_5_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_6 =		REGISTER_ADDRESS(0x400440A4);
		enum IOConfigPIO1_6
		{
			IOConfigPIO1_6_Function_PIO 	=	0x00,
			IOConfigPIO1_6_Function_RXD		=	0x01,
			IOConfigPIO1_6_Function_T2_M0	=	0x02,
			
			IOConfigPIO1_6_PullDown		 	=	(0x01 << 3),
			IOConfigPIO1_6_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_6_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_6_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_6_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO1_7 =		REGISTER_ADDRESS(0x400440A8);
		enum IOConfigPIO1_7
		{
			IOConfigPIO1_7_Function_PIO 	=	0x00,
			IOConfigPIO1_7_Function_TXD		=	0x01,
			IOConfigPIO1_7_Function_T2_M1	=	0x02,
			
			IOConfigPIO1_7_PullDown		 	=	(0x01 << 3),
			IOConfigPIO1_7_PullUp 			=	(0x02 << 3),
			IOConfigPIO1_7_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO1_7_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO1_7_OpenDrain		=	(0x01 << 10),
		};
	REGISTER	IOConfigPIO3_3 =		REGISTER_ADDRESS(0x400440AC);
		enum IOConfigPIO3_3
		{
			IOConfigPIO3_3_Function_PIO 	=	0x00,
			IOConfigPIO3_3_Function_nRI 	=	0x01,
			
			IOConfigPIO3_3_PullDown		 	=	(0x01 << 3),
			IOConfigPIO3_3_PullUp 			=	(0x02 << 3),
			IOConfigPIO3_3_Repeat 			=	(0x03 << 3),
			
			IOConfigPIO3_3_Hysteresis		=	(0x01 << 5),
			
			IOConfigPIO3_3_OpenDrain		=	(0x01 << 10),
		};

	REGISTER	IOConfigSCKLocation =	REGISTER_ADDRESS(0x400440B0);
		enum IOConfigSCKLocation
		{
			IOConfigSCKLocation_PIO0_10_SWCLK 	=	0x00,
			IOConfigSCKLocation_PIO2_11		 	=	0x01,
			IOConfigSCKLocation_PIO0_6 			=	0x02,
		};
	REGISTER	IOConfignDSRLocation =	REGISTER_ADDRESS(0x400440B4);
		enum IOConfignDSRLocation
		{
			IOConfignDSRLocation_PIO2_1	 		=	0x00,
			IOConfignDSRLocation_PIO3_1	 		=	0x01,
		};
	REGISTER	IOConfignDCDLocation =	REGISTER_ADDRESS(0x400440B8);
		enum IOConfignDCDLocation
		{
			IOConfignDCDLocation_PIO2_2 		=	0x00,
			IOConfignDCDLocation_PIO3_2		 	=	0x01,
		};
	REGISTER	IOConfignRILocation =	REGISTER_ADDRESS(0x400440BC);
		enum IOConfignRILocation
		{
			IOConfignRILocation_PIO2_3		 	=	0x00,
			IOConfignRILocation_PIO3_3		 	=	0x01,
		};






	REGISTER	MemoryRemap =			REGISTER_ADDRESS(0x40048000);
		enum MemoryRemap
		{
			MemoryRemap_BootROM 	=	0x00,
			MemoryRemap_SRAM		= 	0x01,
			MemoryRemap_Flash 		=	0x02,
		};
	REGISTER	PeripheralnReset =		REGISTER_ADDRESS(0x40048004);
		enum PeripheralnReset
		{
			PeripheralnReset_SPI0 	=	0x01,
			PeripheralnReset_I2C	= 	0x02,
			PeripheralnReset_SPI1 	=	0x04,
			PeripheralnReset_CAN	= 	0x08,
		};
	//the system PLL takes a selectable input frequency and has transform function H(f) = f * ((M + 1) / 2^^(P + 1))
	REGISTER	PLLControl =			REGISTER_ADDRESS(0x40048008);
		enum PLLControl
		{
			PLLControl_MultiplierBitsMask	= 0x1F,	//value M, multiplication factor is (M + 1)
			PLLControl_DividerBitsMask		= 0x60,	//value P, division factor is 2^^(P + 1)
			PLLControl_DividerBitsShift		= 6,
		};
	REGISTER	PLLStatus =				REGISTER_ADDRESS(0x4004800C);
		enum PLLStatus
		{
			PLLStatus_Locked	= 0x01,
		};
	REGISTER	OscillatorControl =		REGISTER_ADDRESS(0x40048020);
		enum PLLSourceSelect
		{
			PLLSourceSelect_Normal		= 0x00,
			PLLSourceSelect_Bypassed	= 0x01,
			PLLSourceSelect_1_to_20MHz	= 0x00,
			PLLSourceSelect_15_to_25MHz	= 0x02,
		};
	REGISTER	WatchdogControl =			REGISTER_ADDRESS(0x40048024);
		enum WatchdogControl
		{
			WatchdogControl_Frequency_0_6MHz	= (0x01 << 5),
			WatchdogControl_Frequency_1_05MHz	= (0x02 << 5),
			WatchdogControl_Frequency_1_4MHz	= (0x03 << 5),
			WatchdogControl_Frequency_1_75MHz	= (0x04 << 5),
			WatchdogControl_Frequency_2_1MHz	= (0x05 << 5),
			WatchdogControl_Frequency_2_4MHz	= (0x06 << 5),
			WatchdogControl_Frequency_2_7MHz	= (0x07 << 5),
			WatchdogControl_Frequency_3_0MHz	= (0x08 << 5),
			WatchdogControl_Frequency_3_25MHz	= (0x09 << 5),
			WatchdogControl_Frequency_3_5MHz	= (0x0A << 5),
			WatchdogControl_Frequency_3_75MHz	= (0x0B << 5),
			WatchdogControl_Frequency_4_0MHz	= (0x0C << 5),
			WatchdogControl_Frequency_4_2MHz	= (0x0D << 5),
			WatchdogControl_Frequency_4_4MHz	= (0x0E << 5),
			WatchdogControl_Frequency_4_6MHz	= (0x0F << 5),
		};
	REGISTER	InternalCrystalTrim =		REGISTER_ADDRESS(0x40048028);
	REGISTER	ResetStatus =				REGISTER_ADDRESS(0x40048030);
		enum ResetStatus
		{
			ResetStatus_PowerOnReset			= 0x01,
			ResetStatus_ExternalReset			= 0x02,
			ResetStatus_WatchdogReset			= 0x04,
			ResetStatus_BrownoutReset			= 0x08,
			ResetStatus_SoftwareReset			= 0x10,
		};
	REGISTER	PLLSource =				REGISTER_ADDRESS(0x40048040);		//aka SYSPLLCLKSEL
		enum PLLSource
		{
			PLLSource_InternalCrystal	= 0x00,
			PLLSource_ExternalClock		= 0x01,
		};
	REGISTER	PLLSourceUpdate =			REGISTER_ADDRESS(0x40048044);	//aka SYSPLLUEN
		enum PLLSourceUpdate
		{
			PLLSourceUpdate_Enable		= 0x01,
		};
	REGISTER	MainClockSource =			REGISTER_ADDRESS(0x40048070);	//aka MAINCLKSEL
		enum MainClockSource
		{
			MainClockSource_InternalCrystal		= 0x00,
			MainClockSource_PLLInput			= 0x01,
			MainClockSource_WDTOscillator		= 0x02,
			MainClockSource_PLLOutput			= 0x03,
		};
	REGISTER	MainClockSourceUpdate =		REGISTER_ADDRESS(0x40048074);	//aka MAINCLKUEN
		enum MainClockSourceUpdate
		{
			MainClockSourceUpdate_Enable	= 0x01,
		};
	REGISTER	MainBusDivider =			REGISTER_ADDRESS(0x40048078);	//aka SYSAHBCLKDIV
	REGISTER	ClockControl =				REGISTER_ADDRESS(0x40048080);	//aka SYSAHBCLKCTRL
		enum ClockControl
		{
			ClockControl_Core				= 0x000001,	//* = enabled at power-up
			ClockControl_ROM				= 0x000002,	//*
			ClockControl_RAM				= 0x000004,	//*
			ClockControl_FlashRegisters		= 0x000008,	//*
			ClockControl_FlashStorage		= 0x000010,	//*
			ClockControl_I2C				= 0x000020,
			ClockControl_GPIO				= 0x000040,	//*
			ClockControl_Timer16_0			= 0x000080,
			ClockControl_Timer16_1			= 0x000100,
			ClockControl_Timer32_0			= 0x000200,
			ClockControl_Timer32_1			= 0x000400,
			ClockControl_SPI0				= 0x000800,	//*
			ClockControl_UART				= 0x001000,
			ClockControl_ADC				= 0x002000,
			ClockControl_USB_13xx			= 0x004000,	//*
			ClockControl_Watchdog			= 0x008000,
			ClockControl_IOConfig			= 0x010000,
			ClockControl_CAN_11xx			= 0x020000,
			ClockControl_SPI1_11xx			= 0x040000,
		};
	REGISTER	SPI0ClockDivider =			REGISTER_ADDRESS(0x40048094);
	REGISTER	UARTClockDivider =			REGISTER_ADDRESS(0x40048098);
	REGISTER	SPI1ClockDivider =			REGISTER_ADDRESS(0x4004809C);
	REGISTER	SystickClockDivider =		REGISTER_ADDRESS(0x400480B0);
	REGISTER	WatchdogSource =			REGISTER_ADDRESS(0x400480D0);
		enum WatchdogSource
		{
			WatchdogSource_InternalCrystal		= 0x00,
			WatchdogSource_MainClock			= 0x01,
			WatchdogSource_WDTOscillator		= 0x02,
		};
	REGISTER	WatchdogSourceUpdate =	REGISTER_ADDRESS(0x400480D4);
		enum WatchdogSourceUpdate
		{
			WatchdogSourceUpdate_Enable	=	0x01,
		};
	REGISTER	WatchdogClockDivider =	REGISTER_ADDRESS(0x400480D8);
	REGISTER	ClockOutputSource =		REGISTER_ADDRESS(0x400480E0);
		enum ClockOutputSource
		{
			ClockOutputSource_InternalCrystal		= 0x00,
			ClockOutputSource_ExternalOscillator	= 0x01,
			ClockOutputSource_WDTOscillator			= 0x02,
			ClockOutputSource_MainClock				= 0x03,
		};
	REGISTER	ClockOutputSourceUpdate =	REGISTER_ADDRESS(0x400480E4);
		enum ClockOutputSourceUpdate
		{
			ClockOutputSourceUpdate_Enable	=	0x01,
		};
	REGISTER	ClockOutputDivider =		REGISTER_ADDRESS(0x400480E8);
	//...
	REGISTER	BrownoutControl =			REGISTER_ADDRESS(0x40048150);
		enum BrownoutControl
		{
			BrownoutControl_Reset_1460mV_1630mV			= 0x00,
			BrownoutControl_Reset_2060mV_2150mV			= 0x01,
			BrownoutControl_Reset_2350mV_2430mV			= 0x02,
			BrownoutControl_Reset_2630mV_2710mV			= 0x03,
			
			BrownoutControl_Interrupt_1650mV_1800mV		= 0x00,
			BrownoutControl_Interrupt_2220mV_2350mV		= 0x04,
			BrownoutControl_Interrupt_2520mV_2660mV		= 0x08,
			BrownoutControl_Interrupt_2800mV_2900mV		= 0x0C,
			
			BrownoutControl_ResetEnabled				= 0x10,
		};
	
	//PowerDownControl and PowerDownWakeupControl set the power state of various features
	//  while running or upon wakeup (respectivley.)  Modify only the enumerated bits of these
	//  registers, as modifying undocumented bits will lead to undefined behaviour and may
	//  damage the chip.
	//  A 0 for a bit means running, while 1 means powered-down.
	REGISTER	PowerDownWakeupControl =	REGISTER_ADDRESS(0x40048234);	//aka PDAWAKECFG
	REGISTER	PowerDownControl =			REGISTER_ADDRESS(0x40048238);	//aka PDRUNCFG
		enum PowerDownControl
		{
			PowerDownControl_InternalCrystalOutput		= 0x001,
			PowerDownControl_InternalCrystal			= 0x002,
			PowerDownControl_Flash						= 0x004,
			PowerDownControl_BrownOutDetector			= 0x008,
			PowerDownControl_ADC						= 0x010,
			PowerDownControl_SystemOscillator			= 0x020,	//for an external crystal
			PowerDownControl_WatchdogOscillator			= 0x040,
			PowerDownControl_SystemPLL					= 0x080,
			PowerDownControl_USBPLL						= 0x100,
			PowerDownControl_USBPins					= 0x400,
		};
	
	
	//GPIO
	REGISTER	GPIO0 =					REGISTER_ADDRESS(0x50000000);
	REGISTER	GPIO0Data =				REGISTER_ADDRESS(0x50003FFC);
	REGISTER	GPIO0Dir =				REGISTER_ADDRESS(0x50008000);
	REGISTER	GPIO1 =					REGISTER_ADDRESS(0x50010000);
	REGISTER	GPIO1Data =				REGISTER_ADDRESS(0x50013FFC);
	REGISTER	GPIO1Dir =				REGISTER_ADDRESS(0x50018000);
	REGISTER	GPIO2 =					REGISTER_ADDRESS(0x50020000);
	REGISTER	GPIO2Data =				REGISTER_ADDRESS(0x50023FFC);
	REGISTER	GPIO2Dir =				REGISTER_ADDRESS(0x50028000);
	REGISTER	GPIO3 =					REGISTER_ADDRESS(0x50030000);
	REGISTER	GPIO3Data =				REGISTER_ADDRESS(0x50033FFC);
	REGISTER	GPIO3Dir =				REGISTER_ADDRESS(0x50038000);
	
	
	//SPI 0
	REGISTER	SPI0Control0 =			REGISTER_ADDRESS(0x40040000);
		enum SPI0Control0
		{
			SPI0Control0_4BitTransfer 	=	0x03,
			SPI0Control0_5BitTransfer 	=	0x04,
			SPI0Control0_6BitTransfer 	=	0x05,
			SPI0Control0_7BitTransfer 	=	0x06,
			SPI0Control0_8BitTransfer 	=	0x07,
			SPI0Control0_9BitTransfer 	=	0x08,
			SPI0Control0_10BitTransfer 	=	0x09,
			SPI0Control0_11BitTransfer 	=	0x0A,
			SPI0Control0_12BitTransfer 	=	0x0B,
			SPI0Control0_13BitTransfer 	=	0x0C,
			SPI0Control0_14BitTransfer 	=	0x0D,
			SPI0Control0_15BitTransfer 	=	0x0E,
			SPI0Control0_16BitTransfer 	=	0x0F,

			SPI0Control0_FrameFormat_SPI		=	(0x00 << 4),
			SPI0Control0_FrameFormat_TI			=	(0x00 << 4),
			SPI0Control0_FrameFormat_Microwire	=	(0x00 << 4),

			SPI0Control0_ClockPolarity_IdleLow	=	(0x00 << 6),
			SPI0Control0_ClockPolarity_IdleHigh	=	(0x01 << 6),
			
			SPI0Control0_ClockPhase_IdleToActive =	(0x00 << 7),	//for ...ClockPolarity_IdleLow, this means reads are on the rising edge.
			SPI0Control0_ClockPhase_ActiveToIdle =	(0x01 << 7),	//for ...ClockPolarity_IdleLow, this means reads are on the falling edge.
			
			SPI0Control0_SPIMode0				=	(0x00 << 6),	//polarity 0, phase 0
			SPI0Control0_SPIMode1				=	(0x02 << 6),	//polarity 0, phase 1
			SPI0Control0_SPIMode2				=	(0x01 << 6),	//polarity 1, phase 0
			SPI0Control0_SPIMode3				=	(0x03 << 6),	//polarity 1, phase 1

			SPI0Control0_ClockRateMinus1 		=	(0 << 8),		//bits 8-15
		};
	REGISTER	SPI0Control1 =			REGISTER_ADDRESS(0x40040004);
		enum SPI0Control1
		{
			SPI0Control1_LoopbackMode 	=	0x01,
			SPI0Control1_Enable		 	=	0x02,
			SPI0Control1_SlaveMode		=	0x04,
			SPI0Control1_OutputDisable	=	0x08,
		};
	REGISTER	SPI0Data =				REGISTER_ADDRESS(0x40040008);
	REGISTER	SPI0Status =			REGISTER_ADDRESS(0x4004000C);
		enum SPI0Status
		{
			SPI0Status_TransmitFIFOEmpty 	=	0x01,
			SPI0Status_TransmitFIFONotFull 	=	0x02,
			SPI0Status_ReceiveFIFONotEmpty	=	0x04,
			SPI0Status_ReceiveFIFOFull 		=	0x08,
			
			SPI0Status_Busy 				=	0x10,
		};
	REGISTER	SPI0ClockPrescaler =	REGISTER_ADDRESS(0x40040010);
	REGISTER	SPI0InterruptEnable =	REGISTER_ADDRESS(0x40040014);
		enum SPI0Interrupt
		{
			SPI0Interrupt_ReceiveOverrun		=	0x01,
			SPI0Interrupt_ReceiveTimeout		=	0x02,
			SPI0Interrupt_ReceiveFIFOHalfFull	=	0x04,
			SPI0Interrupt_TransmitFIFOHalfEmpty =	0x08,
		};
	//(enum values are identical to SPI0Interrupt)
	REGISTER	SPI0RawInterrupt =		REGISTER_ADDRESS(0x40040018);
	REGISTER	SPI0MaskedInterrupt =	REGISTER_ADDRESS(0x4004001C);
	REGISTER	SPI0InterruptClear =	REGISTER_ADDRESS(0x40040020);
		enum SPI0InterruptClear
		{
			SPI0InterruptClear_ReceiveOverrun		=	0x01,
			SPI0InterruptClear_ReceiveTimeout		=	0x02,
		};

	//UART
	REGISTER	UARTData =					REGISTER_ADDRESS(0x40008000);	//accessible only when UARTLineControl_DivisorLatch = 0
	REGISTER	UARTInterrupts =			REGISTER_ADDRESS(0x40008004);	// "
		enum UARTInterrupts
		{
			UARTInterrupts_ReceivedData		=	(0x01),
			UARTInterrupts_TxBufferEmpty	=	(0x02),
			UARTInterrupts_RxLineStatus		=	(0x04),
			
			UARTInterrupts_AutoBaudComplete	=	(0x100),
			UARTInterrupts_AutoBaudTimeout	=	(0x200),
		};
	REGISTER	UARTDivisorLow =			REGISTER_ADDRESS(0x40008000);	//when UARTLineControl_DivisorLatch = 1
	REGISTER	UARTDivisorHigh =			REGISTER_ADDRESS(0x40008004);	// "
	REGISTER	UARTInterruptID =			REGISTER_ADDRESS(0x40008008);	//read-only
		enum UARTInterruptID
		{
			UARTInterruptID_InterruptPending	=	(0x01),
			
			UARTInterruptID_ReasonMask			=	(0x7 << 1),
			
			UARTInterruptID_ReceiveException	=	(3 << 1),	//signaled on receive overrun, parity, framing or break errors
			UARTInterruptID_DataAvailable		=	(2 << 1),
			UARTInterruptID_ReceiveTimeout		=	(6 << 1),	//not as bad as it sounds: not enough chars received to trip interrupt
			UARTInterruptID_TxBufferEmpty		=	(1 << 1),
			UARTInterruptID_Modem				=	(0 << 1),
			
			UARTInterruptID_FIFOEnabled			=	(1 << 6),
			
			UARTInterruptID_AutoBaudComplete	=	(0x100),
			UARTInterruptID_AutoBaudTimeout		=	(0x200),
		};
	REGISTER	UARTFIFOControl =			REGISTER_ADDRESS(0x40008008);	//write-only
		enum UARTFIFOControl
		{
			UARTFIFOControl_Enable					=	(0x01),		
			UARTFIFOControl_RxReset					=	(0x02),		//self-clearing bit
			UARTFIFOControl_TxReset					=	(0x04),		//self-clearing bit
			
			UARTFIFOControl_RxInterrupt1Char		=	(0x00 << 6),
			UARTFIFOControl_RxInterrupt4Chars		=	(0x01 << 6),
			UARTFIFOControl_RxInterrupt8Chars		=	(0x02 << 6),
			UARTFIFOControl_RxInterrupt14Chars		=	(0x03 << 6),
		};
	REGISTER	UARTLineControl =			REGISTER_ADDRESS(0x4000800C);
		enum UARTLineControl
		{
			UARTLineControl_5BitChars				=	(0x00),
			UARTLineControl_6BitChars				=	(0x01),
			UARTLineControl_7BitChars				=	(0x02),
			UARTLineControl_8BitChars				=	(0x03),
			
			UARTLineControl_UseStopBit				=	(0x04),
			UARTLineControl_UseParity				=	(0x08),
			
			UARTLineControl_UseOddParity			=	(0x00 << 4),
			UARTLineControl_UseEvenParity			=	(0x01 << 4),
			UARTLineControl_Constant1Parity			=	(0x02 << 4),
			UARTLineControl_Constant0Parity			=	(0x03 << 4),
			
			UARTLineControl_BreakControlEnable		=	(0x40),
			UARTLineControl_DivisorLatch			=	(0x80),
		};
	REGISTER	UARTModemControl =			REGISTER_ADDRESS(0x40008010);
		enum UARTModemControl
		{
			UARTModemControl_DTRPinState			=	(0x01),
			UARTModemControl_RTSPinState			=	(0x02),
			UARTModemControl_LoopbackMode			=	(0x04),
			UARTModemControl_RTSEnable				=	(0x40),
			UARTModemControl_CTSEnable				=	(0x80),
		};
	REGISTER	UARTLineStatus =			REGISTER_ADDRESS(0x40008014);
		enum UARTLineStatus
		{
			UARTLineStatus_ReceiverDataReady		=	(0x01),
			UARTLineStatus_OverrunError				=	(0x02),
			UARTLineStatus_ParityError				=	(0x04),
			UARTLineStatus_FramingError				=	(0x08),
			UARTLineStatus_BreakInterrupt			=	(0x10),
			UARTLineStatus_TxHoldingRegisterEmpty	=	(0x20),
			UARTLineStatus_TransmitterEmpty			=	(0x40),
			UARTLineStatus_RxFIFOEmpty				=	(0x80),
		};
	REGISTER	UARTModemStatus =			REGISTER_ADDRESS(0x40008018);	//read-only
	REGISTER	UARTScratch =				REGISTER_ADDRESS(0x4000801C);
	REGISTER	UARTAutoBaudControl =		REGISTER_ADDRESS(0x40008020);
	REGISTER	UARTFractionalDivider =		REGISTER_ADDRESS(0x40008028);
	REGISTER	UARTTransmitEnabled =		REGISTER_ADDRESS(0x40008030);
	
	
	//I2C
	REGISTER	I2CControlSet =				REGISTER_ADDRESS(0x40000000);
	REGISTER	I2CControlClear =			REGISTER_ADDRESS(0x40000018);
		enum I2CControlSet
		{
			I2CControlSet_Ack				=	(0x04),
			I2CControlSet_Interrupt			=	(0x08),
			I2CControlSet_StopCondition		=	(0x10),		//do not clear I2CControlSet_StopCondition
			I2CControlSet_StartCondition	=	(0x20),
			I2CControlSet_EnableI2C			=	(0x40),
		};
	REGISTER	I2CStatus =					REGISTER_ADDRESS(0x40000004);
		enum I2CStatus
		{
			I2CStatus_	//@@ugh
		};
	REGISTER	I2CData =					REGISTER_ADDRESS(0x40000008);
	REGISTER	I2CBuffer =					REGISTER_ADDRESS(0x4000002C);
	
	REGISTER	I2CClockHighTime =			REGISTER_ADDRESS(0x40000010);
	REGISTER	I2CClockLowTime =			REGISTER_ADDRESS(0x40000014);
	
	REGISTER	I2CSlaveAddress0 =			REGISTER_ADDRESS(0x4000000C);
	REGISTER	I2CSlaveAddress1 =			REGISTER_ADDRESS(0x40000020);
	REGISTER	I2CSlaveAddress2 =			REGISTER_ADDRESS(0x40000024);
	REGISTER	I2CSlaveAddress3 =			REGISTER_ADDRESS(0x40000028);
	REGISTER	I2CSlaveAddress0Mask =		REGISTER_ADDRESS(0x40000030);
	REGISTER	I2CSlaveAddress1Mask =		REGISTER_ADDRESS(0x40000034);
	REGISTER	I2CSlaveAddress2Mask =		REGISTER_ADDRESS(0x40000038);
	REGISTER	I2CSlaveAddress3Mask =		REGISTER_ADDRESS(0x4000003C);
	
	//these enums apply to all timers
		enum TimerInterrupts
		{
			TimerInterrupts_Match0Flag		=	(1 << 0),
			TimerInterrupts_Match1Flag		=	(1 << 1),
			TimerInterrupts_Match2Flag		=	(1 << 2),
			TimerInterrupts_Match3Flag		=	(1 << 3),
			TimerInterrupts_Capture0Flag	=	(1 << 4),
		};
		enum TimerControl
		{
			TimerControl_Enable		=	(1 << 0),
			TimerControl_Reset		=	(1 << 1),
		};
		enum TimerMatchControl
		{
			TimerMatchControl_Match0Interrupt	=	(1 << 0),
			TimerMatchControl_Match0Reset		=	(1 << 1),
			TimerMatchControl_Match0Stop		=	(1 << 2),

			TimerMatchControl_Match1Interrupt	=	(1 << 3),
			TimerMatchControl_Match1Reset		=	(1 << 4),
			TimerMatchControl_Match1Stop		=	(1 << 5),

			TimerMatchControl_Match2Interrupt	=	(1 << 6),
			TimerMatchControl_Match2Reset		=	(1 << 7),
			TimerMatchControl_Match2Stop		=	(1 << 8),

			TimerMatchControl_Match3Interrupt	=	(1 << 9),
			TimerMatchControl_Match3Reset		=	(1 << 10),
			TimerMatchControl_Match3Stop		=	(1 << 11),
		};
		enum TimerCaptureControl
		{
			TimerCaptureControl_CaptureOnRising		=	(1 << 0),
			TimerCaptureControl_CaptureOnFalling	=	(1 << 1),
			TimerCaptureControl_InterruptOnCapture	=	(1 << 2),
		};
		enum TimerExternalMatch
		{
			TimerExternalMatch_Match0State		=	(1 << 0),
			TimerExternalMatch_Match1State		=	(1 << 1),
			TimerExternalMatch_Match2State		=	(1 << 2),
			TimerExternalMatch_Match3State		=	(1 << 3),
			
			TimerExternalMatch_Match0DoNothing	=	(0 << 4),
			TimerExternalMatch_Match0Clear		=	(1 << 4),
			TimerExternalMatch_Match0Set		=	(2 << 4),
			TimerExternalMatch_Match0Toggle		=	(3 << 4),
			
			TimerExternalMatch_Match1DoNothing	=	(0 << 6),
			TimerExternalMatch_Match1Clear		=	(1 << 6),
			TimerExternalMatch_Match1Set		=	(2 << 6),
			TimerExternalMatch_Match1Toggle		=	(3 << 6),
			
			TimerExternalMatch_Match2DoNothing	=	(0 << 8),
			TimerExternalMatch_Match2Clear		=	(1 << 8),
			TimerExternalMatch_Match2Set		=	(2 << 8),
			TimerExternalMatch_Match2Toggle		=	(3 << 8),
			
			TimerExternalMatch_Match3DoNothing	=	(0 << 10),
			TimerExternalMatch_Match3Clear		=	(1 << 10),
			TimerExternalMatch_Match3Set		=	(2 << 10),
			TimerExternalMatch_Match3Toggle		=	(3 << 10),
		};
		enum TimerCountControl
		{
			TimerCountControl_IncrementOnInternalClock		=	(0 << 0),
			TimerCountControl_IncrementOnRisingCapEdge		=	(1 << 0),
			TimerCountControl_IncrementOnFallingCapEdge	=	(2 << 0),
			TimerCountControl_IncrementOnEitherCapEdge		=	(3 << 0),
			
			TimerCountControl_SelectCap0					=	(0 << 2),
			TimerCountControl_SelectCap1					=	(1 << 2),	//not supported
			TimerCountControl_SelectCap2					=	(2 << 2),	//not supported
		};
		enum TimerPWMControl
		{
			TimerPWMControl_Match0		=	(1 << 0),
			TimerPWMControl_Match1		=	(1 << 1),
			TimerPWMControl_Match2		=	(1 << 2),
			TimerPWMControl_Match3		=	(1 << 3),
		};
	
	//Timer 0, 16-bit
	REGISTER	Timer0Interrupts =			REGISTER_ADDRESS(0x4000C000);
	REGISTER	Timer0Control =				REGISTER_ADDRESS(0x4000C004);
	REGISTER	Timer0Counter =				REGISTER_ADDRESS(0x4000C008);
	REGISTER	Timer0Prescaler =			REGISTER_ADDRESS(0x4000C00C);
	REGISTER	Timer0PrescaleCounter =		REGISTER_ADDRESS(0x4000C010);
	REGISTER	Timer0MatchControl =		REGISTER_ADDRESS(0x4000C014);
	REGISTER	Timer0Match0 =				REGISTER_ADDRESS(0x4000C018);
	REGISTER	Timer0Match1 =				REGISTER_ADDRESS(0x4000C01C);
	REGISTER	Timer0Match2 =				REGISTER_ADDRESS(0x4000C020);
	REGISTER	Timer0Match3 =				REGISTER_ADDRESS(0x4000C024);
	REGISTER	Timer0CaptureControl =		REGISTER_ADDRESS(0x4000C028);
	REGISTER	Timer0CaptureValue =		REGISTER_ADDRESS(0x4000C02C);
	REGISTER	Timer0ExternalMatch =		REGISTER_ADDRESS(0x4000C03C);
	REGISTER	Timer0CountControl =		REGISTER_ADDRESS(0x4000C070);
	REGISTER	Timer0PWMControl =			REGISTER_ADDRESS(0x4000C03C);
	
	//Timer 1, 16-bit
	REGISTER	Timer1Interrupts =			REGISTER_ADDRESS(0x40010000);
	REGISTER	Timer1Control =				REGISTER_ADDRESS(0x40010004);
	REGISTER	Timer1Counter =				REGISTER_ADDRESS(0x40010008);
	REGISTER	Timer1Prescaler =			REGISTER_ADDRESS(0x4001000C);
	REGISTER	Timer1PrescaleCounter =		REGISTER_ADDRESS(0x40010010);
	REGISTER	Timer1MatchControl =		REGISTER_ADDRESS(0x40010014);
	REGISTER	Timer1Match0 =				REGISTER_ADDRESS(0x40010018);
	REGISTER	Timer1Match1 =				REGISTER_ADDRESS(0x4001001C);
	REGISTER	Timer1Match2 =				REGISTER_ADDRESS(0x40010020);
	REGISTER	Timer1Match3 =				REGISTER_ADDRESS(0x40010024);
	REGISTER	Timer1CaptureControl =		REGISTER_ADDRESS(0x40010028);
	REGISTER	Timer1CaptureValue =		REGISTER_ADDRESS(0x4001002C);
	REGISTER	Timer1ExternalMatch =		REGISTER_ADDRESS(0x4001003C);
	REGISTER	Timer1CountControl =		REGISTER_ADDRESS(0x40010070);
	REGISTER	Timer1PWMControl =			REGISTER_ADDRESS(0x4001003C);
	
	//Timer 2, 32-bit
	REGISTER	Timer2Interrupts =			REGISTER_ADDRESS(0x40014000);
	REGISTER	Timer2Control =				REGISTER_ADDRESS(0x40014004);
	REGISTER	Timer2Counter =				REGISTER_ADDRESS(0x40014008);
	REGISTER	Timer2Prescaler =			REGISTER_ADDRESS(0x4001400C);
	REGISTER	Timer2PrescaleCounter =		REGISTER_ADDRESS(0x40014010);
	REGISTER	Timer2MatchControl =		REGISTER_ADDRESS(0x40014014);
	REGISTER	Timer2Match0 =				REGISTER_ADDRESS(0x40014018);
	REGISTER	Timer2Match1 =				REGISTER_ADDRESS(0x4001401C);
	REGISTER	Timer2Match2 =				REGISTER_ADDRESS(0x40014020);
	REGISTER	Timer2Match3 =				REGISTER_ADDRESS(0x40014024);
	REGISTER	Timer2CaptureControl =		REGISTER_ADDRESS(0x40014028);
	REGISTER	Timer2CaptureValue =		REGISTER_ADDRESS(0x4001402C);
	REGISTER	Timer2ExternalMatch =		REGISTER_ADDRESS(0x4001403C);
	REGISTER	Timer2CountControl =		REGISTER_ADDRESS(0x40014070);
	REGISTER	Timer2PWMControl =			REGISTER_ADDRESS(0x4001403C);
	
	//Timer 3, 32-bit
	REGISTER	Timer3Interrupts =			REGISTER_ADDRESS(0x40018000);
	REGISTER	Timer3Control =				REGISTER_ADDRESS(0x40018004);
	REGISTER	Timer3Counter =				REGISTER_ADDRESS(0x40018008);
	REGISTER	Timer3Prescaler =			REGISTER_ADDRESS(0x4001800C);
	REGISTER	Timer3PrescaleCounter =		REGISTER_ADDRESS(0x40018010);
	REGISTER	Timer3MatchControl =		REGISTER_ADDRESS(0x40018014);
	REGISTER	Timer3Match0 =				REGISTER_ADDRESS(0x40018018);
	REGISTER	Timer3Match1 =				REGISTER_ADDRESS(0x4001801C);
	REGISTER	Timer3Match2 =				REGISTER_ADDRESS(0x40018020);
	REGISTER	Timer3Match3 =				REGISTER_ADDRESS(0x40018024);
	REGISTER	Timer3CaptureControl =		REGISTER_ADDRESS(0x40018028);
	REGISTER	Timer3CaptureValue =		REGISTER_ADDRESS(0x4001802C);
	REGISTER	Timer3ExternalMatch =		REGISTER_ADDRESS(0x4001803C);
	REGISTER	Timer3CountControl =		REGISTER_ADDRESS(0x40018070);
	REGISTER	Timer3PWMControl =			REGISTER_ADDRESS(0x4001803C);
	
	
	//Watchdog
	REGISTER	WatchdogMode =				REGISTER_ADDRESS(0x40004000);
	REGISTER	WatchdogTimeLimit =			REGISTER_ADDRESS(0x40004004);
	REGISTER	WatchdogFeed =				REGISTER_ADDRESS(0x40004008);
	REGISTER	WatchdogTimerValue =		REGISTER_ADDRESS(0x4000400C);


	//ADC
	REGISTER	ADCControl =				REGISTER_ADDRESS(0x4001C000);
		enum ADCControl
		{
			ADCControl_ChannelSelectBitmask =		(0xFF),	//applies to manual (one at a time) mode
			
			ADCControl_BurstModeBitmask =			(0xFF),	//applies to automatic (burst, round-robin) mode only
			
			//This must be chosen so that PCLK / (ADCControl_ADCClockDivider + 1) is close to but less than 4.5MHz.
			//  The clock rate may be decreased to better sample high-impedance analog sources.
			ADCControl_ADCClockDividerBitmask =		(0xFF << 8),

			//Burst Mode: if 0, conversion is performed according to the ADCControl_Start* setting.
			//	If 1, hardware round-robins through the selected bits (7:0) to convert those channels
			//  Important: ADCControl_Stop must be selected when ADCControl_EnableBurstMode is 1 or conversions will not start.
			ADCControl_EnableBurstMode =			(0x01 << 16),
			
			//Resolution setting
			ADCControl_10BitSample_11Clocks =		(0x00 << 17),
			ADCControl_9BitSample_10Clocks =		(0x01 << 17),
			ADCControl_8BitSample_9Clocks =			(0x02 << 17),
			ADCControl_7BitSample_8Clocks =			(0x03 << 17),
			ADCControl_6BitSample_7Clocks =			(0x04 << 17),
			ADCControl_5BitSample_6Clocks =			(0x05 << 17),
			ADCControl_4BitSample_5Clocks =			(0x06 << 17),
			ADCControl_3BitSample_4Clocks =			(0x07 << 17),

			//Note: one ADC sample on one channel takes 2.44us
			ADCControl_StartStopMask =				(0x07 << 24),
			ADCControl_Stop	=						(0x00 << 24),
			ADCControl_StartNow	=					(0x01 << 24),
			
			//Start conversion on timer events:
			ADCControl_StartOnTimer0Cap0 =			(0x02 << 24),
			ADCControl_StartOnTimer2Cap0 =			(0x03 << 24),
			ADCControl_StartOnTimer2Mat0 =			(0x04 << 24),
			ADCControl_StartOnTimer2Mat1 =			(0x05 << 24),
			ADCControl_StartOnTimer0Mat0 =			(0x06 << 24),
			ADCControl_StartOnTimer0Mat1 =			(0x07 << 24),

			ADCControl_StartOnTimerFallingEdge =	(0x01 << 27),	//else rising edge
		};

	REGISTER	ADCData =					REGISTER_ADDRESS(0x4001C004);
		enum ADCData
		{
			ADCData_ResultLeftJustifiedMask		=	(0xFFFF),
			ADCData_ChannelMask					=	(0x7 << 16),
			ADCData_Overrun						=	(1 << 30),
			ADCData_Done						=	(1 << 31),
		};
	REGISTER	ADCInterrupt =				REGISTER_ADDRESS(0x4001C00C);
		enum ADCInterrupt
		{
			ADCInterrupt_InterruptOnADC0	=	(1 << 0),
			ADCInterrupt_InterruptOnADC1	=	(1 << 1),
			ADCInterrupt_InterruptOnADC2	=	(1 << 2),
			ADCInterrupt_InterruptOnADC3	=	(1 << 3),
			ADCInterrupt_InterruptOnADC4	=	(1 << 4),
			ADCInterrupt_InterruptOnADC5	=	(1 << 5),
			ADCInterrupt_InterruptOnADC6	=	(1 << 6),
			ADCInterrupt_InterruptOnADC7	=	(1 << 7),
			ADCInterrupt_InterruptOnAny		=	(1 << 8),
		};
		
	REGISTER	ADC0Data =					REGISTER_ADDRESS(0x4001C010);
	REGISTER	ADC1Data =					REGISTER_ADDRESS(0x4001C014);
	REGISTER	ADC2Data =					REGISTER_ADDRESS(0x4001C018);
	REGISTER	ADC3Data =					REGISTER_ADDRESS(0x4001C01C);
	REGISTER	ADC4Data =					REGISTER_ADDRESS(0x4001C020);
	REGISTER	ADC5Data =					REGISTER_ADDRESS(0x4001C024);
	REGISTER	ADC6Data =					REGISTER_ADDRESS(0x4001C028);
	REGISTER	ADC7Data =					REGISTER_ADDRESS(0x4001C02C);

	REGISTER	ADCStatus =					REGISTER_ADDRESS(0x4001C030);
		enum ADCStatus
		{
			ADCStatus_ADC0Done =			(0x01),
			ADCStatus_ADC1Done =			(0x02),
			ADCStatus_ADC2Done =			(0x04),
			ADCStatus_ADC3Done =			(0x08),
			ADCStatus_ADC4Done =			(0x10),
			ADCStatus_ADC5Done =			(0x20),
			ADCStatus_ADC6Done =			(0x40),
			ADCStatus_ADC7Done =			(0x80),
			
			ADCStatus_ADC0Overrun =			(0x01 << 8),
			ADCStatus_ADC1Overrun =			(0x02 << 8),
			ADCStatus_ADC2Overrun =			(0x04 << 8),
			ADCStatus_ADC3Overrun =			(0x08 << 8),
			ADCStatus_ADC4Overrun =			(0x10 << 8),
			ADCStatus_ADC5Overrun =			(0x20 << 8),
			ADCStatus_ADC6Overrun =			(0x40 << 8),
			ADCStatus_ADC7Overrun =			(0x80 << 8),
			
			ADCStatus_InterruptRaised =		(0x01 << 16),
		};
	
	
	REGISTER	SystickControl =			REGISTER_ADDRESS(0xE000E010);
		enum SystickControl
		{
			SystickControl_Enable =				(0x01),
			SystickControl_InterruptEnabled =	(0x02),
			SystickControl_ClockSource =		(0x04),	//always 0 on LPC1xxx
			SystickControl_Signalled =			(1 << 16),
		};
	REGISTER	SystickReload =				REGISTER_ADDRESS(0xE000E014);
	REGISTER	SystickValue =				REGISTER_ADDRESS(0xE000E018);
	
	//Write 1 to set interrupts on InterruptSet, write 1 to clear interrupts on InterruptClear
	REGISTER	InterruptEnableSet0 =		REGISTER_ADDRESS(0xE000E100);
		enum Interrupt0
		{
			Interrupt1_PIO0_0 =		(1 << 0),
			Interrupt1_PIO0_1 =		(1 << 0),
			Interrupt1_PIO0_2 =		(1 << 0),
			Interrupt1_PIO0_3 =		(1 << 0),
			Interrupt1_PIO0_4 =		(1 << 0),
			Interrupt1_PIO0_5 =		(1 << 0),
			Interrupt1_PIO0_6 =		(1 << 0),
			Interrupt1_PIO0_7 =		(1 << 0),
			Interrupt1_PIO0_8 =		(1 << 0),
			Interrupt1_PIO0_9 =		(1 << 0),
			Interrupt1_PIO0_10 =	(1 << 0),
			Interrupt1_PIO0_11 =	(1 << 11),
			Interrupt1_PIO1_0 =		(1 << 12),
			Interrupt1_PIO1_1 =		(1 << 13),
			Interrupt1_PIO1_2 =		(1 << 14),
			Interrupt1_PIO1_3 =		(1 << 15),
			Interrupt1_PIO1_4 =		(1 << 16),
			Interrupt1_PIO1_5 =		(1 << 17),
			Interrupt1_PIO1_6 =		(1 << 18),
			Interrupt1_PIO1_7 =		(1 << 19),
			Interrupt1_PIO1_8 =		(1 << 20),
			Interrupt1_PIO1_9 =		(1 << 21),
			Interrupt1_PIO1_10 =	(1 << 22),
			Interrupt1_PIO1_11 =	(1 << 23),
			Interrupt1_PIO2_0 =		(1 << 24),
			Interrupt1_PIO2_1 =		(1 << 25),
			Interrupt1_PIO2_2 =		(1 << 26),
			Interrupt1_PIO2_3 =		(1 << 27),
			Interrupt1_PIO2_4 =		(1 << 28),
			Interrupt1_PIO2_5 =		(1 << 29),
			Interrupt1_PIO2_6 =		(1 << 30),
			Interrupt1_PIO2_7 =		(1 << 31),
		};
	REGISTER	InterruptEnableSet1 =		REGISTER_ADDRESS(0xE000E104);
		enum Interrupt1
		{
			Interrupt1_PIO2_8 =		(1 << 0),
			Interrupt1_PIO2_9 =		(1 << 1),
			Interrupt1_PIO2_10 =	(1 << 2),
			Interrupt1_PIO2_11 =	(1 << 3),
			Interrupt1_PIO3_0 =		(1 << 4),
			Interrupt1_PIO3_1 =		(1 << 5),
			Interrupt1_PIO3_2 =		(1 << 6),
			Interrupt1_PIO3_3 =		(1 << 7),
			Interrupt1_I2C =		(1 << 8),
			Interrupt1_Timer0 =		(1 << 9),
			Interrupt1_Timer1 =		(1 << 10),
			Interrupt1_Timer2 =		(1 << 11),
			Interrupt1_Timer3 =		(1 << 12),
			Interrupt1_SPI0 =		(1 << 13),
			Interrupt1_UART =		(1 << 14),
			Interrupt1_USB =		(1 << 15),
			Interrupt1_USBFast =	(1 << 16),
			Interrupt1_ADC =		(1 << 17),
			Interrupt1_Watchdog =	(1 << 18),
			Interrupt1_BrownOut =	(1 << 19),
			//(reserved value = 1 << 20)
			Interrupt1_GPIO3 =		(1 << 21),
			Interrupt1_GPIO2 =		(1 << 22),
			Interrupt1_GPIO1 =		(1 << 23),
			Interrupt1_GPIO0 =		(1 << 24),
			Interrupt1_SPI1 =		(1 << 25),
		};
	REGISTER	InterruptEnableClear0 =		REGISTER_ADDRESS(0xE000E180);
	REGISTER	InterruptEnableClear1 =		REGISTER_ADDRESS(0xE000E184);
	REGISTER	InterruptSetPending0 =		REGISTER_ADDRESS(0xE000E200);
	REGISTER	InterruptSetPending1 =		REGISTER_ADDRESS(0xE000E204);
	REGISTER	InterruptClearPending0 =	REGISTER_ADDRESS(0xE000E280);
	REGISTER	InterruptClearPending1 =	REGISTER_ADDRESS(0xE000E284);
	REGISTER	InterruptActive0 =			REGISTER_ADDRESS(0xE000E300);
	REGISTER	InterruptActive1 =			REGISTER_ADDRESS(0xE000E304);
	REGISTER	InterruptTrigger =			REGISTER_ADDRESS(0xE000EF00);
		enum InterruptTrigger
		{
			InterruptTrigger_PIO0_0		=	0,
			InterruptTrigger_PIO0_1,
			InterruptTrigger_PIO0_2,
			InterruptTrigger_PIO0_3,
			InterruptTrigger_PIO0_4,
			InterruptTrigger_PIO0_5,
			InterruptTrigger_PIO0_6,
			InterruptTrigger_PIO0_7,
			InterruptTrigger_PIO0_8,
			InterruptTrigger_PIO0_9,
			InterruptTrigger_PIO0_10,
			InterruptTrigger_PIO0_11,
			InterruptTrigger_PIO1_0,
			InterruptTrigger_PIO1_1,
			InterruptTrigger_PIO1_2,
			InterruptTrigger_PIO1_3,
			InterruptTrigger_PIO1_4,
			InterruptTrigger_PIO1_5,
			InterruptTrigger_PIO1_6,
			InterruptTrigger_PIO1_7,
			InterruptTrigger_PIO1_8,
			InterruptTrigger_PIO1_9,
			InterruptTrigger_PIO1_10,
			InterruptTrigger_PIO1_11,
			InterruptTrigger_PIO2_0,
			InterruptTrigger_PIO2_1,
			InterruptTrigger_PIO2_2,
			InterruptTrigger_PIO2_3,
			InterruptTrigger_PIO2_4,
			InterruptTrigger_PIO2_5,
			InterruptTrigger_PIO2_6,
			InterruptTrigger_PIO2_7,
			InterruptTrigger_PIO2_8,
			InterruptTrigger_PIO2_9,
			InterruptTrigger_PIO2_10,
			InterruptTrigger_PIO2_11,
			InterruptTrigger_PIO3_0,
			InterruptTrigger_PIO3_1,
			InterruptTrigger_PIO3_2,
			InterruptTrigger_PIO3_3,
			InterruptTrigger_I2C,
			InterruptTrigger_Timer0,
			InterruptTrigger_Timer1,
			InterruptTrigger_Timer2,
			InterruptTrigger_Timer3,
			InterruptTrigger_SPI0,
			InterruptTrigger_UART,
			InterruptTrigger_USB,
			InterruptTrigger_USBFast,
			InterruptTrigger_ADC,
			InterruptTrigger_Watchdog,
			InterruptTrigger_BrownOut,
			//reserved value
			InterruptTrigger_GPIO3		=	(InterruptTrigger_BrownOut + 2),
			InterruptTrigger_GPIO2,
			InterruptTrigger_GPIO1,
			InterruptTrigger_GPIO0,
			InterruptTrigger_SPI1,
		};
	
	#ifdef __cplusplus
	} //ns
	#endif //__cplusplus
	
	#define INTERRUPT							__attribute__ ((interrupt ("IRQ")))		//interrupt type is ignored on ARMv7-M
	#define	InterruptsDisable()					__asm volatile ("CPSID i" ::)
	#define	InterruptsEnable()					__asm volatile ("CPSIE i" ::)
	
	#ifdef __cplusplus
	extern "C" {
	#endif //__cplusplus
	
	void		Sleep(void);
	void		Reset(void);
	
	void		IRQ_WakeupPIO0_0(void);
	void		IRQ_WakeupPIO0_1(void);
	void		IRQ_WakeupPIO0_2(void);
	void		IRQ_WakeupPIO0_3(void);
	void		IRQ_WakeupPIO0_4(void);
	void		IRQ_WakeupPIO0_5(void);
	void		IRQ_WakeupPIO0_6(void);
	void		IRQ_WakeupPIO0_7(void);
	void		IRQ_WakeupPIO0_8(void);
	void		IRQ_WakeupPIO0_9(void);
	void		IRQ_WakeupPIO0_10(void);
	void		IRQ_WakeupPIO0_11(void);
	void		IRQ_WakeupPIO1_0(void);
	void		IRQ_WakeupPIO1_1(void);
	void		IRQ_WakeupPIO1_2(void);
	void		IRQ_WakeupPIO1_3(void);
	void		IRQ_WakeupPIO1_4(void);
	void		IRQ_WakeupPIO1_5(void);
	void		IRQ_WakeupPIO1_6(void);
	void		IRQ_WakeupPIO1_7(void);
	void		IRQ_WakeupPIO1_8(void);
	void		IRQ_WakeupPIO1_9(void);
	void		IRQ_WakeupPIO1_10(void);
	void		IRQ_WakeupPIO1_11(void);
	void		IRQ_WakeupPIO2_0(void);
	void		IRQ_WakeupPIO2_1(void);
	void		IRQ_WakeupPIO2_2(void);
	void		IRQ_WakeupPIO2_3(void);
	void		IRQ_WakeupPIO2_4(void);
	void		IRQ_WakeupPIO2_5(void);
	void		IRQ_WakeupPIO2_6(void);
	void		IRQ_WakeupPIO2_7(void);
	void		IRQ_WakeupPIO2_8(void);
	void		IRQ_WakeupPIO2_9(void);
	void		IRQ_WakeupPIO2_10(void);
	void		IRQ_WakeupPIO2_11(void);
	void		IRQ_WakeupPIO3_0(void);
	void		IRQ_WakeupPIO3_1(void);
	void		IRQ_WakeupPIO3_2(void);
	void		IRQ_WakeupPIO3_3(void);
	void		IRQ_I2C(void);
	void		IRQ_Timer16_0(void);
	void		IRQ_Timer16_1(void);
	void		IRQ_Timer32_0(void);
	void		IRQ_Timer32_1(void);
	void		IRQ_SPI0(void);
	void		IRQ_UART(void);
	void		IRQ_USB_IRQ(void);
	void		IRQ_USB_FIQ(void);
	void		IRQ_ADC(void);
	void		IRQ_Watchdog(void);
	void		IRQ_Brownout(void);
	void		IRQ_GPIO_3(void);
	void		IRQ_GPIO_2(void);
	void		IRQ_GPIO_1(void);
	void		IRQ_GPIO_0(void);
	
	#ifdef __cplusplus
	}	//extern "C"
	#endif //__cplusplus

	#endif	//assembler

#endif	//__LPC1300_SERIES_INCLUDED__
