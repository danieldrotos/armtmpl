#ifndef OLS_FS_HEADER
#define OLS_FS_HEADER

#include <sys/param.h>

//#include "ff.h"

#include "ols_driver.h"

#if NOFILE > 64
#  define OLS_NUOF_FILES 64
#else
#  define OLS_NUOF_FILES NOFILE
#endif


struct ols_file_entry {
  int opened;
  int flags; /* parameter of open */
  struct ols_driver_entry *driver;
  int device;
  //FIL *fil;
};

extern struct ols_file_entry ols_files[OLS_NUOF_FILES];
extern int filesys_mounted;


extern int ols_stdio_init(char *fin, char *fout, char *ferror);

//extern int ols_mount(void);
//extern int ols_umount(void);

extern int ols_open(const char *fname, int fo_flags);
extern int ols_write(int fdes, char *buf, int len);
extern int ols_read(int fdes, char *buf, int len);
extern int ols_close(int fdes);
extern int ols_seek(int fdes, int offset, int whence);

extern int ols_input_avail(int fdes);
extern int ols_writable(int fdes);
extern int ols_ioctl(int fdes, unsigned long int request, va_list ap);
extern int ols_isatty(int fdes);
extern char *ols_ttyname(int fdes);
extern int ols_ttyname_r(int fdes, char *buf, int len);

//extern int freadable(int fd, double *timeout);
//extern int fwritable(int fd, double *timeout);

#endif
