#include "types.h"

#include "dev_adc.h"


static uint chval(int ch)
{
  switch (ch)
  {
    case  0: return ADC_CHANNEL_0 ; break;
    case  1: return ADC_CHANNEL_1 ; break;
    case  2: return ADC_CHANNEL_2 ; break;
    case  3: return ADC_CHANNEL_3 ; break;
    case  4: return ADC_CHANNEL_4 ; break;
    case  5: return ADC_CHANNEL_5 ; break;
    case  6: return ADC_CHANNEL_6 ; break;
    case  7: return ADC_CHANNEL_7 ; break;
    case  8: return ADC_CHANNEL_8 ; break;
    case  9: return ADC_CHANNEL_9 ; break;
    case 10: return ADC_CHANNEL_10; break;
    case 11: return ADC_CHANNEL_11; break;
    case 12: return ADC_CHANNEL_12; break;
    case 13: return ADC_CHANNEL_13; break;
    case 14: return ADC_CHANNEL_14; break;
    case 15: return ADC_CHANNEL_15; break;
    case 16: return ADC_CHANNEL_16; break;
    case 17: return ADC_CHANNEL_17; break;
  }
  return 0;
}


int dev_adc_read(ADC_HandleTypeDef *h, int ch)
{
  int v;
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = chval(ch);
  sConfig.Rank = 1;
  sConfig.SamplingTime =
#ifdef DEV_L4
      ADC_SAMPLETIME_92CYCLES_5;
#else
      ADC_SAMPLETIME_84CYCLES;
#endif
  HAL_ADC_ConfigChannel(h, &sConfig);
  HAL_ADC_Start(h);
  HAL_ADC_PollForConversion(h, 1000);
  v= HAL_ADC_GetValue(h);
  HAL_ADC_Stop(h);
  return v;
}
