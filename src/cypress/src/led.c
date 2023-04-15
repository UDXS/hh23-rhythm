// https://github.com/Infineon/mtb-example-psoc6-capsense-buttons-slider/blob/master/source/led.c

#include "cybsp.h"
#include "cyhal.h"

#include "led.h"

#define PWM_LED_FREQ_HZ (1000000lu) /* in Hz */
#define GET_DUTY_CYCLE(x) (100 - x)

led_state_t led_state_cur = LED_OFF;
cyhal_pwm_t pwm_led;

/**
 * This function updates the LED state, based on the touch input.
 *
 * ledData: the pointer to the LED data structure
 */
void update_led_state(led_data_t *ledData) {
  if ((led_state_cur == LED_OFF) && (ledData->state == LED_ON)) {
    cyhal_pwm_start(&pwm_led);
    led_state_cur = LED_ON;
    ledData->brightness = LED_MAX_BRIGHTNESS;
  } else if ((led_state_cur == LED_ON) && (ledData->state == LED_OFF)) {
    cyhal_pwm_stop(&pwm_led);
    led_state_cur = LED_OFF;
    ledData->brightness = 0;
  } else {
  }

  if ((LED_ON == led_state_cur) ||
      ((LED_OFF == led_state_cur) && (ledData->brightness > 0))) {
    cyhal_pwm_start(&pwm_led);
    uint32_t brightness = (ledData->brightness < LED_MIN_BRIGHTNESS)
                              ? LED_MIN_BRIGHTNESS
                              : ledData->brightness;

    /* Drive the LED with brightness */
    cyhal_pwm_set_duty_cycle(&pwm_led, GET_DUTY_CYCLE(brightness),
                             PWM_LED_FREQ_HZ);
    led_state_cur = LED_ON;
  }
}

/** Initializes a PWM resource for driving an LED. */
cy_rslt_t initialize_led(void) {
  cy_rslt_t rslt;

  rslt = cyhal_pwm_init(&pwm_led, CYBSP_USER_LED, NULL);

  if (CY_RSLT_SUCCESS == rslt) {
    rslt = cyhal_pwm_set_duty_cycle(
        &pwm_led, GET_DUTY_CYCLE(LED_MAX_BRIGHTNESS), PWM_LED_FREQ_HZ);
    if (CY_RSLT_SUCCESS == rslt) {
      rslt = cyhal_pwm_start(&pwm_led);
    }
  }

  if (CY_RSLT_SUCCESS == rslt) {
    led_state_cur = LED_ON;
  }

  return rslt;
}
