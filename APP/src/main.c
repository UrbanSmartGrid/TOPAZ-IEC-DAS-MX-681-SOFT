#include "def.h"


/***********************************************************************************/

SYSTEM_EVENTS system_events = {false, false, false, false};

/***********************************************************************************/
//#define	HSE_FREQ	12	// MHz
//#define	PLL_MUL		5	// HCLK = 72 MHz
#define	HSE_FREQ	8	// MHz
#define	PLL_MUL		9	// HCLK = 80 MHz
#define	_PLL_
void StartupClockConfigure(void)
{
	// смещение таблицы векторов прерываний
	// SCB - System Control Block
	// VTOR - Vector Table Offset Register
	SCB->VTOR = 0x08000000;	// таблица располагается в начале Flash-памяти
	
	// После RESET тактирование осуществляется от HSI = 8 МГц
	
	// Запускаем генератор HSE
	MDR_RST_CLK->HS_CONTROL = 0x0001;	// режим осциллятора
	
	while((MDR_RST_CLK->CLOCK_STATUS & RST_CLK_CLOCK_STATUS_HSE_RDY) != RST_CLK_CLOCK_STATUS_HSE_RDY);
	
	// Переключаем источник тактирования CPU_C1 с HSI на HSE
	// CPU_C1 = HSE_FREQ : CPU_C2 = CPU_C1 : CPU_C3 = CPU_C2 : HCLK = CPU_C3 = HSE_FREQ
	MDR_RST_CLK->CPU_CLOCK = 0x0102;
	
	// выключаем генераторы HSI и LSI
	MDR_BKP->REG_0F = 0x0000;
	
#ifdef	_PLL_
	// коэффициент умножения входного тактового сигнала
	MDR_RST_CLK->PLL_CONTROL = (PLL_MUL << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos);
	// запускаем PLL
	MDR_RST_CLK->PLL_CONTROL |= RST_CLK_PLL_CONTROL_PLL_CPU_ON;
	while((MDR_RST_CLK->CLOCK_STATUS & RST_CLK_CLOCK_STATUS_PLL_CPU_RDY) != RST_CLK_CLOCK_STATUS_PLL_CPU_RDY);
	// подключаем PLL к тактированию: CPU_C2 = PLL_CPU
	MDR_RST_CLK->CPU_CLOCK |= RST_CLK_CPU_CLOCK_CPU_C2_SEL;
#endif	// _PLL_
}


/***********************************************************************************/
#if HSE_FREQ == 12
	#define	RELOAD_VALUE	72000
#else
	#define	RELOAD_VALUE	80000
#endif

void StartSysTick(void)
{
	SysTick->LOAD = RELOAD_VALUE;
	
	// Clk Source = HCLK & SysTick Interrupt Enable
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
	
	// Start Systick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}


/***********************************************************************************/
// Прерывание от системного таймера SysTick каждую 1 мс
void SysTick_Handler(void)
{
	static uint16_t _10ms_		= 0;
	static uint16_t _100ms_	= 0;
	static uint16_t _500ms_	= 0;
	static uint16_t _1000ms_	= 0;
	
	
	_10ms_++;
	_100ms_++;
	_500ms_++;
	_1000ms_++;
	
	
	if(_10ms_ >= 10)
	{
		system_events._10ms_event = true;
		_10ms_ = 0;
	}
	
	if(_100ms_ >= 100)
	{
		system_events._100ms_event = true;
		_100ms_ = 0;
	}
	
	if(_500ms_ >= 500)
	{
		system_events._500ms_event = true;
		_500ms_ = 0;
	}
	
	if(_1000ms_ >= 1000)
	{
		system_events._1000ms_event = true;
		_1000ms_ = 0;
	}	
}


/***********************************************************************************/
/********************			 M A I N			 *******************************/
/***********************************************************************************/
void main(void)
{
//	bool dbg_flag = false;
	
	
	StartupClockConfigure();
	StartSysTick();
	GPIO_Init();

	
	while(1)
	{
//		if(system_events._10ms_event == TRUE)
//		{
//			system_events._10ms_event = FALSE;
//		}
		
		if(system_events._100ms_event == true)
		{
			system_events._100ms_event = false;
			
			CheckButtons();
			
			if(btn_pwr_ctrl.event == true)
			{
				if((btn_pwr_ctrl.state == RELEASED) && (btn_pwr_ctrl.pressed_type == PRESS_SHORTTIME_CONFIRMED))
				{
					printf("BTN_PWR_CTRL => SHORT TIME PRESSED\n");
				}
				
				if(btn_pwr_ctrl.state == PRESS_LONGTIME_CONFIRMED)
				{
					printf("BTN_PWR_CTRL => LONG TIME PRESSED\n");
				}
			}	
		}
		
//		if(system_events._500ms_event == TRUE)
//		{
//			system_events._500ms_event = FALSE;
//		}
		
//		if(system_events._1000ms_event == true)
//		{
//			system_events._1000ms_event = false;
//			
//			if(dbg_flag==false)
//			{
//				dbg_flag = true;
////				LED_PWR_ORANGE_ON
//				LED_PWR_GREEN_ON
//			}
//			else
//			{
//				dbg_flag = false;
////				LED_PWR_ORANGE_OFF
//				LED_PWR_GREEN_OFF
//			}
//		}
	}
}