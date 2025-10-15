#ifndef TERMINAL_HEADER
#define TERMINAL_HEADER

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


// processing style in RAW mode
extern void tu_process_esc(int val);

// Select input mode
extern void tu_cooked();
extern void tu_raw();

// INPUT
// Get char in RAW mode
extern int tu_getch();
extern int tu_getc();
// Get string in COOKED mode
extern int tu_gets(char *buf, int len);

// OUTPUT
// Moving cursor
extern void tu_save_cursor();
extern void tu_restore_cursor();
extern void tu_go_left(int n);
extern void tu_go_right(int n);
extern void tu_go_up(int n);
extern void tu_go_down(int n);
extern void tu_go(int x1, int y1);
// Clearing something
extern void tu_clear_screen();
extern void tu_clear_char();
extern void tu_color(int bg, int fg);
#define NORMAL_COLOR "\033[39;49m"
extern void tu_hide(void);
extern void tu_show(void);
extern void tu_mouse_on(void);
extern void tu_mouse_off(void);

#endif

/* End of terminal.h */
