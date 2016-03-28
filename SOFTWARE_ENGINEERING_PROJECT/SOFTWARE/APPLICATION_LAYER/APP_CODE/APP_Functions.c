/*
 * APP_Functions.c
 *
 *  Created on: Mar 26, 2016
 *      Author: Ahmed
 */
#include "../../SHARED_LIBRARIES/SHARED_LIBRARIES_CODE/types.h"
#include "../../MCAL_LAYER/TIMER/TIMER_CODE/TIMER_interface.h"
#include "../../MCAL_LAYER/DIO/DIO_CODE/DIO_interface.h"
#include "../../HAL_LAYER/LCD/LCD_CODE/LCD_interface.h"
#include "../../HAL_LAYER/TACTILE_SWITCH/TACTILE_SWITCH_CODE/TACTILE_SWITCH_interface.h"
#include "APP_Interface.h"
#include "APP_Private.h"
#include <stdlib.h>

static u8 APP_u81MilliCounter = 0;
static u8 App_u8StopWatchFlag = APP_FLAGDOWN;
static u8 App_u8TempCounterFlag = APP_FLAGDOWN;
static u32 APP_u32Timer = APP_u8INITCOUNTER;
static u32 APP_u32StopWatch = 0;
static u32 APP_u32TempCounter = 0;
static u16 APP_u161MilliSecondCounterStopWatch = 0;
static u8 APP_u8SwitchState[3] =
    {
    TACTILE_u8SWITCHRELEASED, TACTILE_u8SWITCHRELEASED, TACTILE_u8SWITCHRELEASED
    };

//update 1ms counter
void TIMER_ISR(void)
    {

    APP_u81MilliCounter++;

    return;
    }

//initialize all modules
void APP_voidInit(void)
    {

    DIO_VoidInit();
    LCD_voidInit();
    TIMER_voidInit(TIMER_ISR);

    return;
    }

//check 1ms counter return its value then make it equal 0
extern u8 APP_u8CheckFlag(void)
    {

    u8 Local_u8Result;

    Local_u8Result = APP_u81MilliCounter;

    APP_u81MilliCounter = 0;

    return Local_u8Result;

    }

//update timer counter
extern void APP_u8TimeUpdate(u8 Copy_u81MilliSecondFlag)
    {

    static u16 Local_u161MilliSecondCounter = 0;

    Local_u161MilliSecondCounter += Copy_u81MilliSecondFlag;

    if (Local_u161MilliSecondCounter >= 1000) //default 1000
	{
	Local_u161MilliSecondCounter -= 1000; //default 1000

	APP_u32Timer++;

	}
    else
	{

	}

    if (APP_u32Timer == APP_u8FULLDAY) //check reaching end of the day
	{

	APP_u32Timer = APP_u8INITCOUNTER;

	}

    return;
    }

extern void APP_u8StopWatchUpdate(u8 Copy_u81MilliSecondFlag)
    {

    APP_u161MilliSecondCounterStopWatch += Copy_u81MilliSecondFlag;

    if (APP_u161MilliSecondCounterStopWatch >= 1000) //default 1000
	{

	APP_u161MilliSecondCounterStopWatch -= 1000; //default 1000

	APP_u32StopWatch++;

	}
    else
	{

	}

    if (APP_u32Timer == APP_u8FULLDAY) //check reaching end of the day
	{

	APP_u32StopWatch = 0;

	}

    return;
    }

extern void APP_u8TempCounterUpdate(u8 Copy_u81MilliSecondFlag) //////////////////////
    {

    static u16 Local_u161MilliSecondCounter = 0;

    Local_u161MilliSecondCounter += Copy_u81MilliSecondFlag;

    if (Local_u161MilliSecondCounter >= 1000) //default 1000
	{

	Local_u161MilliSecondCounter -= 1000; //default 1000

	APP_u32TempCounter++;

	}
    else
	{

	}

    if (APP_u32Timer == APP_u8FULLDAY) //check reaching end of the day
	{

	APP_u32TempCounter = 0;

	}

    return;

    }

//update all time counters
extern void APP_voidUpdateCounters(void)

    {

    u8 Local_u81MilliSecondFlag;

    Local_u81MilliSecondFlag = APP_u8CheckFlag();

    APP_u8TimeUpdate(Local_u81MilliSecondFlag);

    if (App_u8StopWatchFlag == APP_FLAGUP)
	{

	APP_u8StopWatchUpdate(Local_u81MilliSecondFlag);

	}
    else
	{

	}

    if (App_u8TempCounterFlag == APP_FLAGUP)
	{

	APP_u8TempCounterUpdate(Local_u81MilliSecondFlag);

	}
    else
	{

	}

    return;

    }

//convert to 12hours system
void APP_Convert12HoursSystem(u32 Copy_APP_u32Timer, u8* Copy_Local_u8Time)
    {

    u32 Local_u32Timer;

    // u8 Local_u8TempChar[10];

    Local_u32Timer = Copy_APP_u32Timer / 3600;

    //itoa(Copy_APP_u32Timer, Local_u8TempChar, 10);
    //  LCD_voidWriteChar(Local_u8TempChar[0]);

    //exceded max hours
    if (Local_u32Timer >= 1 && Local_u32Timer < 13)
	{

	Copy_Local_u8Time[APP_u8AMPMFLAG] = APP_u8AM;

	if (Local_u32Timer == 12UL)
	    {

	    Copy_Local_u8Time[APP_u8AMPMFLAG] = APP_u8PM;

	    }
	else
	    {
	    }

	Local_u32Timer = Copy_APP_u32Timer;

	}
    else if (Local_u32Timer >= 13)
	{

	Copy_Local_u8Time[APP_u8AMPMFLAG] = APP_u8PM;

	if (Local_u32Timer == 24UL)
	    {

	    Copy_Local_u8Time[APP_u8AMPMFLAG] = APP_u8AM;

	    }

	else
	    {
	    }

	Local_u32Timer = Copy_APP_u32Timer - (12 * 60 * 60UL);

	}

    else
	{

	Local_u32Timer = Copy_APP_u32Timer;
	}

    Copy_Local_u8Time[APP_HOURS] = Local_u32Timer / 3600;

    Copy_Local_u8Time[APP_MinuteS] = (Local_u32Timer % 3600) / 60;

    Copy_Local_u8Time[APP_Seconds] = ((Local_u32Timer % 3600) % 60);

    return;
    }

extern u8 APP_u8ReadSwitch(u8 Copy_u8SwitchId)
    {

    u8 local_u8CurrentSwitchState;

    u8 Local_u8Result;

    TACTILE_u8GetState(Copy_u8SwitchId, &local_u8CurrentSwitchState);

    if ((local_u8CurrentSwitchState == TACTILE_u8SWITCHRELEASED) && (APP_u8SwitchState[Copy_u8SwitchId] !=
    TACTILE_u8SWITCHRELEASED))
	{

	Local_u8Result = TACTILE_u8SWITCHPRESSED;

	}
    else
	{

	Local_u8Result = TACTILE_u8SWITCHRELEASED;

	}

    APP_u8SwitchState[Copy_u8SwitchId] = local_u8CurrentSwitchState;

    return Local_u8Result;

    }

extern void APP_voidDisplay(u8* Copy_u8Time)

    {

    char Local_u8TempChar[6];

//write to lcd

    LCD_voidWriteCommand(LCD_u8LINE1);

    itoa(Copy_u8Time[APP_HOURS] / 10, Local_u8TempChar, 10);
    LCD_voidWriteChar(Local_u8TempChar[0]);
    itoa(Copy_u8Time[APP_HOURS] % 10, Local_u8TempChar, 10);
    LCD_voidWriteChar(Local_u8TempChar[0]);

    LCD_voidWriteChar(':');

    itoa(Copy_u8Time[APP_MinuteS] / 10, Local_u8TempChar, 10);
    LCD_voidWriteChar(Local_u8TempChar[0]);
    itoa(Copy_u8Time[APP_MinuteS] % 10, Local_u8TempChar, 10);
    LCD_voidWriteChar(Local_u8TempChar[0]);

    LCD_voidWriteChar(':');

    itoa(Copy_u8Time[APP_Seconds] / 10, Local_u8TempChar, 10);
    LCD_voidWriteChar(Local_u8TempChar[0]);
    itoa(Copy_u8Time[APP_Seconds] % 10, Local_u8TempChar, 10);
    LCD_voidWriteChar(Local_u8TempChar[0]);

    if (Copy_u8Time[APP_u8AMPMFLAG] == APP_u8PM)
	{
	LCD_voidWriteChar(' ');
	LCD_voidWriteChar('P');
	LCD_voidWriteChar('M');

	}

    else if (Copy_u8Time[APP_u8AMPMFLAG] == APP_u8AM)
	{
	LCD_voidWriteChar(' ');
	LCD_voidWriteChar('A');
	LCD_voidWriteChar('M');

	}
    else
	{
	LCD_voidWriteChar(' ');
	LCD_voidWriteChar(' ');
	LCD_voidWriteChar(' ');

	}

    return;
    }

u8 APP_u8Timer(void)
    {

    u8 Local_u8Time[4];
    u8 Local_u8SwitchResult;

    do
	{

	APP_voidUpdateCounters();

	APP_Convert12HoursSystem(APP_u32Timer, &Local_u8Time);

	APP_voidDisplay(&Local_u8Time);

	Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8ModeSwitch);

	} while (Local_u8SwitchResult != TACTILE_u8SWITCHPRESSED);

    return APP_STOP_WATCH_STANDBY;

    }

u8 APP_u8APP_STOP_WATCH_STANDBY(void)
    {

    u8 Local_u8Time[4];
    u8 Local_u8SwitchResult;
    u8 Local_u8ReturnCase = APP_STOP_WATCH_STANDBY;

    do
	{

	APP_voidUpdateCounters();

	APP_Convert12HoursSystem(0, &Local_u8Time);

	Local_u8Time[3] = 'x';

	APP_voidDisplay(&Local_u8Time);

	Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8ModeSwitch);

	if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)

	    {

	    Local_u8ReturnCase = APP_EDIT_TIME;

	    }

	else
	    {

	    Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8Start_StopWatch);

	    if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)

		{

		Local_u8ReturnCase = APP_STOP_WATCH;

		}
	    else
		{
		}

	    }

	} while (Local_u8ReturnCase == APP_STOP_WATCH_STANDBY);

    return Local_u8ReturnCase;

    }

extern u8 APP_u8APP_STOP_WATCH(void)
    {

    u8 Local_u8Time[4];
    u8 Local_u8SwitchResult;
    u8 Local_u8ReturnCase = APP_STOP_WATCH;

    App_u8StopWatchFlag = APP_FLAGUP;

    do
	{

	APP_voidUpdateCounters();

	APP_Convert12HoursSystem(APP_u32StopWatch, &Local_u8Time);

	Local_u8Time[3] = 'x'; //any garbage to print ' ' instead of am or pm

	APP_voidDisplay(&Local_u8Time);

	Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8ModeSwitch);

	if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)

	    {

	    APP_u32StopWatch = 0;

	    App_u8StopWatchFlag = APP_FLAGDOWN;

	    Local_u8ReturnCase = APP_EDIT_TIME;

	    }

	else
	    {

	    Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8Start_StopWatch);

	    if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)

		{

		Local_u8ReturnCase = APP_STOP_WATCH_PAUSE;

		App_u8StopWatchFlag = APP_FLAGDOWN;

		}
	    else
		{
		}

	    }

	} while (Local_u8ReturnCase == APP_STOP_WATCH);

    return Local_u8ReturnCase;

    }

extern u8 APP_u8APP_STOP_WATCH_PAUSE(void)
    {

    u8 Local_u8Time[4];
    u8 Local_u8SwitchResult;
    u8 Local_u8ReturnCase = APP_STOP_WATCH_PAUSE;

    do
	{

	APP_voidUpdateCounters();

	APP_Convert12HoursSystem(APP_u32StopWatch, &Local_u8Time);

	Local_u8Time[3] = 'x'; //any garbage to print ' ' instead of am or pm

	APP_voidDisplay(&Local_u8Time);

	Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8ModeSwitch);

	if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)

	    {

	    APP_u32StopWatch = 0;

	    Local_u8ReturnCase = APP_EDIT_TIME;

	    }

	else
	    {

	    TACTILE_u8GetState(APP_u8Start_StopWatch, &Local_u8SwitchResult);

	    if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)

		{

		Local_u8ReturnCase = APP_RESUME_STAND_BY;

		App_u8TempCounterFlag = APP_FLAGUP;

		}
	    else
		{
		}

	    }

	} while (Local_u8ReturnCase == APP_STOP_WATCH_PAUSE);

    return Local_u8ReturnCase;

    }

extern u8 APP_u8APP_RESUME_STAND_BY(void)

    {

    u8 Local_u8Time[4];
    u8 Local_u8SwitchResult;
    u8 Local_u8ReturnCase = APP_RESUME_STAND_BY;

    u8 Local_u8TempChar[4];
    do
	{

	APP_voidUpdateCounters();

	APP_Convert12HoursSystem(APP_u32StopWatch, &Local_u8Time);

	Local_u8Time[3] = 'x'; //any garbage to print ' ' instead of am or pm

	APP_voidDisplay(&Local_u8Time);

	TACTILE_u8GetState(APP_u8Start_StopWatch, &Local_u8SwitchResult);

	if (Local_u8SwitchResult != TACTILE_u8SWITCHPRESSED)

	    {

	    App_u8TempCounterFlag = APP_FLAGDOWN;

	    if (APP_u32TempCounter >= APP_u8MaxSwitchTime)
		{

		Local_u8ReturnCase = APP_STOP_WATCH;

		APP_u32TempCounter = 0;

		}
	    else
		{

		Local_u8ReturnCase = APP_STOP_WATCH;

		APP_u32TempCounter = 0;

		APP_u161MilliSecondCounterStopWatch = 0;

		APP_u32StopWatch = 0;

		}

	    }

	else
	    {

	    }

	} while (Local_u8ReturnCase == APP_RESUME_STAND_BY);

    return Local_u8ReturnCase;

    }

//static u32 APP_u32Resume = 0;

//void APP_u8StopWatchUpdate(u8 Copy_u81MilliSecondFlag)
//    {
//
//    static u16 Local_u161SWMilliSecondCounter = 0;
//
//    Local_u161SWMilliSecondCounter += Copy_u81MilliSecondFlag;
//
//    if (Local_u161SWMilliSecondCounter >= 1000) //default 1000
//	{
//	Local_u161SWMilliSecondCounter -= 1000; //default 1000
//
//	APP_u32StopWatch++;
//
//	}
//    else
//	{
//
//	}
//
////overflow isn't handled
//    return;
//    }
//
//u8 APP_u8ReadSwitch(u8 Copy_u8SwitchId)
//    {
//
//    u8 local_u8CurrentSwitchState;
//
//    u8 Local_u8Result;
//
//    TACTILE_u8GetState(Copy_u8SwitchId, &local_u8CurrentSwitchState);
//
//    if ((local_u8CurrentSwitchState == TACTILE_u8SWITCHRELEASED) && (APP_u8SwitchState[Copy_u8SwitchId] !=
//    TACTILE_u8SWITCHRELEASED))
//	{
//
//	Local_u8Result = TACTILE_u8SWITCHPRESSED;
//
//	}
//    else
//	{
//
//	Local_u8Result = TACTILE_u8SWITCHRELEASED;
//
//	}
//
//    APP_u8SwitchState[Copy_u8SwitchId] = local_u8CurrentSwitchState;
//
//    return Local_u8Result;
//
//    }
//
//void APP_u8ResumeCounterUpdate(u8 Copy_u81MilliSecondFlag)
//    {
//
//    static u16 Local_u161MilliSecondCounter = 0;
//
//    Local_u161MilliSecondCounter += Copy_u81MilliSecondFlag;
//
//    if (Local_u161MilliSecondCounter >= 1000) //default 1000
//	{
//	Local_u161MilliSecondCounter -= 1000; //default 1000
//
//	APP_u32Resume++;
//
//	}
//    else
//	{
//
//	}
//
//    return;
//    }
//
//extern u32 APP_u32ReadResumeCounter(void)
//
//    {
//
//    return APP_u32Resume;
//
//    }
//
//extern void APP_voidEraseResumeCounter(void)
//
//    {
//
//    APP_u32Resume = 0;
//    return;
//
//    }
//
//extern void APP_voidEraseStopWatchCounter(void)
//
//    {
//
//    APP_u32StopWatch = 0;
//
//    return;
//
//    }
//
//extern u32 APP_u32ReadTimer(void)
//    {
//
//    return APP_u32Timer;
//    }
//
//extern void APP_voidEdittime(u32 Local_u32Timer)
//    {
//
//
//    u8 Local_u8TempChar[10];
//    u8 Local_u8Time[4];
//    u8 Local_u8Numpresses = 0;
//    u8 Local_u8SwitchResult;
//    u8 Local_u8Time_LIMITS[4]={12,59,59,1};
//
//    u8 Local_u81MilliSecondFlag;
//    //Local_u8Hours
//    Local_u8Time[0] = Local_u32Timer / 3600;
//
//
//    if (Local_u8Time[0]  > 12)
//    	    {
//
//	Local_u8Time[0]  = Local_u8Time[0]  - (12 * 60 * 60UL);
//
//	Local_u8Time[3]  = APP_u8PM;
//    	    }
//    	else
//    	    {
//
//    	Local_u8Time[3]  = APP_u8AM;
//
//    	    }
//
//
//    //Local_u8Minutes
//    Local_u8Time[1] = (Local_u32Timer % 3600) / 60;
//
//    //Local_u8Seconds
//    Local_u8Time[2] = ((Local_u32Timer % 3600) % 60);
//
//
//    do
//	{
//
//
//	////////////////// keep updating time
//
//
//	Local_u81MilliSecondFlag = APP_u8CheckFlag();
//
//		APP_u8TimeUpdate(Local_u81MilliSecondFlag);
//
//
//
//
//
//	//////////
//	Local_u8SwitchResult = APP_u8ReadSwitch(APP_u8ModeSwitch);
//
//	if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)
//
//	    {
//
//	    Local_u8Numpresses++;
//
//	    }
//
//	Local_u8SwitchResult = APP_u8ReadSwitch(TACTILE_u8SWITCH2);
//	if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)
//
//		    {
//
//		    Local_u8Time[Local_u8Numpresses]++;
//
//		    if(Local_u8Time[Local_u8Numpresses]>Local_u8Time_LIMITS[Local_u8Numpresses])
//			{
//
//			Local_u8Time[Local_u8Numpresses]=0;
//
//			}
//		    else{
//
//			}
//
//		    }
//
//	Local_u8SwitchResult = APP_u8ReadSwitch(TACTILE_u8SWITCH3);
//	if (Local_u8SwitchResult == TACTILE_u8SWITCHPRESSED)
//	    {
//
//
//	    if(Local_u8Time[Local_u8Numpresses]>0)
//	   			{
//
//		    Local_u8Time[Local_u8Numpresses]--;
//
//	   			}
//	    else{
//		Local_u8Time[Local_u8Numpresses]=Local_u8Time_LIMITS[Local_u8Numpresses];
//	    }
//
//	    ///////////////////////////////////
//	    }
//
//	    //write to lcd
//
//	        LCD_voidWriteCommand(LCD_u8LINE1);
//
//	        itoa( Local_u8Time[0] / 10, Local_u8TempChar, 10);
//	        LCD_voidWriteChar(Local_u8TempChar[0]);
//	        itoa(Local_u8Time[0] % 10, Local_u8TempChar, 10);
//	        LCD_voidWriteChar(Local_u8TempChar[0]);
//
//	        LCD_voidWriteChar(':');
//
//	        itoa(Local_u8Time[1] / 10, Local_u8TempChar, 10);
//	        LCD_voidWriteChar(Local_u8TempChar[0]);
//	        itoa(Local_u8Time[1] % 10, Local_u8TempChar, 10);
//	        LCD_voidWriteChar(Local_u8TempChar[0]);
//
//	        LCD_voidWriteChar(':');
//
//	        itoa(Local_u8Time[2] / 10, Local_u8TempChar, 10);
//	        LCD_voidWriteChar(Local_u8TempChar[0]);
//	        itoa(Local_u8Time[2] % 10, Local_u8TempChar, 10);
//	        LCD_voidWriteChar(Local_u8TempChar[0]);
//
//
//
//	        if (Local_u8Time[3]==APP_u8PM)
//	    	{
//	    	LCD_voidWriteChar(' ');
//	    	LCD_voidWriteChar('P');
//	    	LCD_voidWriteChar('M');
//
//	    	}
//
//	        else if (Local_u8Time[3]==APP_u8AM)
//	    	{
//	    	LCD_voidWriteChar(' ');
//	    	LCD_voidWriteChar('A');
//	    	LCD_voidWriteChar('M');
//
//	    	}
//
//
//	    //////////////////////////////////////////
//
//
//	} while (Local_u8Numpresses < 4);
//
//
//    return;
//    }
//
