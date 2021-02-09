#ifndef	__DEF_H__
#define	__DEF_H__

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"

//typedef enum {FALSE=0, TRUE=1} BOOLEAN;


#pragma pack(1)
typedef struct
{
	//---------------------//
	// события от системного таймера SysTick
	bool	_10ms_event;
	bool	_100ms_event;
	bool	_500ms_event;
	bool	_1000ms_event;
	//---------------------//
} SYSTEM_EVENTS;

#pragma pack()


//====================================	   B U T T O N S   ====================================	 //
#define		LSB16		0x00010000

#define		PIN0	0
#define		PIN1	1
#define		PIN2	2
#define		PIN3	3
#define		PIN4	4
#define		PIN5	5
#define		PIN6	6
#define		PIN7	7
#define		PIN8	8
#define		PIN9	9
#define		PIN10	10
#define		PIN11	11
#define		PIN12	12
#define		PIN13	13
#define		PIN14	14
#define		PIN15	15


// button states
typedef enum {	RELEASED=0,
				PRESS_DETECTED,
				PRESS_SHORTTIME_CONFIRMED,
				PRESS_LONGTIME_CONFIRMED,
				RELEASE_DETECTED} BUTTON_STATE;

typedef struct {
	BUTTON_STATE	state;
	bool			event;
	BUTTON_STATE	pressed_type;
	uint16_t		press_time_cnt;
} BUTTON_CTRL;


//====================================	   L E D S  ====================================	 //
#define		LED_PWR_ORANGE_ON		MDR_PORTA->RXTX &= ~(1U<<PIN4);
#define		LED_PWR_ORANGE_OFF		MDR_PORTA->RXTX |= (1U<<PIN4);
#define		LED_PWR_GREEN_ON		MDR_PORTA->RXTX &= ~(1U<<PIN5);
#define		LED_PWR_GREEN_OFF		MDR_PORTA->RXTX |= (1U<<PIN5);


// SYSTEM POWER ON/OFF
#define	SYSTEM_POWER_ON		MDR_PORTE->RXTX |= (1U<<PIN3);
#define	SYSTEM_POWER_OFF	MDR_PORTE->RXTX &= ~(1U<<PIN3);



// GPIO.c
extern BUTTON_CTRL btn_pwr_ctrl;
extern BUTTON_CTRL btn_rst_ctrl;
extern BUTTON_CTRL btn_boot_ctrl;

// GPIO.c
void GPIO_Init(void);
void CheckButtons(void);


#endif	// __DEF_H__