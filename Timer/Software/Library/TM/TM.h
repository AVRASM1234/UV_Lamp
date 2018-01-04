//***************************************************************************
//  Author(s):    Владимир Иванишин (http://de.my1.ru)
//  Target(s):    Encoder
//  Compiler:     GCC
//  Description:  Библиотека диспетчера задач с таймерной службой
//  Data:         04.04.17
//***************************************************************************
// Описания файла: в данном файле находятся все настройки.
//***************************************************************************
#ifndef __TM_H
#define __TM_H
#include <avr/io.h>
#include <avr/interrupt.h>

#define __TM_TaskQueueSize									20				// Размер очереди задач
#define __TS_TimerQueueSize									15				// Размер таймерной службы
// Дефайны таймерной службы
#define __TS_Timer0_TCNTDefault								0x83
#define __TS_ISR											TIMER0_OVF_vect

typedef void (*TPTR)(void);
//***************************************************************************
volatile uint8_t __TM_Start;												// Указатель начала очереди задач
volatile uint8_t __TM_End;													// Указатель конца очереди задач
volatile uint8_t __TM_TaskCnt;												// Счетчик задач
volatile static TPTR __TM_TaskQueue[__TM_TaskQueueSize];					// Сама очередь задач

volatile static struct
{
	TPTR Task;
	uint16_t Time;	
} __TS_TimerTaskQueue[__TS_TimerQueueSize];									// Массив задач
//***************************************************************************



/*Инициализация диспетчера задач*/
extern void RTOS_Init(void);
/*Запуск диспетчера задач*/
extern void RTOS_Run(void);
/*Отправка задачи в очередь диспетчера задач*/
extern void SendTask(TPTR Task);
/*Отправка задачи в очередь таймерной службы*/
extern void SendTimerTask(TPTR Task, uint16_t Time);
/*Обновление тайминга задачи*/
extern void UpdateTimerTask(TPTR Task, uint16_t Time);
/*Удаление задачи из таймерной службы*/
extern void RemoveTask (TPTR Task);
/*Запуск диспетчера задач*/
extern void TaskManager(void);
/*Запуск таймерной службы*/
extern void TimerService(void);
/*Прерывание системного таймера*/
ISR(__TS_ISR);
/*Пустая процедура - простой ядра.*/
extern void Idle(void);

#endif