/*
 * Software.c
 *
 * Created: 04.01.2018 19:19:17
 * Author : AVRASM1234
 */ 

#define F_CPU	8000000UL

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "Library/NOKIA5110/NOKIA5110.h"
#include "Library/TM/TM.h"
#include "Library/TM/ENC.h"



unsigned char EE_TimeModes[5][3] EEMEM =
{
	{  0,  0, 30 },
	{  0,  1,  0 },
	{  0,  1, 30 },
	{  0,  2,  0 },
	{  0,  2, 30 }																// Ячейки хранения содержимого наших пресетов
};
unsigned char EE_TimerSettings[3] EEMEM = {0, 10, 1};							// Ячейки хранения настроек таймера
	
unsigned char Time[3];															// Время таймера
unsigned char Setting[3];														// Настройки таймера
//unsigned char TimerDalay, Bright, Sound;
unsigned char DisplayN = 0, RSeg = 0;											// Местонахождение в древе меню. Бит сброса сегмента.


/*Обновление содержимого дисплея*/
void DisplayUpdate(void);
/*Вывод динамических параметров на дисплей*/
void PrintParameters(void);
/*Прочитать время из ячейки пресета*/
void ReadTime(void);


int main(void)
{
	RTOS_Init();
	RTOS_Run();
	
	ReadTime();
	
	ENC_Init();
	
	LCD5110_Init();
	LCD5110_Clear();
	DisplayUpdate();
    while (1)
    {
		TaskManager();
	}
}



void DisplayUpdate(void)
{
	LCD5110_Clear();
	switch (DisplayN/10)
	{
		case 0:
		{
			HeaderPrints("<Режим #1>");
			PrintParameters();
			break;
		}
		case 1:
		{
			HeaderPrints("<Режим #2>");
			PrintParameters();
			break;
		}
		case 2:
		{
			HeaderPrints("<Режим #3>");
			PrintParameters();
			break;
		}
		case 3:
		{
			HeaderPrints("<Режим #4>");
			PrintParameters();
			break;
		}
		case 4:
		{
			HeaderPrints("<Режим #5>");
			PrintParameters();
			break;
		}
		case 5:
		{
			HeaderPrints("<Настройки>");
			//LCD5110_LargeNumPrints("        " , 10, 3);
			LCD5110_Prints("Задержка  59 с" ,0 , 3);
			LCD5110_Prints("Яркость  100 %" ,0 , 4);
			LCD5110_Prints("Звук      Вкл." ,0 , 5);
			break;
		}
	}
}



/*Объедени DisplayUpdate и PrintParameters*/
void PrintParameters(void)
{	
	char str[9];
	unsigned char ZeroInd = 0;
	if (DisplayN<50)
	{
		for (unsigned char i=0; i<3; i++)
		{
			str[3*i] = Time[i]/10 + '0';
			str[3*i+1] = Time[i]%10 + '0';
			str[3*i+2] = ':';
		}
		str[8] = 0;
		
		if ((DisplayN%10) && (RSeg))
		{
			str[9 - (DisplayN%10)*3] = ' ';
			str[10 - (DisplayN%10)*3] = ' ';
		}
		LCD5110_LargeNumPrints(str, 10, 3);		
	}
	else
	{
		for (unsigned char i=0; i<3; i++)
		{
			if ()
			{
			}
		}
		if (ZeroInd)
		{
			str[0] = Setting[0]/100+'0';
			
			
		
		// Вывод параметров звука	
		if (Setting[2])
			LCD5110_Prints(" Вкл.", 7, 5);
		else
			LCD5110_Prints("Выкл.", 7, 5);
	}
}



inline void ReadTime(void)
{
	for (unsigned char i=0; i<3; i++)
		Time[i] = eeprom_read_byte(&EE_TimeModes[DisplayN/10][i]);
}



void MirrorDigit(void)
{
	//// Мерцание сегмента
}

//***************************************************************************
// Энкодер вращается по часовой стрелке
//***************************************************************************
void ENC_Inc(void)
{
//Здесь может быть ваш код ;)
	if (DisplayN%10 == 0)
	{
		if (DisplayN == 50)
			DisplayN = 0;	
		else
			DisplayN += 10;
		if (DisplayN != 50)
		{
			ReadTime();
		}
	}

	SendTask(DisplayUpdate);
}

//***************************************************************************
// Энкодер вращается против часовой стрелки
//***************************************************************************
void ENC_Dec(void)
{
//Здесь может быть ваш код ;)
	if (DisplayN%10 == 0)
	{
		if (DisplayN == 0)
			DisplayN = 50;
		else
			DisplayN -= 10;
			
		if (DisplayN != 50)
		{
			ReadTime();
		}
	}

	SendTask(DisplayUpdate);
}

//***************************************************************************
// Короткое нажатие кнопки
//***************************************************************************
void ENC_ShortPress(void)
{
	if (DisplayN%10 == 0)
	{
		// Пуск таймера
	}
	else
	{
		DisplayN++;
		if (DisplayN%10 == 4)
		{
			DisplayN = DisplayN - 3;
		}
	}
}

//***************************************************************************
// Длинное нажатие кнопки
//***************************************************************************
void ENC_LongPress(void)
{
	if (DisplayN%10 == 0)
	{
		DisplayN++;
	}
	else
	{
		DisplayN = DisplayN - DisplayN%10;
	}
}