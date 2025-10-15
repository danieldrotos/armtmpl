/*
 * hd44780.h
 *
 *  Created on: 2020. okt. 20.
 *      Author: drdani
 */

#ifndef HD44780_H_
#define HD44780_H_

#define RS "PB13"
#define RW "PB14"
#define E  "PB15"

#define D0 "PB1"
#define D1 "PB2"
#define D2 "PB12"
#define D3 "PA11"
#define D4 "PA12"
#define D5 "PC5"
#define D6 "PC6"
#define D7 "PC8"

extern void hd44780_init();
extern void hd44780_printf(char *fmt, ...);

extern void hd44780_info(int fd);
extern int hd44780_cmd(int fd, char *cmd);

extern int ols_drv_hd44780_install();

#endif /* HD44780_H_ */
