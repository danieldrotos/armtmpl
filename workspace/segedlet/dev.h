#ifndef DEV_HEADER
#define DEV_HEADER

#ifdef STM32F410Rx
// nucleo 410R
#define DEV_N410
#define DEV_F4
#include "stm32f4xx_hal.h"

#elif defined STM32F411xE
// nucleo 411R
#define DEV_N411
#define DEV_F4
#include "stm32f4xx_hal.h"

#elif defined STM32F401xE
// nucleo 401R
#define DEV_N401
#define DEV_F4
#include "stm32f4xx_hal.h"

#elif defined STM32G491xx
// nucleo 491
#define DEV_N491
#define DEV_G4
#include "stm32f4xx_hal.h"

#elif defined STM32F407xx
// discovery 407
#define DEV_F4DISC
#define DEV_F4
#include "stm32f4xx_hal.h"

#elif defined STM32F401xC
// DOA-F4
#define DEV_DOAF4
#define DEV_F4
#include "stm32f4xx_hal.h"

#elif defined STM32L433xx
// nucleo L433
#define DEV_L433
#define DEV_L4
#include "stm32l4xx_hal.h"

#endif


#endif /* DEV_HEADER */
