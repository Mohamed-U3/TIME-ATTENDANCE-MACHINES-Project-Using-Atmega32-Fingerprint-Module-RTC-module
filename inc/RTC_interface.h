/*
 * RTC_interface.h
 *
 * Created: 4/15/2022 1:09:40 AM
 *  Author: mmyra
 */ 


#ifndef RTC_INTERFACE_H_
#define RTC_INTERFACE_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../inc/I2C_interface.h"
#include "../inc/LCD_interface.h"

char second,minute,hour,day,date,month,year;

bool IsItPM(char hour_);
void RTC_Read_Clock(char read_clock_address);			//address 0
void RTC_Read_Calendar(char read_calendar_address);		//address 3
void RTC();

#endif /* RTC_INTERFACE_H_ */