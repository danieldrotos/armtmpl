#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include "wrap_termios.h"

//#include "utils.h"

//#include "ff.h"
//#include "ff_utils.h"

#include "ols.h"
#include "ols_driver.h"

#include "ols_fs.h"


/* BSS initialized to zero */
struct ols_file_entry ols_files[OLS_NUOF_FILES];

//static FATFS file_system;
//int filesys_mounted= 0;


int
ols_stdio_init(char *fin, char *fout, char *ferror)
{
  int fd;
  struct termios tp;

  if (fin && *fin)
    {
      if (ols_files[0].opened)
	{
	  if (stdin!=NULL)
	    fclose(stdin);
	  else
	    close(0);
	}
      fd= ols_open(fin, O_RDONLY);
      if (fd > 0)
	{
	  if (fd != 0)
	    {
	      memcpy(&(ols_files[0]), &(ols_files[fd]), sizeof(struct ols_file_entry));
	      ols_files[fd].opened= 0;
	      stdin= fdopen(0, "r");
	      tcgetattr(0, &tp);
	      tp.c_lflag|= ECHO;
	      cfsetspeed(&tp, 115200);
	      tcsetattr(0, TCSANOW, &tp);
	    }
	}
    }

  if (fout && *fout)
    {
      if (ols_files[1].opened)
	{
	  if (stdout!=NULL)
	    fclose(stdout);
	  else
	    close(1);
	}
      fd= ols_open(fout, O_WRONLY);
      if (fd > 0)
	{
	  if (fd != 1)
	    {
	      memcpy(&(ols_files[1]), &(ols_files[fd]), sizeof(struct ols_file_entry));
	      ols_files[fd].opened= 0;
	      stdout= fdopen(1, "w");
	      tcgetattr(1, &tp);
	      cfsetspeed(&tp, 115200);
	      tcsetattr(1, TCSANOW, &tp);
	    }
	}
    }

  if (ferror && *ferror)
    {
      fd= ols_open(ferror, O_WRONLY);
      if (fd > 0)
	{
	  if (fd != 2)
	    {
	      memcpy(&(ols_files[2]), &(ols_files[fd]), sizeof(struct ols_file_entry));
	      ols_files[fd].opened= 0;
	      stderr= fdopen(2, "w");
	    }
	}
    }
  return 0;
}

/*
int
ols_mount(void)
{
  if (!filesys_mounted)
    {
      if (f_mount(&file_system, "", 1) == FR_OK)
	{
	  filesys_mounted= 1;
	  //debug("FS mounted\n");
	}
      else
	{
	  //debug("FS mount failed\n");
	  return -1;
	}
    }
  return 0;
}
*/
/*
int
ols_umount(void)
{
  f_mount(NULL, "", 1);
  filesys_mounted= 0;
  return 0;
}
*/

int
ols_open(const char *fname, int fo_flags)
{
  int fdes, i;

  //printf("ols_open(%s,0x%x)...\n",fname,fo_flags);

  for (fdes= 3; fdes < OLS_NUOF_FILES; fdes++)
    if (!ols_files[fdes].opened)
      break;
  if (fdes == OLS_NUOF_FILES)
    {
      //printf("ols_open: too many files open\n");
      return -1;
    }

  if (!fname ||
      !*fname)
    {
      //printf("ols_open: filename missing\n");
      return -2;
    }

  //printf("ols_open(%s): fdes=%d\n", fname, fdes);
  ols_files[fdes].driver= NULL;
  ols_files[fdes].device= 0;
  ols_files[fdes].flags= fo_flags;
  ols_files[fdes].opened= 1;

  /* Map file name to a driver */
  //l= strlen(fname);
  for (i= 0; (i < OLS_NUOF_DRIVERS) && (ols_drivers[i]!=NULL); i++)
    {
      //printf("checking driver[%d] %s <-> %s\n", i, fname, ols_drivers[i]->name);
      if (strstr(fname, ols_drivers[i]->name) == fname)
	{
	  const char *s= fname + strlen(ols_drivers[i]->name);
	  //printf("driver %s match\n", ols_drivers[i]->name);
	  //printf("tail of fname= \"%s\"\n", s);
	  ols_files[fdes].driver= ols_drivers[i];
	  if (*s != '\0' &&
	      isdigit((int)(*s)))
	    {
	      char *tail;
	      int d;
	      if (*s)
		{
		  d= strtol(s, &tail, 10);
		  if (*tail)
		    {
		      //printf("ols_open: corrupt device nr in filename\n");
		      ols_files[fdes].opened= 0;
		      return -3;
		    }
		  //printf("ols_open: device= %d\n", d);
		  ols_files[fdes].device= d;
		}
	      break;
	    }	      
	}
    }
  
  /* driver selected, now open really */
  if (ols_files[fdes].driver)
    {
      /* pass open to driver */
      int ret;
      //printf("ols_open: driver=%s\n", ols_files[fdes].driver->name);
      if (ols_files[fdes].driver->open_fn != NULL)
	ret= ols_files[fdes].driver->open_fn(ols_files[fdes].device, fdes, fo_flags);
      else
	ret= 0;
      //printf("%s_open([fdes=%d]%s,0x%x)=%d\n",
      //ols_files[fdes].driver->name,
      //     fdes,fname,fo_flags,ret);
      if (ret!=0)
	{
	  ols_files[fdes].opened= 0;	  
	  return -1;
	}
    }
  else
    {
      /* use fatfs */
      /*
      int fa= 0, res;

      //printf("ols_open: fat\n");
      if (!filesys_mounted)
	{
	  return -1;
	}
      //printf("ols_open: no driver by name, use fatfs\n");
      if ((fo_flags & O_ACCMODE) == O_RDONLY)
	fa= FA_READ;
      if ((fo_flags & O_ACCMODE) == O_WRONLY)
	fa= FA_WRITE | FA_OPEN_ALWAYS;
      if ((fo_flags & O_ACCMODE) == O_RDWR)
	fa= FA_READ | FA_WRITE | FA_OPEN_ALWAYS;
      
      if (fo_flags & O_TRUNC)
	fa|= FA_CREATE_ALWAYS;
      if (fo_flags & O_CREAT)
	fa|= FA_OPEN_ALWAYS;

      ols_files[fdes].fil= (FIL*)dmalloc("ols:FIL", sizeof(FIL));
      //printf("f_open([fdes=%d]%s,0x%x)...\n",fdes,fname,fa);
      res= f_open(ols_files[fdes].fil, fname, fa);
      //printf("f_open([fdes=%d]%s,0x%x)=%d\n",fdes,fname,fa,res);
      if (res != FR_OK)
	{
	  // Failed to open, take it as closed
	  ols_files[fdes].opened= 0;	  
	  return (errno= (res<0)?-res:res),-1;
	}
      if (fo_flags & O_APPEND)
	f_lseek(ols_files[fdes].fil, f_size(ols_files[fdes].fil));
      */
    }

  return fdes;
}


int
ols_write(int fdes, char *buf, int len)
{
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return -1;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->write_fn != NULL)
	return ols_files[fdes].driver->write_fn(ols_files[fdes].device, fdes, buf, len);
      else
	return len;
    }
  else
    {
      return -1;
      /*
      unsigned int res= 0;
      int ret;
      ret= f_write(ols_files[fdes].fil, buf, len, &res);
      if ((ret == FR_OK) &&
	  (res >= 0))
	{
	  //printf("f_write([fdes=%d])=%d, res=%u\n", fdes, ret, res);
	  return res;
	}
      else
	{
	  //printf("f_write([fdes=%d])=%d res=%d %s\n", fdes, ret, res, f_error_str(ret));
	  return -ret;
	}
	*/
    }
}


int
ols_read(int fdes, char *buf, int len)
{
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return -1;
  if (!ols_files[fdes].opened)
    return -2;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->read_fn != NULL)
	return ols_files[fdes].driver->read_fn(ols_files[fdes].device, fdes, buf, len);
      else
	return 0;
    }
  else
    {
      return -1;
      /*
      unsigned int res= 0;
      int ret;
      ret= f_read(ols_files[fdes].fil, buf, len, &res);
      if (ret == FR_OK)
	{
	  //printf("f_read([fdes=%d])=%d, res=%u\n", fdes, ret, res);
	  return res;
	}
      else
	{
	  //printf("f_read([fdes=%d])=%d %s\n", fdes, ret, f_error_str(ret));
	  return -ret;
	}
	*/
    }
}


int
ols_close(int fdes)
{
  int res= 0;

  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return -1;
  if (!ols_files[fdes].opened)
    return -2;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->close_fn != NULL)
	res= ols_files[fdes].driver->close_fn(ols_files[fdes].device, fdes);
      else
	res= 0;
    }
  else
    {
      /*
      res= f_close(ols_files[fdes].fil);
      free(ols_files[fdes].fil);
      */
    }

  ols_files[fdes].opened= 0;
  //printf("fdes=%d now closed, res=%d\n",fdes,res);

  return res;
}


int
ols_seek(int fdes, int offset, int whence)
{
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return -1;
  if (!ols_files[fdes].opened)
    return -2;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->seek_fn != NULL)
	return ols_files[fdes].driver->seek_fn(ols_files[fdes].device, fdes, offset, whence);
      else
	return 0;
    }
  else
    {
      return -1;
      /*
      int ret= FR_OK;
      FIL *f= ols_files[fdes].fil;
      switch (whence)
	{
	case SEEK_SET:
	  // set to offset
	  ret= f_lseek(f, offset);
	  break;
	case SEEK_CUR:
	  // set to CUR+offset
	  ret= f_lseek(f, f_tell(f) + offset);
	  break;
	case SEEK_END:
	  // set to SIZE+offset
	  ret= f_lseek(f, f_size(f) + offset);
	  break;
	}
      if (ret == FR_OK)
	return f_tell(f);
      else
	return -1;
	*/
    }
}


int
ols_input_avail(int fdes)
{
  int ret;
  
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return 0;

  if (!ols_files[fdes].opened)
    return 0;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->input_avail_fn != NULL)
	ret= ols_files[fdes].driver->input_avail_fn(ols_files[fdes].device, fdes);
      else
	ret= 0;
    }
  else
    {
      /*
      FIL *f= ols_files[fdes].fil;
      ret= f_eof(f)?0:1;
      */
      ret= 1;
    }
  return ret;
}


int
ols_writable(int fdes)
{
  int ret;

  if (fdes < 0 ||
      fdes >=OLS_NUOF_FILES)
    return 0;

  if (!ols_files[fdes].opened)
    return 0;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->writable_fn != NULL)
	ret= ols_files[fdes].driver->writable_fn(ols_files[fdes].device, fdes);
      else
	ret= 1;
    }
  else
    {
      //FIL *f= ols_files[fdes].fil;
      ret= 1;
    }
  return ret;
}


int
ols_ioctl(int fdes, unsigned long int request, va_list ap)
{
  int ret;

  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return 0;

  if (!ols_files[fdes].opened)
    return 0;

  if (ols_files[fdes].driver)
    {
      if (ols_files[fdes].driver->ioctl_fn != NULL)
	{
	  ret= ols_files[fdes].driver->ioctl_fn(ols_files[fdes].device, fdes, request, ap);
	}
      else
	ret= 0;
    }
  else
    {
      ret= 0;
    }
  return (ret<=0)?ret:-ret;
}


int
ols_isatty(int fdes)
{
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return 0;

  if (!ols_files[fdes].opened)
    return 0;

  if (ols_files[fdes].driver)
    {
      return ols_files[fdes].driver->is_tty;
    }
  return 0;
}


static char ols_ttyname_buffer[100];

char *
ols_ttyname(int fdes)
{
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return 0;

  if (!ols_files[fdes].opened)
    return 0;

  if (ols_files[fdes].driver)
    {
      if (!ols_files[fdes].driver->is_tty)
	return NULL;
      snprintf(ols_ttyname_buffer, 99, "%s%d", ols_files[fdes].driver->name,
	       ols_files[fdes].device);
      return ols_ttyname_buffer;
    }
  return NULL;
}


int
ols_ttyname_r(int fdes, char *buf, int len)
{
  if (fdes < 0 ||
      fdes >= OLS_NUOF_FILES)
    return 1;

  if (!ols_files[fdes].opened)
    return 2;

  if (ols_files[fdes].driver)
    {
      if (!ols_files[fdes].driver->is_tty)
	return 3;
      if (buf == NULL)
	return 4;
      snprintf(buf, len-1, "%s%d", ols_files[fdes].driver->name,
	       ols_files[fdes].device);
      return 0;
    }
  return 5;
}

/*
int
freadable(int fd, double *timeout)
{
  int i= 0;
  double started;

  if (timeout)
	  start= dnow();
  do
  {
    i= ols_input_avail(fd)>0;
    if (timeout != NULL)
      {
    	if (dnow()-started > *timeout)
    		return i?1:0;
      }
  }
  while ((i==0) &&
	 (timeout != NULL));
  return i?1:0;
}
*/

/*
int
fwritable(int fd, double *timeout)
{
  int i= 0;
  double started= dnow();

  do {
    i= ols_writable(fd)>0;
    if (timeout!=NULL)
      {
	if ((dnow()-started > *timeout))
	  return i?1:0;
      }
  }
  while ((i==0) &&
	 (timeout != NULL));

  return i?1:0;
}
*/

