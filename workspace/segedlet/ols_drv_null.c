#include <stdio.h>

#include "ols_driver.h"
#include "ols.h"

#include "ols_drv_null.h"


int
dev_null_read(int device, int fid, void *buf, int nr)
{
  return 0;
}

int
dev_null_write(int device, int fid, void *buf, int nr)
{
  return nr;
}

static const struct ols_driver_entry od_null= {
  .name= "/dev/null",
  .is_tty= 0,  
  .open_fn= NULL,
  .read_fn= dev_null_read,
  .write_fn= dev_null_write,
  .close_fn= NULL,
  .seek_fn= NULL
};


int
ols_drv_null_install(void)
{
  return ols_register_driver(&od_null);
}
