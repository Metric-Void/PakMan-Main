#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <string.h>

#define __CONF_PASWD "991218"
#define __CONF_PWDLEN 16

bool passmatch(char* x);

void changepwd(char* x);

#endif
