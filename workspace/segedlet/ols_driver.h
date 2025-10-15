#ifndef OLS_DRIVER_HEADER
#define OLS_DRIVER_HEADER

#include "stdarg.h"


#define OLS_NUOF_DRIVERS 50

struct ols_driver_entry {
  const char *name;
  int is_tty;
  int (*open_fn)(int device, int fid, int fo_flags);
  int (*read_fn)(int device, int fid, void *buf, int nr);
  int (*write_fn)(int device, int fid, void *buf, int nr);
  int (*ioctl_fn)(int device, int fid, unsigned long int request, va_list ap);
  int (*close_fn)(int device, int fid);
  int (*seek_fn)(int device, int fid, int offset, int whence);
  int (*input_avail_fn)(int device, int fid);
  int (*writable_fn)(int device, int fid);
};


#endif
