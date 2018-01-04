//***************************************************************************
//  Author(s):    �������� �������� (http://de.my1.ru)
//  Target(s):    TaskManager
//  Compiler:     GCC
//  Description:  ���������� ���������� ����� � ��������� �������
//  Data:         04.04.17
//***************************************************************************
// �������� �����: � ������ ����� ��������� ��� ������� ������ ����������.
//***************************************************************************
#include "TM.h"
//***************************************************************************



//***************************************************************************
// ������������� ���������� �����
//***************************************************************************
inline void RTOS_Init(void)													
{
	// �������������� ��������� �����
	__TM_Start = 0;															// �������� ��������� ������ �������
	__TM_End = 0;															// �������� ��������� ����� �������
	__TM_TaskCnt = 0;														// ���������� ������� �����
	
	// �������������� ��������� ������
	for (uint8_t i = 0; i < __TS_TimerQueueSize; i++)
	{
		__TS_TimerTaskQueue[i].Task = Idle;									// ��������� ��������� ���������
		__TS_TimerTaskQueue[i].Time = 0;									// �������� ��������� ���������
	}
}



//***************************************************************************
// ������ ���������� �����
//***************************************************************************
inline void RTOS_Run(void)
{
	cli();																	// ��������� ����������
																			// ����������� ������. � ���� TIMER0, ������ ��� �� ������ �� �����.
																			// �� �� ������ ����� ���-������ ��������, ���� 16-������ TIMER1.
	TCCR0 &= ~((1<<CS02)|(1<<CS02)|(1<<CS02));								// ������� ���� ������������.
	TCCR0 |= ((1<<CS01)|(1<<CS00));											// � ������ ������������ 64.
	TCNT0 = __TS_Timer0_TCNTDefault;										// �������������� �������� ��������
	TIMSK |= (1<<TOIE0);													// � ������� ���������� �� ������������.
	
	
	sei();																	// ������� ���������� ���������� � �����.
}



//***************************************************************************
// �������� ������ � ������� ���������� �����
//***************************************************************************
void SendTask(TPTR Task)
{
	uint8_t noint = 0;
	if (SREG&(1<<SREG_I))													// ���� �� �� ��������� � ����������...
	{
		cli();																// ...�� ��������� ����������
		noint = 1;															// � ������� ������, ��� �� �� � ����������, � ���� ��������� �� �� ������.
	}
	
	if (__TM_TaskCnt < __TM_TaskQueueSize)									// ���� � ������� ���� ��������� �����...
	{
		__TM_TaskQueue[__TM_End] = Task;									// ...�� ����� � ���� ���� ������
		__TM_End++;															// � ������� ��� ��������� �� ����� �������.
		if (__TM_End == __TM_TaskQueueSize)									// ���� ����� �� ����� �������, 
			__TM_End = 0;													// �� ���������� ���������.
		__TM_TaskCnt++;
		
		if (noint) sei();													// ���� �� �� � ���������� - ��������� ����������.
	}
	else																	// ...�����, ���� ������� ����� ���...
	{
		if (noint) sei();													// ...��������� ����������, ���� �� �� � ����������.
		return;																// � �������. ����� ���������� ��� ������ �����-������. 
																			// �� ��� ���� ���� ��� ������... :)
	}
}



//***************************************************************************
// �������� ������ � ������� ��������� ������
//***************************************************************************
void SendTimerTask(TPTR Task, uint16_t Time)
{
	uint8_t i = 0;															// �������������� ����������
	uint8_t noint = 0;
	if (SREG&(1<<SREG_I))													// ���� �� �� ��������� � ����������...
	{
		cli();																// ...�� ��������� ����������
		noint = 1;															// � ������� ������, ��� �� �� � ����������, � ���� ��������� �� �� ������.
	}	
	
	while (__TS_TimerTaskQueue[i].Task != Idle)								// ����������� ������� �� ������� ���������� �����,
	{
		i++;																// �� ������� �������� ������ ������.
		if (i == __TS_TimerQueueSize)										// ���� ����� �� �����...
		{
			if (noint) sei();												// ...��������� ����������, ���� �� �� � ����������.
			return;															// � �������. ����� ���������� ��� ������ �����-������.
																			// �� ��� ���� ���� ��� ������... :)
		}
	}
	
	if (noint) sei();														// ���� �� �� � ���������� - ��������� ����������.
																			// ���� ����� ��������� ����� -
	__TS_TimerTaskQueue[i].Task = Task;										// ����� ���� ������
	__TS_TimerTaskQueue[i].Time = Time;										// � �����, ����� ������� �� ����� ����� ���������.
}



//***************************************************************************
// ������ ���������� �����
//***************************************************************************
inline void TaskManager(void)
{
	TPTR Task;																// �������������� ����������
	
	cli();																	// ��������� ����������
	
	if (__TM_TaskCnt != 0)													// ���� ���� ������ �� ���������...
	{
		//PORTB |= (1<<3);													// ��� ������ ������������� ����������,
																			// ��� ��������� ����� ������������� ��� ����� � 1.
																			// � ����� ��� ������ ��������� - ���������� � 0.
																			
		Task = __TM_TaskQueue[__TM_Start];									// ... �� ����� �� �� �������
		__TM_Start++;														// � ������� ��������� ������ �������.
		if (__TM_Start == __TM_TaskQueueSize)								// ���� ����� �� ����� �������,
			__TM_Start = 0;													// �� ���������� ���������.
		__TM_TaskCnt--;
		sei();																// ��������� ����������.
		
		(Task)();															// � ����� ��������� ���� ������.
	}
	/*else
	{
		PORTB &= ~(1<<3);													// ��� ������ ������������� ����������,
	}*/																		// ��� ��������� ����� ������������� ��� ����� � 1.
																			// � ����� ��� ������ ��������� - ���������� � 0.
	
	sei();																	// ��������� ����������.
}



//***************************************************************************
// ������ ��������� ������
//***************************************************************************
inline void TimerService(void)
{
	for (uint8_t i = 0; i < __TS_TimerQueueSize; i++)						// ����������� �������
	{
		if (__TS_TimerTaskQueue[i].Task == Idle) continue;					// ���� ���������� �� ������� - ��������� � ��������� ������.
		if (__TS_TimerTaskQueue[i].Time != 0)								// ���� ���������� �� ���������� ������ � �� ����� ��� �� �������...
		{
			__TS_TimerTaskQueue[i].Time--;									// ...�������������� �� ��������� ��������
		}
		else																// ...�����, ���� ������ ����� ��������� ������...
		{
			SendTask(__TS_TimerTaskQueue[i].Task);							// ...����� ��� ������ � ��������� �����
			__TS_TimerTaskQueue[i].Task = Idle;								// � ����� �� �� ����� �������.
		}
	}
}



//***************************************************************************
// ���������� ���������� �������
//***************************************************************************
ISR(__TS_ISR)
{
	TCNT0 = __TS_Timer0_TCNTDefault;										// ���������� �������� ��������
	
	TimerService();															// �������� ��������� ������
}



//***************************************************************************
// ������ ��������� - ������� ����
//***************************************************************************
inline void Idle(void)
{

}