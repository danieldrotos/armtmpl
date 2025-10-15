/* wrap_termios.h */

#ifndef WRAP_TERMIOS_HEADER
#define WRAP_TERMIOS_HEADER


#if defined TARGET_HASOS
#include <termios.h>
#include <sys/ttydefaults.h>
#else
#include "system_termios.h"
#endif


#endif
