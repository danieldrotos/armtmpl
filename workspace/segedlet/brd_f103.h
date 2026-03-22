/*
 * brd_f103.h
 *
 *  Created on: Mar 18, 2026
 *      Author: drdani
 */

#ifndef BRD_F103_H_
#define BRD_F103_H_


enum {
	LED0= 0,
	BTN0= 1
};

#define TDR		DR
#define RDR		DR

#define BRD_STDIO "/dev/usart1"
#define BRD_USART 1
#define BRD_TX_PIN "PA9"
#define BRD_RX_PIN "PA10"


#endif /* BRD_F103_H_ */
