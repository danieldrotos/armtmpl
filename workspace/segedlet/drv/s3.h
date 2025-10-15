#ifndef S3_HEADER
#define S3_HEADER

/* Signals */

#define S_RST "PD15"
#define S_WR  "PD14"
#define S_RD  "PD12"
#define S_D0  "PB11"
#define S_D1  "PB12"
#define S_D2  "PB13"
#define S_D3  "PB14"
#define S_D4  "PB15"
#define S_D5  "PD8"
#define S_D6  "PD9"
#define S_D7  "PD10"
#define S_A0  "PE8"
#define S_A1  "PE7"
#define S_A2  "PE10"
#define S_A3  "PE9"
#define S_A4  "PE12"
#define S_A5  "PE11"
#define S_A6  "PE14"
#define S_A7  "PE13"

/* Registers */
enum s3_regs {
  /* Outputs*/
  R_NRDSP0        = 0,       /* Numerical display, digit 0 */
  R_NRDSP1        = 1,       /* Numerical display, digit 1 */
  R_NRDSP2        = 2,       /* Numerical display, digit 2 */
  R_NRDSP3        = 3,       /* Numerical display, digit 3 */
  R_LED           = 4,       /* LEDs */
  R_DSP0          = 8,       /* 7seg display, digit 0 */
  R_DSP1          = 9,       /* 7seg display, digit 1 */
  R_DSP2          = 10,      /* 7seg display, digit 2 */
  R_DSP3          = 11,      /* 7seg display, digit 3 */
/* Inputs */
  R_BTN           = 5,       /* Buttons */
  R_SW            = 6        /* Switches */
};


extern void s3_init(void);
extern int s3_read(int address);
extern int s3_write(int address, int value);


#endif

/* End of s3.h */
