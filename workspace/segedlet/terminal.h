#ifndef TERMINAL_HEADER
#define TERMINAL_HEADER

#include <stdio.h>


enum {
	tu_buf_size= 200
};

enum {
	TU_UP		= -1,
	TU_DOWN		= -2,
	TU_LEFT		= -3,
	TU_RIGHT	= -4,
	TU_HOME		= -5,
	TU_END		= -6,
	TU_PGUP		= -7,
	TU_PGDOWN	= -8,
	TU_DEL		= -9,
	TU_F1		= -10,
	TU_F2		= -11,
	TU_F3		= -12,
	TU_F4		= -13,
	TU_F5		= -14,
	TU_F6		= -15,
	TU_F7		= -16,
	TU_F8		= -17,
	TU_F9		= -18,
	TU_F10		= -19,
	TU_F11		= -20,
	TU_F12		= -21,
	TU_INS		= -22,

	// mouse reports in 4 bytes: FF X Y Code
	TU_BTN1		= -50, // button1
	TU_BTN2		= -51, // button2
	TU_BTN3		= -52, // button3
	TU_CBTN1	= -53, // CTRL-button1
	TU_CBTN2	= -54, // CTRL-button2
	TU_CBTN3	= -55, // CTRL-button3
	TU_ABTN1	= -56, // ALT-button1
	TU_ABTN2	= -57, // ALT-button2
	TU_ABTN3	= -58, // ALT-button3
	TU_SUP		= -59, // Scroll-UP
	TU_SDOWN	= -60, // Scroll-DOWN
	TU_CSUP		= -61, // CTRL-Scroll-UP
	TU_CSDOWN	= -62, // CTRL-Scroll-DOWN
};

struct tu_input_t {
	int ifd;
	int ofd;
	int raw_mode;
	int process_esc;
	int buffer[tu_buf_size];
	int first_free;
	int last_used;
	char esc_buffer[100];
	char line[tu_buf_size];
	int cursor;
	int ready;
};

enum tu_color_t {
  RGB_NONE    = -1,
  RGB_BLACK   = 0,
  RGB_RED     = 1,
  RGB_GREEN   = 2,
  RGB_YELLOW  = 3,
  RGB_BLUE    = 4,
  RGB_MAGENTA = 5,
  RGB_CYAN    = 6,
  RGB_WHITE   = 7,
  RGB_BBLACK  = 0x10,
  RGB_BRED    = 0x11,
  RGB_BGREEN  = 0x12,
  RGB_BYELLOW = 0x13,
  RGB_BBLUE   = 0x14,
  RGB_BMAGENTA= 0x15,
  RGB_BCYAN   = 0x16,
  RGB_BWHITE  = 0x17,
};

extern struct tu_input_t ti_std;

// Select input mode
extern void ti_cooked(struct tu_input_t *ti);
extern void tu_cooked();
extern void ti_raw(struct tu_input_t *ti);
extern void tu_raw();
extern void ti_process_esc(struct tu_input_t *ti, int proc);
extern void tu_process_esc(int proc);

// INPUT
// Get char in RAW mode
extern int ti_getch(struct tu_input_t *ti);
extern int tu_getch();
extern int ti_getc(struct tu_input_t *ti);
extern int tu_getc();
// Get string in COOKED mode
extern int ti_gets(struct tu_input_t *ti, char *buf, int len);
extern int tu_gets(char *buf, int len);

// OUTPUT
// Moving cursor
extern void tu_save_cursor();
extern void dtu_save_cursor(int fd);
extern void ftu_save_cursor(FILE *f);

extern void tu_restore_cursor();
extern void dtu_restore_cursor(int fd);
extern void ftu_restore_cursor(FILE *f);

extern void tu_go_left(int n);
extern void dtu_go_left(int fd, int n);
extern void ftu_go_left(FILE *f, int n);

extern void tu_go_right(int n);
extern void dtu_go_right(int fd, int n);
extern void ftu_go_right(FILE *f, int n);

extern void tu_go_up(int n);
extern void dtu_go_up(int fd, int n);
extern void ftu_go_up(FILE *f, int n);

extern void tu_go_down(int n);
extern void dtu_go_down(int fd, int n);
extern void ftu_go_down(FILE *f, int n);

extern void tu_go(int x1, int y1);
extern void dtu_go(int fd, int x1, int y1);
extern void ftu_go(FILE *f, int x1, int y1);

// Clearing something
extern void tu_clear_screen();
extern void dtu_clear_screen(int fd);
extern void ftu_clear_screen(FILE *f);

extern void tu_clear_char();
extern void dtu_clear_char(int fd);
extern void ftu_clear_char(FILE *f);

#define NORMAL_COLOR "\033[39;49m"

extern void tu_color(enum tu_color_t bg, enum tu_color_t fg);
extern void dtu_color(int fd, enum tu_color_t bg, enum tu_color_t fg);
extern void ftu_color(FILE *f, enum tu_color_t bg, enum tu_color_t fg);

#define tu_bgfg(BG,FG) tu_color(BG,FG)
#define tu_fgbg(FG,BG) tu_color(BG,FG)
#define TU_BW tu_bgfg(RGB_BLACK, RGB_WHITE)

#define tu_fg(X) tu_color(RGB_NONE,X)
#define tu_bg(X) tu_color(X,RGB_NONE)
#define dtu_fg(D,X) dtu_color(D,RGB_NONE,X)
#define dtu_bg(D,X) dtu_color(D,X,RGB_NONE)
#define ftu_fg(F,X) dtu_color(F,RGB_NONE,X)
#define ftu_bg(F,X) dtu_color(F,X,RGB_NONE)

extern void tu_hide();
extern void dtu_hide(int fd);
extern void ftu_hide(FILE *f);

extern void tu_show();
extern void dtu_show(int fd);
extern void ftu_show(FILE *f);

extern void tu_mouse_on();
extern void dtu_mouse_on(int fd);
extern void ftu_mouse_on(FILE *f);

extern void tu_mouse_off(void);
extern void dtu_mouse_off(int fd);
extern void ftu_mouse_off(FILE *f);

#endif

/* End of terminal.h */
