#ifndef __LCDPREDEFS_H__
#define __LCDPREDEFS_H__

#include "lcd.h"
#include "progui_fsm.h"

void scr_idle();

void scr_wait_password();

void scr_wrongpwd();

void scr_mgmt();

void scr_openall();

void scr_openall_done();

void scr_chkpkg();

void scr_reconf_hw();

void scr_settings();

void scr_reconf_hw_detect();

void scr_reconf_hw_done();

void scr_mgmt_new_password();

void scr_mgmt_new_password_done();

void scr_function_choose();

void scr_get_tracknum();

void scr_deli_wait_choice();

void scr_deli_okfail();

void scr_deli_if_repeat();

void scr_alarm();

#endif
