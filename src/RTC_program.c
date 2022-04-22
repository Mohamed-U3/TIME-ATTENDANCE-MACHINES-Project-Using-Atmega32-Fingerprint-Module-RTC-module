/*
 * RTC_program.c
 *
 * Created: 4/15/2022 1:08:22 AM
 *  Author: mmyra
 */
#include "../inc/RTC_interface.h"

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address		0xD1	/* Make LSB bit high of slave address for read */
#define TimeFormat12			0x40	/* Define 12 hour format */
#define AMPM					0x20

bool IsItPM(char hour_)
{
	if(hour_ & (AMPM))
	return 1;
	else
	return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(read_clock_address);	/* Write address to read */
	I2C_Repeated_Start(Device_Read_address);/* Repeated start with device read address */

	second = I2C_Read_Ack();	/* Read second */
	minute = I2C_Read_Ack();	/* Read minute */
	hour = I2C_Read_Nack();		/* Read hour with Nack */
	I2C_Stop();			/* Stop i2C communication */
}

void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();		/* Read day */
	date = I2C_Read_Ack();		/* Read date */
	month = I2C_Read_Ack();		/* Read month */
	year = I2C_Read_Nack();		/* Read the year with Nack */
	I2C_Stop();			/* Stop i2C communication */
}

void RTC()
{
	char buffer[20];
	const char* days[7]= {"Thu","Fri","Sat","Sun","Mon","Tue","Wed"};
	RTC_Read_Clock(0);	/* Read clock with second add. i.e location is 0 */
	if (hour & TimeFormat12)
	{
		sprintf(buffer, "Time: %02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
		if(IsItPM(hour))
		strcat(buffer, "PM");
		else
		strcat(buffer, "AM");
		lcdgotoxy(1,1);
		lcdprint(buffer);
	}
	
	else
	{
		sprintf(buffer, "Time: %02x:%02x:%02x  ", (hour & 0b00111111), minute, second);
		lcdgotoxy(1,1);
		lcdprint(buffer);
	}
	RTC_Read_Calendar(3);	/* Read calendar with day address i.e location is 3 */
	sprintf(buffer, "Date:%02x/%02x/%02x%3s ", date, month, year,days[day]);
	lcdgotoxy(2,1);
	lcdprint(buffer);
}