#include "def.h"


BUTTON_CTRL btn_pwr_ctrl	= {RELEASED, false, RELEASED, 0};
BUTTON_CTRL btn_rst_ctrl	= {RELEASED, false, RELEASED, 0};
BUTTON_CTRL btn_boot_ctrl	= {RELEASED, false, RELEASED, 0};


/***********************************************************************************/
void GPIO_Init(void)
{
	// Power On GPIO ports
	MDR_RST_CLK->PER_CLOCK |= (1U<<29) | (1U<<25) | (1U<<24) | (1U<<23) | (1U<<22) | (1U<<21);
	
	//--------------------------------------------------------------------------------------------------------//
	// Buttons
	// inputs
	MDR_PORTA->OE &= ~((1U<<PIN1) | (1U<<PIN2) | (1U<<PIN3));
	// no pull-up & no pull-down
	MDR_PORTA->PULL &= ~((1U<<PIN1) | (1U<<PIN2) | (1U<<PIN3) | (LSB16<<PIN1) | (LSB16<<PIN2) | (LSB16<<PIN3));
	// триггер Шмитта 400 мВ
	MDR_PORTA->PD |= (LSB16<<PIN1) | (LSB16<<PIN2) | (LSB16<<PIN3);
	// фильтрация импульсов до 10 нс
	MDR_PORTA->GFEN |= (1U<<PIN1) | (1U<<PIN2) | (1U<<PIN3);
	
	//MDR_PORTA->PWR
	
	// режим цифрового порта
	MDR_PORTA->ANALOG |= (1U<<PIN1) | (1U<<PIN2) | (1U<<PIN3);
	
	//--------------------------------------------------------------------------------------------------------//
	// Leds
	MDR_PORTA->RXTX |= (1U<<PIN5) | (1U<<PIN4);
	// no pull-up & no pull-down
	MDR_PORTA->PULL &= ~((1U<<PIN5) | (1U<<PIN4) | (LSB16<<PIN5) | (LSB16<<PIN4));
	MDR_PORTA->OE |= (1U<<PIN5) | (1U<<PIN4);
	// режим цифрового порта
	MDR_PORTA->ANALOG |= (1U<<PIN5) | (1U<<PIN4);
	
	 // BMC_PWR_ON = PE3: digital out '0' - power off '1' - power on
	MDR_PORTE->RXTX &= ~(1U<<PIN3);
	// no pull-up & no pull-down
	MDR_PORTE->PULL &= ~(1U<<PIN3);
	MDR_PORTE->OE |= (1U<<PIN3);
	// медленный фронт (100 нс)
	MDR_PORTE->PWR &= ~(3U << 6);
	MDR_PORTE->PWR |= (1U << 6);
	// режим цифрового порта
	MDR_PORTE->ANALOG |= (1U<<PIN3);
}


/***********************************************************************************/
// Отслеживание нажатия кнопок. Опрос состояния каждые 100 мс.
// BTN_PWR_CTRL - PA3: краткое и длительное нажатие
// BTN_RST_CTRL - PA3: краткое нажатие
// BTN_BOOT_CTRL - PA2: нажатие только при загрузке системы
#define	LONGTIME_CNT	30	// 3 seconds

bool DefineButtonState(uint16_t btn_pressed, BUTTON_CTRL* btn)
{
	if(btn_pressed)
	{
		if(btn->state == RELEASE_DETECTED)
		{
			btn->state = RELEASED;
			
			return true;
		}
		else
		{
			if(btn->state != RELEASED)
				btn->state = RELEASE_DETECTED;
			else
				btn->pressed_type = RELEASED;
		}			
	}
	else
	{
		if((btn->state == RELEASED) || (btn->state == RELEASE_DETECTED))
			btn->state = PRESS_DETECTED;
		else
		{
			if(btn->state == PRESS_DETECTED)
			{
				btn->state = PRESS_SHORTTIME_CONFIRMED;
				btn->pressed_type = PRESS_SHORTTIME_CONFIRMED;
				btn->press_time_cnt = 0;

				return true;
			}
			else
			{
				if(btn->state == PRESS_SHORTTIME_CONFIRMED)
				{
					btn->press_time_cnt++;
					
					if(btn->press_time_cnt == LONGTIME_CNT)
					{
						btn->state = PRESS_LONGTIME_CONFIRMED;
						btn->pressed_type = PRESS_LONGTIME_CONFIRMED;
												
						return true;
					}
				}
			}
		}
	}
	
	
	return false;
}


void CheckButtons(void)
{	
	//	BTN_PWR_CTRL
	btn_pwr_ctrl.event = DefineButtonState(MDR_PORTA->RXTX & (1U<<PIN3), &btn_pwr_ctrl);
	
	//	BTN_RST_CTRL
	btn_rst_ctrl.event = DefineButtonState(MDR_PORTA->RXTX & (1U<<PIN1), &btn_rst_ctrl);
	
	//	BTN_BOOT_CTRL
	btn_boot_ctrl.event = DefineButtonState(MDR_PORTA->RXTX & (1U<<PIN2), &btn_boot_ctrl);	
}