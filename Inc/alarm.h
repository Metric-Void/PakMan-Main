#include "progui_fsm.h"
#include <stdbool.h>
#include "i2c_util.h"
#include "main.h"
#include <stm32f407xx.h>
#include <stm32f4xx_it.h>

bool armed;
bool alarm_triggered;
bool alarm_handling;

void start_monitor();

void stop_monitor();

void timer_trigger();

void Alarm_Interrupt();
