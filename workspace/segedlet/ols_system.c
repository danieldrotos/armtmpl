#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

//#include "ff.h"

#include "ols_fs.h"


int
_fstat(int file, struct stat *st)
{
  struct ols_file_entry *fe;
  //FIL *f;
  
  if (file >= OLS_NUOF_FILES)
    return (errno= 1), -1;
  fe= &ols_files[file];
  if (!fe->opened)
    return (errno= 2), -1;
  if (!st)
    return (errno= 3), -1;
  //f= fe->fil;
  
  st->st_dev= -1;//f->id;
  st->st_ino= 0;
  st->st_mode= S_IFCHR;
  st->st_nlink= 1;
  st->st_uid= 0;
  st->st_gid= 0;
  st->st_rdev= 0;
  st->st_size= 512;//f->fsize;
  st->st_blksize= 512;//f->fs->ssize;
  st->st_blocks= 512;//1 + (f->fsize / 512/*f->fs->ssize*/); // hint!
  /* st_atim */
  /* st_mtim */
  /* st_ctim */
  return 0;
}

int
_stat(char *file, struct stat *st)
{
  //FILINFO fi;
  
  if (!file ||
      !*file)
    return (errno= 1), -1;
  if (!st)
    return (errno= 3), -1;
  /*
  if (f_stat(file, &fi) != FR_OK)
    return (errno= 4), -1;
  */
  st->st_dev= 0;//f->id;
  st->st_ino= 0;
  st->st_mode= S_IFCHR;
  st->st_nlink= 1;
  st->st_uid= 0;
  st->st_gid= 0;
  st->st_rdev= 0;
  st->st_size= 512;//fi.fsize;
  st->st_blksize= 512;//f->fs->ssize;
  st->st_blocks= 512;//1 + (fi.fsize / 512/*f->fs->ssize*/); // hint!
  /* st_atim */
  /* st_mtim */
  /* st_ctim */
  return 0;
}
