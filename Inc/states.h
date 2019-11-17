#ifndef __STATES_H__
#define __STATES_H__

void state_sys_idle();

void state_sys_auth();

void state_idleauth_wrongpwd();

void state_management();

void state_openall();

void state_openall_done();

void state_settings();

void state_hwreconf();

void state_hwreconf_detect();

void state_hwreconf_done();

void state_mgmt_new_password();

void state_mgmt_new_password_done();

void state_choose_kbop();

void state_pkg_drop();

void state_deli_wait_choice();

void state_deli_query();

void state_deli_ifrepeat();
#endif
