// https://github.com/Infineon/mtb-example-psoc6-capsense-buttons-slider/blob/master/source/led.h

#ifndef SOURCE_LED_H_
#define SOURCE_LED_H_

#include <stdio.h>

/* Allowed duty cycle for maximum brightness */
#define LED_MAX_BRIGHTNESS (100u)

/* Allowed duty cycle for minimum brightness*/
#define LED_MIN_BRIGHTNESS (2u)

typedef enum { LED_OFF, LED_ON } led_state_t;

typedef struct {
  led_state_t state;
  uint32_t brightness;
} led_data_t;

cy_rslt_t initialize_led(void);
void update_led_state(led_data_t *led_data);

#endif
