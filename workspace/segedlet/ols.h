#ifndef OLS_HEADER
#define OLS_HEADER

#include "ols_driver.h"


extern struct ols_driver_entry *ols_drivers[OLS_NUOF_DRIVERS];

extern int ols_register_driver(const struct ols_driver_entry *drv);


#endif
