#include <config.h>
#include <string.h>

char password_internal[] = __CONF_PASWD;

extern int strcmp(const char *str1, const char *str2);

bool passmatch(char* x) {
	return (strcmp(x,password_internal)==0);
}

void changepwd(char x[]) {
	memcpy(password_internal,x,sizeof(password_internal));
}
