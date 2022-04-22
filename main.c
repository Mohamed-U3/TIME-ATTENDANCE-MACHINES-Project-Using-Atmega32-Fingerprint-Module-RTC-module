/*
 * main.c
 *
 * Created: 3/23/2022 8:09:16 PM
 * last modification : 4/14/2022 1:49:16 AM
 *  Author: Mohamed U3
 */
#define F_CPU 12000000UL
#include <xc.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "inc/LCD_interface.h"
#include "inc/UART_interface.h"
#include "inc/FP_interface.h"
#include "inc/keys_interface.h"
//I2C & RTC are token from https://www.electronicwings.com/avr-atmega/real-time-clock-rtc-ds1307-interfacing-with-atmega16-32 but modified
#include "inc/I2C_interface.h"
#include "inc/RTC_interface.h"

void show_attendance()
{
	char buffer[20];
	char show_ID = GetID();
	show_ID = show_ID*6;
	second	= eeprom_read_byte ((const uint8_t*) show_ID++);
	minute	= eeprom_read_byte ((const uint8_t*) show_ID++);
	hour	= eeprom_read_byte ((const uint8_t*) show_ID++);
	date	= eeprom_read_byte ((const uint8_t*) show_ID++);
	month	= eeprom_read_byte ((const uint8_t*) show_ID++);
	year	= eeprom_read_byte ((const uint8_t*) show_ID);
	
	sprintf(buffer, "ID:%d  @ %02x:%02x:%02x", (show_ID/6),hour, minute, second);
	lcdgotoxy(1,1);
	lcdprint(buffer);
	sprintf(buffer, "Date: %02x/%02x/20%02x ", date, month, year);
	lcdgotoxy(2,1);
	lcdprint(buffer);
	_delay_ms(10000);
}

int main()
{
	//eeprom_busy_wait();
	UART_InterrptInit(57600);
	lcdbegin();
	keys_init();
	I2C_Init();
	
	lcdclear();
	lcdprint("Loading.. ");
	_delay_ms(500);
	sendcmd2fb(FB_connect);
	_delay_ms(500);
	unsigned char options_conter = 0;
	unsigned char key_Feedback = keys_Feedback();
	lcdclear();
	int display_counter = 0;
	
	while(1)
	{
		while(key_Feedback == keys_non)
		{
			RTC();
			display_counter = 0;
			key_Feedback = keys_Feedback();
		}
		
		_delay_ms(200);
		lcdclear();
		
		while(key_Feedback != keys_non)
		{
			lcdfirstrow();
			lcdprint("Options: ");
			lcdwrite(0x7f,DATA);
			lcdwrite(0x7e,DATA);
			lcdprint("     ");
			
			if(keys_Feedback() == keys_up)
			{
				options_conter++;
				_delay_ms(150);
				display_counter = 0;
			}
			else if(keys_Feedback() == keys_down)
			{
				options_conter--;
				_delay_ms(150);
				display_counter = 0;
			}
			else if(keys_Feedback() == keys_ok)
			{
				if(options_conter == 0)
				{
					unsigned char __ID = Search();
					RTC_Read_Clock(0);
					RTC_Read_Calendar(3);
					__ID = __ID * 6;
					eeprom_update_byte (( uint8_t *) __ID++,second);
					eeprom_update_byte (( uint8_t *) __ID++,minute);
					eeprom_update_byte (( uint8_t *) __ID++,(hour&0b00111111));
					eeprom_update_byte (( uint8_t *) __ID++,date);
					eeprom_update_byte (( uint8_t *) __ID++,month);
					eeprom_update_byte (( uint8_t *) __ID,year);
				}
				else if(options_conter == 1)
				enroll();
				else if(options_conter == 2)
				sendcmd2fb(F_delete);
				else if(options_conter == 3)
				sendcmd2fb(FP_empty);
				else if(options_conter == 4)
				show_attendance();
			}
			else if (keys_Feedback() == keys_cancel)
			{
				key_Feedback = keys_non;
			}
			
			if(options_conter > 4)
			options_conter = 0;
			
			lcdsecondrow();
			if(options_conter == 0)
			lcdprint("1-Scan           ");
			else if(options_conter == 1)
			lcdprint("2-Enroll         ");
			else if(options_conter == 2)
			lcdprint("3-Delete ID      ");
			else if(options_conter == 3)
			lcdprint("4-Erase IDs      ");
			else if(options_conter == 4)
			lcdprint("5-show attendance");
			
			
			display_counter++;
			if(display_counter >= 100)
			key_Feedback = keys_non;
		}
		_delay_ms(200);
	}
}