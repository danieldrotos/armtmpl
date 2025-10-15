/* wrap_iocl.h */

#ifndef WRAP_IOCTL_HEADER
#define WRAP_IOCTL_HEADER


#if defined TARGET_HASOS
#include <sys/ioctl.h>
#else
#include "system_ioctl.h"
#endif


#endif
