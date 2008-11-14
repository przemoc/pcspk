#ifndef STR_H_
#define STR_H_

#define	str(name)	extern char *str_##name

str(nocdroot);
str(noconnect);
str(noconsolewriting);
str(nohost);
str(nomem);
str(noprocess);
str(nosetsid);
str(nosocket);
str(nosocketaccept);
str(nosocketbind);

#undef	str

#endif /* STR_H_ */
