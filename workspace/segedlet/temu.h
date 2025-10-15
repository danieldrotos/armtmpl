/*
 * temu.h
 *
 *  Created on: 2020. okt. 23.
 *      Author: drdani
 */

#ifndef TEMU_H_
#define TEMU_H_

enum { esc_size= 20 };

enum sgr_flags {
  sgr_none			= 0,
  sgr_bright		= 0x001, // 1
  sgr_dim			= 0x002, // 2
  sgr_italic		= 0x004, // 3
  sgr_underline		= 0x008, // 4
  sgr_blink_slow	= 0x010, // 5
  sgr_blink_rapid	= 0x020, // 6
  sgr_reverse		= 0x040, // 7
  sgr_hidden		= 0x080, // 8
  sgr_crossed		= 0x100  // 9
};

struct temu_t {
	int curs_x;
	int curs_y;
	int cols;
	int rows;
	int saved_x;
	int saved_y;
	int esc_state;
	int esc_idx;
	char esc_seq[esc_size];
	int params[10];
	int valid[10];
	int nuof_fonts;
	int sgr_fg;
	int sgr_bg;
	int sgr_font;
	enum sgr_flags sgr_flags;
};

enum temu_ret_t {
	temu_clear_below_cursor		= -1,
	temu_clear_above_cursor		= -2,
	temu_clear_entire_screen	= -3,
	temu_clear_left				= -4,
	temu_clear_right			= -5,
	temu_clear_line				= -6,
	temu_set_sgr				= -7,
	temu_show_cursor			= -8,
	temu_hide_cursor			= -9
};

extern enum temu_ret_t temu_print(struct temu_t *e, char c);

#endif /* TEMU_H_ */
