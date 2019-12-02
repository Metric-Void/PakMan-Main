#include "progui_fsm.h"
#include <stdbool.h>
#include "i2c_util.h"
#include "main.h"

bool armed;

void start_monitor();

void stop_monitor();

void timer_trigger();

void Alarm_Interrupt();
