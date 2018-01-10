/*
 * Software.c
 *
 * Created: 04.01.2018 19:19:17
 * Author : AVRASM1234
 */ 

#define F_CPU	8000000UL

#define TimerDDR		DDRB
#define TimerPort		PORTB
#define TimerNPin		0

#define TimerON()		PORTB |= (1<<TimerNPin)
#define TimerOFF()		PORTB &= ~(1<<TimerNPin)

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
																				//  Time[0] - часы
																				//  Time[1] - минуты
																				//  Time[2] - секунды
																				
unsigned char Settings[3];														// Настройки таймера:
																				//  Settings[0] - задержка
																				//  Settings[1] - яркость подсветки
																				//  Settings[2] - звук

unsigned char TimerState = 0;													// Состояние таймера:
																				//  0 - режим ожидания
																				//  1 - рабочий режим
																				//  2 - пауза
unsigned char TimerDelay;													
unsigned char TimerTemp;					
unsigned char DisplayN = 0;														// Местонахождение в древе меню
unsigned char RSeg = 0;															// Бит сброса сегмента

/*Обновление содержимого дисплея*/
void DisplayUpdate(void);
/*Вывод динамических параметров на дисплей*/
void PrintParameters(void);

void TimerStart(void);
void TimerPause(void);
void TimerStop(void);

void TimerStep(void);

/*Считывание и запись параметров таймера*/
inline void ReadTime(void);
inline void ReadSettings(void);
inline void WriteTime(void);
inline void WriteSettings(void);
/*Мерцание сегментов*/
void MirrorDigit(void);


int main(void)
{
	TimerDDR |= (1<<TimerNPin);
	TimerPort &= ~(1<<TimerNPin);
	RTOS_Init();
	RTOS_Run();
	
	ReadSettings();
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
	LCD5110_SetXY(0, 3);
	for (unsigned int i = 0; i < 84*3; i++)
		LCD5110_SendByte(0, 1);
		
	switch (DisplayN/10)
	{
		case 0:
		{
			HeaderPrints("<Режим #1>");
			break;
		}
		case 1:
		{
			HeaderPrints("<Режим #2>");
			break;
		}
		case 2:
		{
			HeaderPrints("<Режим #3>");
			break;
		}
		case 3:
		{
			HeaderPrints("<Режим #4>");
			break;
		}
		case 4:
		{
			HeaderPrints("<Режим #5>");
			break;
		}
		case 5:
		{
			HeaderPrints("<Настройки>");
			LCD5110_Prints("Задержка     с", 0, 3);
			LCD5110_Prints("Яркость      %", 0, 4);
			LCD5110_Prints("Звук", 0, 5);
			break;
		}
	}
	
	PrintParameters();
}



/*Объедени DisplayUpdate и PrintParameters*/
void PrintParameters(void)
{	
	char str[9];
	unsigned char i;
	
	if (DisplayN<50)
	{
		for (i = 0; i < 3; i++)
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
		for (unsigned char NumPar = 0; NumPar < 2; NumPar++)
		{
			if ((DisplayN%10 != (NumPar+1))||(RSeg != 1))
			{
				str[0] = (Settings[NumPar] / 100) + '0';
				str[1] = ((Settings[NumPar] / 10) % 10)  + '0';
				str[2] = (Settings[NumPar] % 10)  + '0';
				str[3] = 0;
					
				i = 0;
				while ((i < 2)&&(str[i] == '0'))
				{
					str[i] = ' ';
					i++;
				}
				LCD5110_Prints(str, 9*6, 3 + NumPar);
			}
			else
			{
				LCD5110_Prints("   ", 9*6, 3 + NumPar);
			}
			
		}			
		
		// Вывод параметров звука	
		if ((DisplayN != 53)||(RSeg != 1))
		{
			if (Settings[2])
				LCD5110_Prints(" Вкл.", 9*6, 5);
			else
				LCD5110_Prints("Выкл.", 9*6, 5);
		}
		else
		{
			LCD5110_Prints("     ", 9*6, 5);
		}
	}
}



//***************************************************************************
// Энкодер вращается по часовой стрелке
//***************************************************************************
void ENC_Inc(void)
{
//Здесь может быть ваш код ;)
	if (TimerState == 0)
	{
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
			SendTask(DisplayUpdate);
		}
		else
		{
			unsigned char i = DisplayN%10 - 1;
		
			if (DisplayN < 50)
			{
				i = 2-i;
				Time[i]++;
				if (i != 0) 
				{
					if (Time[i] == 60)
						Time[i] = 0;
				}
				else
				{
					if (Time[i] == 100)
						Time[i] = 0;
				}
			} 
			else
			{
				switch (i)
				{
					case 0:
					{
						Settings[0]++;
						break;
					}
					case 1:
					{
						Settings[1] += 10;
						if (Settings[1] > 100)
							Settings[1] = 0;
					 
						break;
					}
					case 2:
					{
						Settings[2] ^= (1<<0);
						break;
					}
				}
			}
			RSeg = 0;
			UpdateTimerTask(MirrorDigit, 500);
		
			SendTask(PrintParameters);
		}
	}
}



//***************************************************************************
// Энкодер вращается против часовой стрелки
//***************************************************************************
void ENC_Dec(void)
{
//Здесь может быть ваш код ;)
	if(TimerState == 0)
	{
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
			SendTask(DisplayUpdate);
		}
		else
		{
			unsigned char i = DisplayN%10 - 1;
		
			if (DisplayN < 50)
			{
				i = 2-i;
				Time[i]--;
				if (Time[i] == 255)
				{
					if (i != 0)
						Time[i] = 59;
					else
						Time[i] = 99;
				}
			}
			else
			{
				switch (i)
				{
					case 0:
					{
						Settings[0]--;
						break;
					}
					case 1:
					{
						Settings[1] -= 10;
						if (Settings[1] > 100)
						Settings[1] = 100;
					
						break;
					}
					case 2:
					{
						Settings[2] ^= (1<<0);
						break;
					}
				}
			}
			RSeg = 0;
			UpdateTimerTask(MirrorDigit, 500);
		
			SendTask(PrintParameters);
		}
	}
}



//***************************************************************************
// Короткое нажатие кнопки
//***************************************************************************
void ENC_ShortPress(void)
{
	if (DisplayN%10 == 0)
	{
		if (DisplayN < 50)
		{
			if (TimerState == 1)
				TimerPause();
			else
				TimerStart();
		}
	}
	else
	{
		DisplayN++;
		if (DisplayN%10 == 4)
		{
			DisplayN = DisplayN - 3;
		}
		//RSeg = 0;
		//UpdateTimerTask(MirrorDigit, 500);
	}
}



//***************************************************************************
// Длинное нажатие кнопки
//***************************************************************************
void ENC_LongPress(void)
{
	if (TimerState == 0)
	{
		if (DisplayN%10 == 0)
		{
			DisplayN++;
			RSeg = 0;
			SendTimerTask(MirrorDigit, 500);
		}
		else
		{
			DisplayN = DisplayN - DisplayN%10;
			if (DisplayN < 50)
				WriteTime();
			else
				WriteSettings();
			RSeg = 0;
			RemoveTask(MirrorDigit);
			HeaderPrints("Сохранено");
			SendTimerTask(DisplayUpdate, 1000);
			SendTask(PrintParameters);
		}
	}
	else
	{
		TimerStop();
		SendTask(PrintParameters);
	}
}


void TimerStart(void)
{
	
	if (TimerState == 0)
	{
		TimerDelay = Settings[0];
		SendTimerTask(TimerStep, 1000);
	}
	else
	{
		if (TimerState == 2)
			SendTimerTask(TimerStep, TimerTemp);
	}
	
	TimerState = 1;
	if (TimerDelay == 0)
		TimerON();
}

void TimerPause(void)
{
	if (TimerState == 1)
	{
		TimerOFF();
		TimerTemp = RemoveTask(TimerStep);
		TimerState = 2;
	}
}

void TimerStop(void)
{
	TimerOFF();
	if (TimerState == 1)
		RemoveTask(TimerStep);
	TimerState = 0;	
	ReadTime();
}

void TimerStep(void)
{
	SendTimerTask(TimerStep, 1000);
	SendTask(PrintParameters);
	if(TimerDelay == 0)
	{
		if (Time[2])
		{
			Time[2]--;
			if ((Time[0] == 0) && (Time[1] == 0) && (Time[2] == 0))
				TimerStop();
		}
		else
		{
			Time[2] = 59;
			
			if (Time[1])
				Time[1]--;
			else
			{
				Time[1] = 59;
				if (Time[0])
					Time[0]--;
			}
		}
	}
	else
	{
		TimerDelay--;
		if (TimerDelay == 0)
			TimerON();
	}
}

/*Считывание и запись параметров таймера*/
inline void ReadTime(void)
{
	for (unsigned char i=0; i<3; i++)
	{
		Time[i] = eeprom_read_byte(&EE_TimeModes[DisplayN/10][i]);
	}
}

inline void ReadSettings(void)
{
	for (unsigned char i=0; i<3; i++)
	{
		Settings[i] = eeprom_read_byte(&EE_TimerSettings[i]);
	}
}

inline void WriteTime(void)
{
	for (unsigned char i=0; i<3; i++)
	{
		eeprom_write_byte(&EE_TimeModes[DisplayN/10][i], Time[i]);
	}
}

inline void WriteSettings(void)
{
	for (unsigned char i=0; i<3; i++)
	{
		eeprom_write_byte(&EE_TimerSettings[i], Settings[i]);
	}
}


void MirrorDigit(void)
{
	RSeg ^= (1<<0);
	SendTask(PrintParameters);
	SendTimerTask(MirrorDigit, 500);
}