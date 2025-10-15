#include <stdio.h>

#include "ols_driver.h"


/* NULL terminated array */
const struct ols_driver_entry *ols_drivers[OLS_NUOF_DRIVERS]= { NULL };


int
ols_register_driver(const struct ols_driver_entry *drv)
{
  int i;

  if (drv == NULL)
    return -1;
  for (i= 0; (i<OLS_NUOF_DRIVERS-1) && (ols_drivers[i] != NULL); i++)
    ;//printf("drvreg: drv[%d] entry=%p (name=%s)\n", i, ols_drivers[i], ols_drivers[i]->name);
  if (i<OLS_NUOF_DRIVERS-1)
    {
      ols_drivers[i]= drv;
      ols_drivers[i+1]= NULL;
      return 0;
    }
  else
    return -2;
}
