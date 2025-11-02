#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "brd.h"
#include "terminal.h"

#include "mos.h"


static struct mos_task_t *mos_tasks= NULL;
static struct mos_task_t *mos_fg= NULL;
static struct mos_task_t *mos_act= NULL;

static int inited= 0;
static char mos_cmd[100]= { 0 };


static struct mos_task_t *
mos_find(char *name)
{
  struct mos_task_t *t= mos_tasks;
  if (!name || !*name)
    return NULL;
  while (t)
    {
      if (strcmp(t->name, name) == 0)
        return t;
      t= t->next;
    }
  return NULL;
}


static void
mos_cmd_ls()
{
  struct mos_task_t *t= mos_tasks;
  while (t)
    {
      printf("%p %c%c%c%c %s\n",
             t, (t->fg)?'F':'-',
             (t->init==NULL)?'-':'I',
             (t->loop==NULL)?'-':'L',
             (t->exit==NULL)?'-':'E',
             t->name
             );
      t= t->next;
    }
  mos_exit();
}


static void
mos_cmd_ps()
{
  struct mos_task_t *t= mos_tasks;
  while (t)
    {
      if (t->run)
        {
          printf("%p %s\n", t,
                 t->name);
        }
      t= t->next;
    }
  mos_exit();
}


static void
mos_cmd_kill()
{
  struct mos_task_t *t= mos_find(mos_act->param);
  if (t && (t != mos_act))
    {
      struct mos_task_t *save= mos_act;
      mos_act= t;
      mos_exit();
      mos_act= save;
    }
  mos_exit();
}


static void
mos_cmd_cls()
{
  tu_color(0, 7);
  tu_clear_screen();
  mos_exit();
}


static void
mos_cmd_date()
{
  time_t t;
  struct tm *tm;
  t= time(NULL);
  tm= gmtime(&t);
  printf("%4d.%02d.%02d. %02d:%02d:%02d\n",
         tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
         tm->tm_hour, tm->tm_min, tm->tm_sec);
  mos_exit();
}

static int reset_wait_answer= 0;

static void
mos_cmd_reset()
{
  if (!reset_wait_answer)
    {
      tu_color(0, RGB_BRED);
      printf("Realy (Y/n)?\n");
      reset_wait_answer= 1;
    }
  else
    {
      int c= tu_getc();
      if (c == 'Y')
        {
          tu_color(0, 7);
          tu_clear_screen();
          tu_show();
          {
            unsigned int now= HAL_GetTick();
            while (HAL_GetTick() - now < 100) ;
          }
          NVIC_SystemReset();
        }
      else if ((c == 'n') || (c == 'N'))
        {
          reset_wait_answer= 0;
          mos_exit();
        }
      else
        ;
    }
}


static void
mos_prompt()
{
  tu_color(0, RGB_BYELLOW);
  printf("mos> ");
  tu_color(0, RGB_BWHITE);
  tu_show();
}


void
mos_register_task(struct mos_task_t *t)
{
  if ((t == NULL) ||
      (t->name == NULL) ||
      (t->name[0] == 0))
    return;
  t->next= mos_tasks;
  mos_tasks= t;
}


struct mos_task_t *
mos_create_task(char *name, int fg, task_loop_fn *loop)
{
  struct mos_task_t *t;
  t= calloc(sizeof(struct mos_task_t), 1);
  t->name= name;
  t->fg= fg;
  t->loop= loop;
  mos_register_task(t);
  return t;
}


int
mos_start_task(char *name, char *param)
{
  struct mos_task_t *t= mos_find(name);
  if (t && (!t->fg || (mos_fg == NULL)))
    {
      if (t->run)
        {
          tu_color(0, 1);
          printf("%s is running.\n", name);
          return 0;
        }
      if (param && *param)
        t->param= strdup(param);
      if (t->init != NULL)
        t->init(param);
      t->run= 1;
      if (t->fg)
        {
          mos_fg= t;
          tu_color(0, 7);
        }
      return 1;
    }
  else
    {
      tu_color(0, 1);
      printf("Unknown command: %s\n", name);
      return 0;
    }
}


void
mos_stop_task(char *name)
{
  struct mos_task_t *t= mos_find(name);
  if (t /*&& (t != mos_act)*/)
    {
      struct mos_task_t *save= mos_act;
      mos_act= t;
      mos_exit();
      mos_act= save;
    }
}


char *
mos_get_param()
{
  if (mos_act != NULL)
    return mos_act->param;
  return NULL;
}


void
mos_exit()
{
  if (mos_act != NULL)
    {
      mos_act->run= 0;
      if (mos_act->exit != NULL)
        mos_act->exit();
      if (mos_act->param != NULL)
        free(mos_act->param);
      mos_act->param= NULL;
      if (mos_fg == mos_act)
        {
          mos_fg= NULL;
          mos_prompt();
        }
    }
}


static void
mos_process()
{
  char *s= strtok(mos_cmd, DELIM);
  printf("\n");
  if (s != NULL)
    {
      char *name= s;
      s= strtok(NULL, DELIM);
      if (!mos_start_task(name, s) || (mos_fg == NULL))
        mos_prompt();
    }
  else
    mos_prompt();
}


static void
mos_interpret()
{
  if (tu_gets(mos_cmd, 99))
    {
      mos_process();
      mos_cmd[0]= 0;
    }
}


void
mos_init()
{
  if (mos_find("ls") != NULL)
    return;
  mos_create_task("ls"   , 1, &mos_cmd_ls);
  mos_create_task("ps"   , 1, &mos_cmd_ps);
  mos_create_task("kill" , 1, &mos_cmd_kill);
  mos_create_task("cls"  , 1, &mos_cmd_cls);
  mos_create_task("reset", 1, &mos_cmd_reset);
  mos_create_task("date" , 1, &mos_cmd_date);
  tu_show();
  tu_color(0,7);
  //tu_clear_screen();
  printf("Starting \"mos\" mini operating system.\n");
  printf("Use ls command to list know commands.\n");
  printf("\n");
}


void
mos()
{
  struct mos_task_t *t;
  if (mos_fg == NULL)
    {
      if (!inited)
        {
          mos_prompt();
          inited= 1;
        }
      else
        mos_interpret();
    }
  t= mos_tasks;
  while (t)
    {
      if (t->run)
        {
          mos_act= t;
          t->loop();
          mos_act= NULL;
        }
      t= t->next;
    }
}
