#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xhw_tpm.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xwdt.h"
#include "xuart.h"
#include "xhw_ints.h"

#include "segedlet.h"

volatile unsigned int clk __attribute__ ((aligned (4)))= 0;

void SysTickIntHandler(void)
{
  clk++;
}

unsigned int HAL_GetTick()
{
	return clk;
}

#define UART_BASE UART0_BASE

void
board_init()
{
  WDTimerDisable();
  xSysCtlClockSet(CLOCK, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
  SysCtlDelay(1000000);

  xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOD);
  xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOB);

  //Green LED
  xGPIOSPinTypeGPIOOutput(PB19);
  //Blue LED
  //xGPIOSPinTypeGPIOOutput(PD1);
  //Red LED
  xGPIOSPinTypeGPIOOutput(PB18);

  xGPIOPinWrite(xGPIO_PORTB_BASE, xGPIO_PIN_18, 1);
  xGPIOPinWrite(xGPIO_PORTB_BASE, xGPIO_PIN_19, 1);
  xGPIOPinWrite(xGPIO_PORTD_BASE, xGPIO_PIN_1, 1);

  systick_init(0.001);
  uart_init();
  tsi_init();
  tsi_start();

}

void
systick_init(float time)
{
  xSysTickPeriodSet(time / (1.0/ /*(48000000.0/16.0)*/ (float)CLOCK));
  xSysTickIntEnable();
  xSysTickValueGet();
  xSysTickEnable();

}

void
green_set(int v)
{
  xGPIOPinWrite(xGPIO_PORTB_BASE, xGPIO_PIN_19, v?0:1);
}

void
red_set(int v)
{
  xGPIOPinWrite(xGPIO_PORTB_BASE, xGPIO_PIN_18, v?0:1);
}

void
blue_set(int v)
{
  xGPIOSPinTypeGPIOOutput(PD1);
  xGPIOPinWrite(xGPIO_PORTD_BASE, xGPIO_PIN_1, v?0:1);
}

void
green_on()
{
  green_set(1);
}

void
green_off()
{
  green_set(0);
}

void
green_toggle()
{
  xGPIOSPinTypeGPIOOutput(PD1);
  green_set(xGPIOPinRead(xGPIO_PORTB_BASE, xGPIO_PIN_19));
}

void
red_on()
{
  red_set(1);
}

void
red_off()
{
  red_set(0);
}

void
red_toggle()
{
  red_set(xGPIOPinRead(xGPIO_PORTB_BASE, xGPIO_PIN_18));
}

void
blue_on()
{
  blue_set(1);
}

void
blue_off()
{
  blue_set(0);
}

void
blue_toggle()
{
  blue_set(xGPIOPinRead(xGPIO_PORTD_BASE, xGPIO_PIN_1));
}

int dummy2;

int
blue_pwm(float fr, int duty)
{
  int d= 0;
  int divby= 1;
  int ifr= CLOCK/4;
  int tfr;
  int steps= 0;
  int compare_level;
  volatile int sc;

  SysCtlPeripheralClockSourceSet(SYSCTL_PERIPH_TPM_S_MCGPLLCLK_2);
 // xHWREG(0x40048004)= 0x05010000;
  xSysCtlPeripheralEnable(xSYSCTL_PERIPH_PWMA);
  xSPinTypePWM(TIM0CH1, PD1);
  // disable timer
  xHWREG(PWMA_BASE+TPM_SC)= TPM_SC_CMOD_DIS;
  while ((divby= (1<<d)) < 129)
    {
      float tstep;
      tfr= ifr/divby;
      tstep= 1/(float)tfr;
      steps= (1/fr)/tstep;
      if (steps < 0xffff)
        break;
      d++;
    }
  if (steps > 0xffff)
    return steps;
  compare_level= (steps * duty)/100;

  // select prescaler, and edge-aligned mode
  xHWREG(PWMA_BASE+TPM_SC)= d;
  // tfr: timer stepping freq, steps= nr of steps needed for fr output
  xHWREG(PWMA_BASE+TPM_MOD)= steps;

    // use channel 1, high-true pulses
  xHWREG(PWMA_BASE+TPM_C1SC)= TPM_CNSC_MSB | TPM_CNSC_ELSA;
  sc= xHWREG(PWMA_BASE+TPM_C1SC);

  // calc and set compare level
  xHWREG(PWMA_BASE+TPM_C1V)= compare_level;

  // start
  xHWREG(PWMA_BASE+TPM_SC)= xHWREG(PWMA_BASE+TPM_SC)|TPM_SC_CMOD_CLK;
  sc= xHWREG(PWMA_BASE+TPM_SC);
  dummy2= sc;
  return steps;
}

enum { ubs= 128 };
char ubuf[ubs];
volatile int ff= 0, lu= 0;

unsigned long
uart_callback(void *pvCBData,
    unsigned long ulEvent,
    unsigned long ulMsgParam,
    void *pvMsgData)
{
  int c;
  //red_toggle();
  c= /*UARTCharGetNonBlocking*/UART_get(UART_BASE);
  if (c >= 0)
    {
      int n= (ff+1)%ubs;
      if (n!=lu)
        {
          ubuf[ff]= c;
          ff= n;
        }
    }
  return 0;
}

int
light2duty(int light)
{
	return (light*light)/100;
}

void
blue_light(int percent)
{
	int duty;
	if (percent<0)
		percent= 0;
	if (percent>100)
		percent= 100;
	duty= light2duty(percent);
	blue_pwm(1000, duty);
}

int
ubuf_non_empty()
{
  int f, l;
  UARTIntDisable(UART_BASE, UART_INT_R);
  f= ff;
  l= lu;
  UARTIntEnable(UART_BASE, UART_INT_R);
  return f!=l;
}

char
ubuf_get()
{
  char c= 0;
  UARTIntDisable(UART_BASE, UART_INT_R);
  if (lu!=ff)
    {
      c= ubuf[lu];
      lu= (lu+1)%ubs;
    }
  UARTIntEnable(UART_BASE, UART_INT_R);
  return c;
}

void
uart_init()
{
  //
  // Enable GPIO and UART Clock
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

  //
  // Remap UART pin to GPIO Port UART0_RX --> PA2 UART0_TX --> PA1
  //
  xSPinTypeUART(UART0RX, PA1);
  xSPinTypeUART(UART0TX, PA2);
  // UARt2 RX=D2 TX=D3
  xSPinTypeUART(UART2RX, PD2);
  xSPinTypeUART(UART2TX, PD3);

  //
  // Set UART clock
  //
  SysCtlPeripheralClockSourceSet(SYSCTL_PERIPH_UART0_S_MCGPLLCLK_2);

  //
  // Disable UART Receive/Transmit
  //
  UARTDisable(UART_BASE, UART_TX | UART_RX);

  //
  // Configure UART Baud 115200
  //
  UARTConfigSet(UART_BASE, 19200, UART_CONFIG_SAMPLE_RATE_16 | UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_1);

  UARTIntCallbackInit(UART_BASE, uart_callback);
  UARTIntEnable(UART_BASE, UART_INT_R);
  xIntMasterEnable();
  xIntEnable(28);
  xIntEnable(29);
  xIntEnable(30);
  //
  // Enable UART Receive and Transmit
  //
  UARTEnable(UART_BASE, UART_TX | UART_RX);
}

int
uart_avail()
{
  return ubuf_non_empty();
  //return UARTCharsAvail(UART_BASE);
}

int
ols_input_avail(int fd)
{
	return uart_avail();
}

int uart_echo= 1;

int
uart_read(char *ptr, int len)
{
  int i;

  for(i = 0; i < len; i++)
    {
      if (!uart_avail())
        return i;
      *ptr++ = ubuf_get()/*UARTCharGet(UART_BASE)*/;
    }

  return len;
}

int
uart_write(char *ptr, int len)
{
  int i;

  for (i= 0; i<len; i++)
    UARTCharPut(UART_BASE, *ptr++);

  return len;
}


#define TSI0_GENCS (TSI_BASE+0)
#define TSI0_DATA (TSI_BASE+4)

#define REG(x) (*((volatile uint32_t *)(x)))
#define GENCS (REG(TSI0_GENCS))
#define DATA (REG(TSI0_DATA))

int tsi_value;
int tsi_base;
int tsi_raw;
int tsi_prev_on, tsi_prev_off;


void
TSIIntHandler(void)
{
  tsi_value= DATA & 0xffff;
  tsi_start();
}

void
tsi_init()
{
  uint32_t r;

  // ENABLE TPM1
  r= xHWREG(/*SCGC6*/0x4004803c);
  r|= 1<<25;
  xHWREG(0x4004803c)= r;

  // Enable TSI
  r= xHWREG(/*SCGC5*/0x40048038);
  r|= /*SIM_SCGC5_TSI_MASK*/1<<5;
  xHWREG(0x40048038)= r;

  r= xHWREG(/*PORTB_PCR16*/0x4004A040);
  r&= ~(0b111 << 8);
  r|= 0b000 << 8;
  xHWREG(0x4004A040)= r;
  r= xHWREG(/*PORTB_PCR17*/0x4004A044);
  r&= ~(0b111 << 8);
  r|= 0b000 << 8;
  xHWREG(0x4004A044)= r;

  r= 0;
  // ESOR: end-of-scan
  r|= 1 << 28;
  // MODE: capacitive sense
  r|= 0b0000 << 24;
  // REFCHREG: 8uA
  r|= 0b100 << 21;
  // DVOLT: 0
  r|= 0b00 << 19;
  // EXTCHRG: 64uA
  r|= 0b111 << 16;
  // PS: div by 1
  r|= 0b000 << 13;
  // NSNC: 30
  r|= 0b11101 << 8;
  // TSIEN: disable (yet)
  r|= 0 << 7;
  // TSIIEN: disable
  r|= 0 << 6;
  // STPE: disbale
  r|= 0 << 5;
  // STM: software trigger
  r|= 0 << 4;
  // CURSW: not swapped
  r|= 0 << 1;

  GENCS= r;

  r= 0;
  // TSICH: channel9
  r|= 0b1001 << 28;
  // DMAEN: disabled
  r|= 0 << 23;
  // SWTS: no effect
  r|= 0 << 22;
  // TSICNT: 0
  r|= 0 << 0;
  DATA= r;

  r= GENCS;
  r|= (1 << 7);
  GENCS= r;

  tsi_scan();
  tsi_base= tsi_raw;
  tsi_value= 0;
  tsi_prev_on= tsi_prev_off= tsi_touch();
}

void
tsi_start()
{
  uint32_t r;

  r= GENCS;
  r|= (1 << 2);
  GENCS= r;

  r= DATA;
  r|= 1 << 22;
  r&= ~0xffff;
  DATA= r;
}

int
tsi_progress()
{
  return (GENCS & (1<<3))?1:0;
}

int
tsi_done()
{
  return (GENCS & (1<<2))?1:0;
}

int
tsi_get()
{
  uint32_t r;

  r= GENCS;
  r|= (1 << 2);
  GENCS= r;

  tsi_raw= DATA & 0xffff;
  if (tsi_raw < tsi_base)
    tsi_value= 0;
  else
    tsi_value= tsi_raw-tsi_base;
  return tsi_value;
}

int
tsi_scan()
{
  if (tsi_progress())
    while (!tsi_done())
      ;
  tsi_start();
  while (!tsi_done())
    ;
  return tsi_get();
}

void
tsi_loop()
{
	if (tsi_done())
	{
		tsi_get();
        tsi_start();
    }
}

int
tsi_touch()
{
  int b;
  b= tsi_base+5;
  if (tsi_raw > b)
    return 1;
  return 0;
}

int
tsi_touched()
{
  if (tsi_prev_on != tsi_touch())
    {
      tsi_prev_on= tsi_touch();
      return tsi_prev_on;
    }
  return 0;
}

int
tsi_released()
{
  if (tsi_prev_off != tsi_touch())
    {
      tsi_prev_off= tsi_touch();
      return tsi_prev_off?0:1;
    }
  return 0;
}
