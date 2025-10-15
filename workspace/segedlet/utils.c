#include <time.h>
#include <sys/time.h>
#include <stdint.h>

uint msnow()
{
  struct timeval tv;
  uint64_t s, u;
  gettimeofday(&tv, NULL);
  s= tv.tv_sec;
  u= tv.tv_usec;
  s*= 1000000;
  s+= u;
  s/= 1000;
  return s;
}

double dnow(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double)tv.tv_sec + ((double)tv.tv_usec/1000000.0);
}
