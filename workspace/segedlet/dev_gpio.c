#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dev_gpio.h"


const struct gpio_device gpio_devices[GPIO_NUOF_DEVICES]= {
  // 0 A 
  {
    .gpio= GPIOA,
    //.rcc_ahb1= RCC_AHB1Periph_GPIOA
  },
  // 1 B 
  {
    .gpio= GPIOB,
  },
  // 2 C 
  {
    .gpio= GPIOC,
  },
  // 3 D 

  {
#ifdef GPIOD
		  .gpio= GPIOD,
#else
		  .gpio= NULL,
#endif
  },
   // 4 E
  {
#ifdef GPIOE
    .gpio= GPIOE,
#else
	.gpio= NULL,
#endif
  },
  // 5 F 
  {
    .gpio=
#ifdef GPIOF
    		GPIOF,
#else
			NULL,
#endif
  },
  // 6 G 
  {
    .gpio=
#ifdef GPIOG
    		GPIOG,
#else
			NULL,
#endif
  },
  // 7 H 
  {
    .gpio=
#ifdef GPIOH
    		GPIOH,
#else
			NULL,
#endif
  },
  // 8 I 
  {
    .gpio=
#ifdef GPIOI
    		GPIOI,
#else
			NULL
#endif
  }
};


/* Port initialization: start clock */
int
dev_gpio_init(int devnr)
{
  const struct gpio_device *dev;
  GPIO_TypeDef *gpio;

  if (devnr>=GPIO_NUOF_DEVICES ||
      devnr<0)
    return -1;
  dev= &gpio_devices[devnr];
  gpio= dev->gpio;
  if (gpio==NULL)
    return -2;

  /* Turn on gpio clock */
  //RCC_AHB1PeriphClockCmd(dev->rcc_ahb1, ENABLE);
  switch (devnr)
    {
#ifdef GPIOA
    case 0: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
#endif
#ifdef GPIOB
    case 1: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
#endif
#ifdef GPIOC
    case 2: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#endif
#ifdef GPIOD
    case 3: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#endif
#ifdef GPIOE
    case 4: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#endif
#ifdef GPIOF
    case 5: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif
#ifdef GPIOG
    case 6: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
#endif
#ifdef GPIOH
    case 7: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif
#ifdef GPIOI
    case 8: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
#endif
    }
  return 0;
}


/* Process init string: get out mode [0] */
static unsigned int //GPIOMode_TypeDef
dev_gpio_mode_of(char *init)
{
  if (init == NULL ||
      !*init)
    return GPIO_MODE_INPUT; //GPIO_Mode_IN;
  if (strlen(init) < 4)
    return GPIO_MODE_INPUT; //GPIO_Mode_IN;

  switch (toupper((int)init[0]))
    {
    case 'I':
      return GPIO_MODE_INPUT; //GPIO_Mode_IN;
    case 'O':
      return GPIO_MODE_OUTPUT_PP; //GPIO_Mode_OUT;
    case 'F':
      return GPIO_MODE_AF_PP; //GPIO_Mode_AF;
    case 'A':
      return GPIO_MODE_ANALOG; //GPIO_Mode_AN;
    default:
      return GPIO_MODE_INPUT; //GPIO_Mode_IN;
    }
}


/* Process init string: get out OType [1] */
/*static unsigned int //GPIOOType_TypeDef
f4_gpio_otype_of(char *init)
{
  if (init == NULL ||
      !*init)
    return GPIO_MODE_OUTPUT_OD; //GPIO_OType_OD;
  if (strlen(init) < 4)
    return GPIO_MODE_OUTPUT_OD; //GPIO_OType_OD;

  switch (toupper((int)init[1]))
    {
    case 'P':
      return GPIO_MODE_OUTPUT_PP; //GPIO_OType_PP;
    case 'O': case 'D':
      return GPIO_MODE_OUTPUT_OD; //GPIO_OType_OD;
    default:
      return GPIO_MODE_OUTPUT_OD; //GPIO_OType_OD;
    }
}
*/

/* Process init string: get out speed [2] */
static unsigned int //GPIOSpeed_TypeDef
dev_gpio_speed_of(char *init)
{
  if (init == NULL ||
      !*init)
    return GPIO_SPEED_FREQ_LOW; //GPIO_Speed_2MHz;
  if (strlen(init) < 4)
    return GPIO_SPEED_FREQ_LOW; //GPIO_Speed_2MHz;

  switch (toupper((int)init[2]))
    {
    case '2': case 'L':
      return GPIO_SPEED_FREQ_LOW; //GPIO_Speed_2MHz;
    case 'M':
      return GPIO_SPEED_FREQ_MEDIUM; //GPIO_Speed_25MHz;
    case 'F': case '5':
      return GPIO_SPEED_FREQ_HIGH; //GPIO_Speed_50MHz;
    case 'H': case '1':
      return GPIO_SPEED_FREQ_VERY_HIGH; //GPIO_Speed_100MHz;
    default:
      return GPIO_SPEED_FREQ_LOW; //GPIO_Speed_2MHz;
    }
}


/* Process init string: get out PuPd [3] */
static unsigned int //GPIOPuPd_TypeDef
dev_gpio_pupd_of(char *init)
{
  if (init == NULL ||
      !*init)
    return GPIO_NOPULL; //GPIO_PuPd_NOPULL;
  if (strlen(init) < 4)
    return GPIO_NOPULL; //GPIO_PuPd_NOPULL;

  switch (toupper((int)init[3]))
    {
    case 'N':
      return GPIO_NOPULL; //GPIO_PuPd_NOPULL;
    case 'U':
      return GPIO_PULLUP; //GPIO_PuPd_UP;
    case 'D':
      return GPIO_PULLDOWN; //GPIO_PuPd_DOWN;
    default:
      return GPIO_NOPULL; //GPIO_PuPd_NOPULL;
    }
}


/* Process pin name: get out port number */
int
dev_gpio_devnr_of(char *pin_name)
{
  char port;

  if (!pin_name)
    return -1;
  if (!*pin_name)
    return -2;

  if (toupper((int)(*pin_name)) == 'P')
    pin_name++;

  port= toupper((int)(*pin_name));
  if (port == '\0')
    return -3;

  if (port < 'A' ||
      port > 'A'+GPIO_NUOF_DEVICES)
    return -4;

  return port-'A';
}


/* Process pin name: get out bit mask */
int
dev_gpio_pinmask_of(char *pin_name)
{
  if (!pin_name)
    return -1;
  if (!*pin_name)
    return -2;

  if (toupper((int)(*pin_name)) == 'P')
    pin_name++;

  if (!*pin_name)
    return -3;
  
  pin_name++;
  if (!*pin_name)
    return -4;

  if (!isdigit((int)(*pin_name)))
    return -5;
  
  return 1<<(strtol(pin_name, NULL, 10));
}


/* Process pin name: get out pin nr (0..15) */
int
dev_gpio_pinnr_of(char *pin_name)
{
  if (!pin_name)
    return -1;
  if (!*pin_name)
    return -2;

  if (toupper((int)(*pin_name)) == 'P')
    pin_name++;

  if (!*pin_name)
    return -3;
  
  pin_name++;
  if (!*pin_name)
    return -4;

  if (!isdigit((int)(*pin_name)))
    return -5;
  
  return strtol(pin_name, NULL, 10);
}


/* Low level bit initialization */
static int
dev_gpio_pin_init(int devnr,
		  int pinmask, /* bit mask */
		  unsigned int /*GPIOMode_TypeDef*/ mode,
		  unsigned int /*GPIOSpeed_TypeDef*/ speed,
		  //unsigned int /*GPIOOType_TypeDef*/ otype,
		  unsigned int /*GPIOPuPd_TypeDef*/ pupd,
		  unsigned int af)
{
  GPIO_InitTypeDef g;
  const struct gpio_device *dev;
  GPIO_TypeDef *gpio;

  if (devnr>=GPIO_NUOF_DEVICES ||
      devnr<0)
    return -1;
  dev= &gpio_devices[devnr];
  gpio= dev->gpio;
  if (gpio==NULL)
    return -2;

  //printf("GPIO dev=%d pin=%d(0x%x)\n", devnr,pin,pin);
  if (dev_gpio_init(devnr) < 0)
    return -3;

  g./*GPIO_*/Pin  = pinmask;
  g./*GPIO_*/Mode = mode;
  //g.GPIO_OType= otype;
  g./*GPIO_*/Speed= speed;
  g./*GPIO_PuPd*/Pull = pupd;
  g.Alternate= af;
  HAL_GPIO_Init(gpio, &g);

  return 0;
}


/* High level bit initialization API */
// Mode   Otype   Speed           PuPd
// [IOFA] [P[OD]] [[L2]M[F5][H1]] [NUD]
int
dev_gpio_pin_init_of(char *pin_name, char *init)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_mask= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_mask < 0)
    return -1;

  if (dev_gpio_pin_init(dev_nr, pin_mask,
			/*mode*/dev_gpio_mode_of(init),
			/*speed*/dev_gpio_speed_of(init),
			///*otype*/f4_gpio_otype_of(init),
			/*pupd*/dev_gpio_pupd_of(init),
			0) < 0)
    return -2;

  return 0;
}

int
dev_gpio_input(char *pin_name)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_mask= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_mask < 0)
    return -1;

  if (dev_gpio_pin_init(dev_nr, pin_mask,
			/*mode*/GPIO_MODE_INPUT,//GPIO_Mode_IN,
			/*speed*/GPIO_SPEED_LOW,//GPIO_Speed_2MHz,
			///*otype*/GPIO_OType_PP,
			/*pupd*/GPIO_NOPULL, //GPIO_PuPd_UP
			0) < 0)
    return -2;

  return 0;
}

int
dev_gpio_input_ud(char *pin_name, int ud)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_mask= dev_gpio_pinmask_of(pin_name);

  if (dev_nr < 0 ||
      pin_mask < 0)
    return -1;

  if (dev_gpio_pin_init(dev_nr, pin_mask,
                        /*mode*/GPIO_MODE_INPUT,//GPIO_Mode_IN,
                        /*speed*/GPIO_SPEED_LOW,//GPIO_Speed_2MHz,
                        ///*otype*/GPIO_OType_PP,
                        ud, ///*pupd*/GPIO_NOPULL, //GPIO_PuPd_UP
                        0) < 0)
    return -2;

  return 0;
}


int
dev_gpio_output(char *pin_name)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  if (dev_gpio_pin_init(dev_nr, pin_nr,
			/*mode*/GPIO_MODE_OUTPUT_PP,//GPIO_Mode_OUT,
			/*speed*/GPIO_SPEED_LOW,//GPIO_Speed_25MHz,
			///*otype*/GPIO_OType_PP,
			/*pupd*/GPIO_NOPULL, //GPIO_PuPd_NOPULL
			0) < 0)
    return -2;

  return 0;
}


int
dev_gpio_soutput(char *pin_name, int init_value)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  HAL_GPIO_WritePin(gpio_devices[dev_nr].gpio,
		    pin_nr,
		    init_value?GPIO_PIN_SET:GPIO_PIN_RESET);

  if (dev_gpio_pin_init(dev_nr, pin_nr,
			/*mode*/GPIO_MODE_OUTPUT_PP,//GPIO_Mode_OUT,
			/*speed*/GPIO_SPEED_LOW,//GPIO_Speed_25MHz,
			///*otype*/GPIO_OType_PP,
			/*pupd*/GPIO_NOPULL, //GPIO_PuPd_NOPULL
			0) < 0)
    return -2;

  return 0;
}


int
dev_gpio_analog(char *pin_name)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  if (dev_gpio_pin_init(dev_nr, pin_nr,
			/*mode*/GPIO_MODE_ANALOG,//GPIO_Mode_AN,
			/*speed*/GPIO_SPEED_MEDIUM,//GPIO_Speed_25MHz,
			///*otype*/GPIO_OType_PP,
			/*pupd*/GPIO_NOPULL, //GPIO_PuPd_NOPULL
			0) < 0)
    return -2;

  return 0;
}


int
dev_gpio_af(char *pin_name, uint8_t af)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  //int idx= f4_gpio_pinnr_of(pin_name);

  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  dev_gpio_pin_init(dev_nr, pin_nr,
		    /*mode*/GPIO_MODE_AF_PP,//GPIO_Mode_AF,
		    /*speed*/GPIO_SPEED_MEDIUM,//GPIO_Speed_2MHz,
		    ///*otype*/GPIO_OType_PP,
		    /*pupd*/GPIO_NOPULL, //GPIO_PuPd_UP
		    af);

  
  //GPIO_PinAFConfig(gpio_devices[dev_nr].gpio, idx, af);
  return 0;
}

int
dev_gpio_init_af(char *pin_name, char *init, uint8_t af)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_mask= dev_gpio_pinmask_of(pin_name);
  //int idx= f4_gpio_pinnr_of(pin_name);

  if (dev_nr < 0 ||
      pin_mask < 0)
    return -1;

  dev_gpio_pin_init(dev_nr, pin_mask,
		    GPIO_MODE_AF_PP,//f4_gpio_mode_of(init),///*mode*/GPIO_MODE_AF_PP,//GPIO_Mode_AF,
		    dev_gpio_speed_of(init),///*speed*/GPIO_SPEED_MEDIUM,//GPIO_Speed_2MHz,
		    ///*otype*/GPIO_OType_PP,
		    dev_gpio_pupd_of(init),///*pupd*/GPIO_NOPULL, //GPIO_PuPd_UP
		    af);

  
  //GPIO_PinAFConfig(gpio_devices[dev_nr].gpio, idx, af);
  return 0;
}

/*
int
f4_gpio_exti(char *pin_name, int edge, int af)
{
  uint32_t port, pinnr;
  EXTI_InitTypeDef exti;
  NVIC_InitTypeDef nvic;

  port= f4_gpio_devnr_of(pin_name);
  pinnr= f4_gpio_pinnr_of(pin_name);

  f4_gpio_input(pin_name);
  if (af >= 0)
    f4_gpio_af(pin_name, af);
  //GPIO_PinAFConfig(gpio_devices[port].gpio, pinnr, af); 

  // Enable SYSCFG clock 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  // Connect EXTI LineX to port pin 
  SYSCFG_EXTILineConfig(port, //EXTI_PortSourceGPIOA 0,1...
			pinnr //EXTI_PinSource0 0,1..15
			);

  // Configure EXTI Line 
  exti.EXTI_Line = 1<<pinnr;//EXTI_Line0
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  if (edge < 0)
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
  else if (edge == 0)
    exti.EXTI_Trigger= EXTI_Trigger_Rising_Falling;
  else if (edge > 0)
    exti.EXTI_Trigger= EXTI_Trigger_Rising;
  exti.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti);

  // Enable and set EXTI Interrupt to the highest priority 
  if (pinnr == 0)
    nvic.NVIC_IRQChannel = EXTI0_IRQn;
  else if (pinnr == 1)
    nvic.NVIC_IRQChannel = EXTI1_IRQn;
  else if (pinnr == 2)
    nvic.NVIC_IRQChannel = EXTI2_IRQn;
  else if (pinnr == 3)
    nvic.NVIC_IRQChannel = EXTI3_IRQn;
  else if (pinnr == 4)
    nvic.NVIC_IRQChannel = EXTI4_IRQn;
  else if ((pinnr >= 5) && (pinnr <= 9))
    nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
  else if ((pinnr >= 10) && (pinnr <= 15))
    nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority = pinnr;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);

  return pinnr;
}
*/

int
dev_gpio_set(char *pin_name, int value)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  HAL_GPIO_WritePin(gpio_devices[dev_nr].gpio, pin_nr, value?GPIO_PIN_SET:GPIO_PIN_RESET);
  return value;
}

int
dev_gpio_inv(char *pin_name)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  int value;
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  value= (HAL_GPIO_ReadPin(gpio_devices[dev_nr].gpio, pin_nr)==GPIO_PIN_SET)?1:0;
  HAL_GPIO_WritePin(gpio_devices[dev_nr].gpio, pin_nr, value?GPIO_PIN_RESET:GPIO_PIN_SET);
  return value;
}


int
dev_gpio_get(char *pin_name)
{
  int f4_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  
  if (f4_nr < 0 ||
      pin_nr < 0)
    return -1;

  return
    (HAL_GPIO_ReadPin(gpio_devices[f4_nr].gpio, pin_nr)==GPIO_PIN_SET)?
    1:0;
}

int
dev_gpio_oget(char *pin_name)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return -1;

  return
    (gpio_devices[dev_nr].gpio->ODR & pin_nr)?1:0;
}


static const char *pin_modes[]= {
  "Input", // I
  "Output", // O
  "Function", // F
  "Analog" // A
};

static const char *pin_otype[]= {
  "Push-pull", // P
  "Open-drain" // OD
};

static const char *pin_speed[]= {
  "Low 2 MHz", // L2
  "Med 25 MHz", // M
  "Full 50 MHz", // F5
  "High 100 MHz" // H1
};

static const char *pin_pupd[]= {
  "NoPuPd", // N
  "Up-pull",  // U
  "Down-pull",  // D
  "Reserved"
};

uint32_t *
dev_gpio_band_output(char *pin_name)
{
  int dev= dev_gpio_devnr_of(pin_name);
  int pnr= dev_gpio_pinnr_of(pin_name);
  uint32_t offset, alias;
  const struct gpio_device *d;
  
  if ((dev < 0) ||
      (dev >= GPIO_NUOF_DEVICES))
    return 0;
  d= &(gpio_devices[dev]);
  offset= (uint32_t)(&(d->gpio->ODR)) - 0x40000000;
  alias= 0x42000000 + (offset * 32) + (pnr * 4);
  return (uint32_t*)alias;
}

uint16_t *
dev_gpio_odr(char *pin_name)
{
  int dev= dev_gpio_devnr_of(pin_name);
  const struct gpio_device *d;
  
  if ((dev < 0) ||
      (dev >= GPIO_NUOF_DEVICES))
    return 0;
  d= &(gpio_devices[dev]);
  return (uint16_t*)(&(d->gpio->ODR));
}

GPIO_TypeDef *
dev_gpio_of(char *pin_name)
{
  int dev= dev_gpio_devnr_of(pin_name);
  const struct gpio_device *d;
  
  if ((dev < 0) ||
      (dev >= GPIO_NUOF_DEVICES))
    return 0;
  d= &(gpio_devices[dev]);
  return d->gpio;
}

static char port_name[3];

char *
dev_gpio_name(GPIO_TypeDef *port)
{
	int i;
	char c= 'A';
	for (i=0;i<GPIO_NUOF_DEVICES;i++,c++)
	{
		if (gpio_devices[i].gpio == port)
		{
			port_name[0]= 'P';
			port_name[1]= c;
			port_name[2]= 0;
			return port_name;
		}
	}
	return "";
}
uint32_t *
dev_gpio_band_input(char *pin_name)
{
  int dev= dev_gpio_devnr_of(pin_name);
  int pnr= dev_gpio_pinnr_of(pin_name);
  uint32_t offset, alias;
  const struct gpio_device *d;
  
  if ((dev < 0) ||
      (dev >= GPIO_NUOF_DEVICES))
    return 0;
  d= &(gpio_devices[dev]);
  offset= (uint32_t)(&(d->gpio->IDR)) - 0x40000000;
  alias= 0x42000000 + (offset * 32) + (pnr * 4);
  return (uint32_t*)alias;
}

// Mode   Otype   Speed           PuPd
// [IOFA] [P[OD]] [[L2]M[F5][H1]] [NUD]

void
dev_print_gpio(int fd, char *pin_name)
{
  int dev_nr= dev_gpio_devnr_of(pin_name);
  int pin_nr= dev_gpio_pinmask_of(pin_name);
  int i, idx= dev_gpio_pinnr_of(pin_name);
  uint32_t *p;
  char init[5];
  
  if (dev_nr < 0 ||
      pin_nr < 0)
    return;
  
  p= (uint32_t*)(gpio_devices[dev_nr].gpio);
  for (i=0;i<10;i++)
    dprintf(fd, "P%c.%d dev[%d].%x reg[%d,%02x]= 0x%lx\n",
	    dev_nr+'A', idx, dev_nr, pin_nr,
	    i, i*4, p[i]);
  

  if (idx<8)
    i= (p[8] >> (idx*4)) & 0xf;
  else
    i= (p[9] >> ((idx-8)*4)) & 0xf;
  dprintf(fd, "AF    = %d 0x%x\n", i, i);

  i= (p[0] >> (idx*2)) & 3;
  dprintf(fd, "Mode  = %d %s\n", i, pin_modes[i]);
  init[0]= pin_modes[i][0];
       
  i= p[1] & pin_nr;
  dprintf(fd, "Otype = %x %s\n", i, pin_otype[i?1:0]);
  init[1]= pin_otype[i?1:0][0];

  i= (p[2] >> (idx*2)) & 3;
  dprintf(fd, "Speed = %d %s\n", i, pin_speed[i]);
  init[2]= pin_speed[i][0];

  i= (p[3] >> (idx*2)) & 3;
  dprintf(fd, "PuPd  = %d %s\n", i, pin_pupd[i]);
  init[3]= pin_pupd[i][0];

  init[4]= '\0';
  dprintf(fd, "init = %s\n", init);

  i= p[4] & pin_nr;
  dprintf(fd, "Input = %d\n", i?1:0);

  i= p[5] & pin_nr;
  dprintf(fd, "Output= %d\n", i?1:0);
}
