#ifndef PLANET_H_
#define PLANET_H_


struct planet_t
{
  float g;
  float v0;
  float h;
  int col;
  int row;
  int last_row;
  float t;
  unsigned int t0;
  char *title;
};


extern float hmax;
extern int max_row;
extern struct planet_t planets[];


extern void show(struct planet_t *p);


#endif /* PLANET_H_ */
