#include "lcd_predefs.h"
#include "lcd.h"

// extern void LCD_clearScreen(LCD_TypeDef* lcd);

void scr_idle() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 2);
	LCD_putstr(global_lcd, (uint8_t*) "Personal Pacman");
	LCD_setCursor(global_lcd, 2, 4);
	LCD_putstr(global_lcd, (uint8_t*) "Scan package");
}

void scr_scan() {
	// TODO
	// This is for I2C scanning wait screen.
}

void scr_wait_password() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Enter Password:");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A]ccept [C]ancel");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[B]ackspace");
	LCD_setCursor(global_lcd, 1, 0);
}

void scr_wrongpwd() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Wrong Password.");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A] Try again");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[B] Exit");
}

void scr_mgmt() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Management");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[1] Pick up package");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[2] Settings");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[D] Exit");
}

void scr_openall() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Opening all cabinets.");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Please wait");
}

void scr_openall_done() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "All cabinets opened.");
	LCD_setCursor(global_lcd,1,0);
	LCD_putstr(global_lcd, (uint8_t*) "Close after done.");
}

void scr_chkpkg() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Checking info");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Please wait");
}

void scr_settings() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Settings");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[1] Change Password");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[2] Change Hardware");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[D] Quit");
}

void scr_reconf_hw() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "System disarmed.");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "+/- Cabinets now");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A] To finish");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[D] To cancel");
}

void scr_reconf_hw_detect() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Please wait");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Scanning hardware");
}

void scr_reconf_hw_done() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Reconfigure Done");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Returning to idle.");
}

void scr_mgmt_new_password() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "New password:");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A]ccept [C]ancel");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[B]ackspace");
	LCD_setCursor(global_lcd, 1, 0);
}

void scr_mgmt_new_password_done() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Password reset.");
}

void scr_function_choose() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Menu");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A] Drop Package");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[B] Management");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[C] Quit");
}

void scr_get_tracknum() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Enter package num.");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A]ccept [C]ancel");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[B]ackspace");
	LCD_setCursor(global_lcd, 1, 0);
}

void scr_deli_wait_choice() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Press the button on");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "the locker you want");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "to use.");
}

void scr_deli_okfail() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Is the delivery");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "successful?");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A] Yes  [B] No");
}

void scr_deli_if_repeat() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "Deliver another");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "package?");
	LCD_setCursor(global_lcd, 3, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A] Yes  [B] No");
}

void scr_alarm() {
	LCD_clearScreen(global_lcd);
	LCD_setCursor(global_lcd, 0, 0);
	LCD_putstr(global_lcd, (uint8_t*) "ALARM // Hardware");
	LCD_setCursor(global_lcd, 1, 0);
	LCD_putstr(global_lcd, (uint8_t*) "changed unexpectedly.");
	LCD_setCursor(global_lcd, 2, 0);
	LCD_putstr(global_lcd, (uint8_t*) "[A] Authenticate.");
}
