//***************************************************************************
//  Author(s):    �������� �������� (http://de.my1.ru)
//  Target(s):    Encoder
//  Compiler:     GCC
//  Description:  ���������� ���������� ����� � ��������� �������
//  Data:         04.04.17
//***************************************************************************
// �������� �����: � ������ ����� ��������� ��� ���������.
//***************************************************************************
#ifndef __TM_H
#define __TM_H
#include <avr/io.h>
#include <avr/interrupt.h>

#define __TM_TaskQueueSize									20				// ������ ������� �����
#define __TS_TimerQueueSize									15				// ������ ��������� ������
// ������� ��������� ������
#define __TS_Timer0_TCNTDefault								0x83
#define __TS_ISR											TIMER0_OVF_vect

typedef void (*TPTR)(void);
//***************************************************************************
volatile uint8_t __TM_Start;												// ��������� ������ ������� �����
volatile uint8_t __TM_End;													// ��������� ����� ������� �����
volatile uint8_t __TM_TaskCnt;												// ������� �����
volatile static TPTR __TM_TaskQueue[__TM_TaskQueueSize];					// ���� ������� �����

volatile static struct
{
	TPTR Task;
	uint16_t Time;	
} __TS_TimerTaskQueue[__TS_TimerQueueSize];									// ������ �����
//***************************************************************************



/*������������� ���������� �����*/
extern void RTOS_Init(void);
/*������ ���������� �����*/
extern void RTOS_Run(void);
/*�������� ������ � ������� ���������� �����*/
extern void SendTask(TPTR Task);
/*�������� ������ � ������� ��������� ������*/
extern void SendTimerTask(TPTR Task, uint16_t Time);
/*���������� �������� ������*/
extern void UpdateTimerTask(TPTR Task, uint16_t Time);
/*�������� ������ �� ��������� ������*/
extern void RemoveTask (TPTR Task);
/*������ ���������� �����*/
extern void TaskManager(void);
/*������ ��������� ������*/
extern void TimerService(void);
/*���������� ���������� �������*/
ISR(__TS_ISR);
/*������ ��������� - ������� ����.*/
extern void Idle(void);

#endif