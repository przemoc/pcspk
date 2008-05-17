#define	str(name, val)	char *str_##name = val

#ifdef PSTR_CHDIR_ROOT
str(nocdroot, "Unable to change current directory to root /");
#endif

#ifdef PSTR_CONNECT
str(noconnect, "Unable to connect");
#endif

#ifdef PSTR_CONSOLE_WRITING
str(noconsolewriting, "Could not open console for writing");
#endif

#ifdef PSTR_ALLOC_MEM
str(nomem, "Not enough memory\n");
#endif

#ifdef PSTR_GETHOSTBYNAME
str(nohost, "Unable to get informations about host name");
#endif

#ifdef PSTR_FORK
str(noprocess, "Unable to create process");
#endif

#ifdef PSTR_SOCKET
str(nosocket, "Unable to create socket");
#endif

#ifdef PSTR_SOCKET_ACCEPT
str(nosocketaccept, "Unable to accept socket");
#endif

#ifdef PSTR_SOCKET_BIND
str(nosocketbind, "Unable to bind socket");
#endif

#undef	str
