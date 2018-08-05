//***************************************************************************
//  Author(s):    �������� �������� (http://de.my1.ru)
//  Target(s):    LCD from NOKIA5110
//  Compiler:     GCC
//  Description:  ���������� ������� �� �������� NOKIA5110
//  Data:         12.12.17
//***************************************************************************
// �������� �����: � ������ ����� ��������� ��� ��������� �������.
//***************************************************************************
#ifndef __NOKIA5110_H
#define __NOKIA5110_H

#define F_CPU			8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define __LCD5110_Port	PORTD
#define __LCD5110_DDR	DDRD
#define __LCD5110_R		4
#define __LCD5110_CE	3
#define __LCD5110_DC	2
#define __LCD5110_DIN	1
#define __LCD5110_CLK	0

/* ������������� ������� */
void LCD5110_Init(void);
/* ������� ������� */
void LCD5110_Clear(void);
/* ��������� ������ ��������� ������� */
void LCD5110_SetXY(unsigned char X, unsigned char Y);
/* ����� ������ �� ������� */
void LCD5110_Prints(char *s, unsigned char X, unsigned char Y);
/* ����� ������� �� ������� */
void LCD5110_Putc(unsigned char c);
/* �������� ����� �� ������� */
void LCD5110_SendByte(unsigned char data, unsigned char DC);



void HeaderPrints(char *s);
void LCD5110_LargeNumPrints(char *s, unsigned char X, unsigned char Y);

#endif