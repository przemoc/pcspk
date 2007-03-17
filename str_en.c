#define str(name, val)	char *str_##name = #val

str(nocdroot, "Unable to change current directory to root /");
str(noconnect, "Unable to connect");
str(noconsolewriting, "Could not open console for writing");
str(nomem, "Not enough memory\n");
str(nohost, "Unable to get informations about host name");
str(noprocess, "Unable to create process");
str(nosocket, "Unable to create socket");
str(nosocketaccept, "Unable to accept socket");
str(nosocketbind, "Unable to bind socket");

#undef str
