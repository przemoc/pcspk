#ifndef _STR_H
#define _STR_H

#define str(name)	extern char *str_##name

str(nocdroot);
str(noconnect);
str(noconsolewriting);
str(nomem);
str(nohost);
str(noprocess);
str(nosocket);
str(nosocketaccept);
str(nosocketbind);

#undef str

#endif
