#ifndef DEV_GPIO_HEADER
#define DEV_GPIO_HEADER


#include "dev.h"

#define GPIO_NUOF_DEVICES 9

struct gpio_device {
  GPIO_TypeDef *gpio;
  //uint32_t rcc_ahb1;
};

extern const struct gpio_device gpio_devices[GPIO_NUOF_DEVICES];


/* Process pin name */
extern int dev_gpio_devnr_of(char *pin_name);
extern int dev_gpio_pinmask_of(char *pin_name);
extern int dev_gpio_pinnr_of(char *pin_name);


/* Bit initialization */
// Mode   Otype   Speed           PuPd
// [IOFA] [P[OD]] [[L2]M[F5][H1]] [NUD]
extern int dev_gpio_pin_init_of(char *pin_name, char *init);
extern int dev_gpio_input(char *pin_name);
extern int dev_gpio_input_ud(char *pin_name, int ud);
extern int dev_gpio_output(char *pin_name);
extern int dev_gpio_soutput(char *pin_name, int init_value);
extern int dev_gpio_analog(char *pin_name);
extern int dev_gpio_af(char *pin_name, uint8_t af);
extern int dev_gpio_init_af(char *pin_name, char *init, uint8_t af);
extern int f4_gpio_exti(char *pin_name, int edge, int af);

#define f4_gpio_input dev_gpio_input
#define f4_gpio_output dev_gpio_output

/* Set/get bit */
extern int dev_gpio_set(char *pin_name, int value);
extern int dev_gpio_inv(char *pin_name);
extern int dev_gpio_get(char *pin_name);
extern int dev_gpio_oget(char *pin_name);

#define f4_gpio_set dev_gpio_set
#define f4_gpio_inv dev_gpio_inv
#define f4_gpio_get dev_gpio_get

/* Bit banding */
extern uint32_t *dev_gpio_band_output(char *pin_name);
extern uint32_t *dev_gpio_band_input(char *pin_name);

extern uint16_t *dev_gpio_odr(char *pin_name);
extern GPIO_TypeDef *dev_gpio_of(char *pin_name);
extern char *dev_gpio_name(GPIO_TypeDef *port);

/* Print info */
extern void dev_print_gpio(int fd, char *pin_name);


#endif
