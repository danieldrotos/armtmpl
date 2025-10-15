#ifndef MOS_H_
#define MOS_H_


#define DELIM " \t\v\n():=,;"


typedef void task_init_fn(char *param);
typedef void task_loop_fn();
typedef void task_exit_fn();


struct mos_task_t {
  char *name;               // command name
  int fg;                   // true if run in foreground
  char *param;              // param passed from start
  int run;                  // true if runs
  task_init_fn *init;       // called at start
  task_loop_fn *loop;       // called periodically
  task_exit_fn *exit;       // called on exit
  struct mos_task_t *next;
};

// Task structure allocation macros
#define MOS_TASK(N,NAME,LOOP) struct mos_task_t N= { .name=NAME,.loop= LOOP }
#define MOS_TASKI(N,NAME,LOOP,INIT) struct mos_task_t N= { .name=NAME,.loop= LOOP,.init=INIT }
#define MOS_TASKE(N,NAME,LOOP,EXIT) struct mos_task_t N= { .name=NAME,.loop= LOOP,.exit=EXIT }
#define MOS_TASKIE(N,NAME,LOOP,INIT,EXIT) struct mos_task_t N= { .name=NAME,.loop= LOOP,.init=INIT,.exit=EXIT }


// API

// task management
extern void mos_register_task(struct mos_task_t *t);
extern struct mos_task_t *mos_create_task(char *name, int fg, task_loop_fn *loop);
extern int mos_start_task(char *name, char *param);
extern void mos_stop_task(char *name);

// functions for tasks
extern char *mos_get_param();
extern void mos_exit();

// application functions
extern void mos_init();
extern void mos();


#endif /* MOS_H_ */
