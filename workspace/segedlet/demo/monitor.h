#ifndef MONITOR_HEADER
#define MONITOR_HEADER

enum { CMD_LEN=128 };

#define DELIMS " \t\v\r\n=,:"

extern int monitor(int ifd, int ofd);

#endif

/* End of monitor.h */
