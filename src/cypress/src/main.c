// https://github.com/Infineon/mtb-example-psoc6-capsense-buttons-slider/blob/master/source/main.c

#include "cybsp.h"
#include "cycfg.h"
#include "cycfg_capsense.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "led.h"
//#include <inttypes.h>
//#include "cy_pdl.h"
//#include "cybsp.h"
//#include "SpiEpmployee.h"

#define CAPSENSE_INTR_PRIORITY (7u)
#define EZI2C_INTR_PRIORITY                                                    \
  (6u) /* EZI2C interrupt priority must be                                     \
        * higher than CapSense interrupt */

static uint32_t initialize_capsense(void);
static void process_touch(void);
static void initialize_capsense_tuner(void);
static void capsense_isr(void);
static void capsense_callback();
void handle_error(void);

cy_stc_scb_ezi2c_context_t ezi2c_context;
cyhal_ezi2c_t sEzI2C;
cyhal_ezi2c_slave_cfg_t sEzI2C_sub_cfg;
cyhal_ezi2c_cfg_t sEzI2C_cfg;
volatile bool capsense_scan_complete = false;

/** User defined error handling function */
void handle_error(void) {
  /* Disable all interrupts. */
  __disable_irq();

  CY_ASSERT(0);
}

// https://github.com/Infineon/mtb-example-hal-spi-slave/blob/master/main.c
/* SPI baud rate in Hz */
#define SPI_FREQ_HZ                (1000000UL)
/* SPI transfer bits per frame */
#define BITS_PER_FRAME             (8)
void handle_eror(uint32_t status)
{
    if (status != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
}

/**
 * System entrance point. This function performs
 *
 * - initial setup of device
 * - initialize CapSense
 * - initialize tuner communication
 * - scan touch input continuously and update the LED accordingly.
 */
int main(void) {
  cy_rslt_t result;
  cyhal_spi_t sSPI;
  /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen. */
  printf("\x1b[2J\x1b[;H");
#if defined(CY_DEVICE_SECURE)
  cyhal_wdt_t wdt_obj;

  printf("Clear watchdog timer so that it doesn't trigger a reset\r\n");
  result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
  CY_ASSERT(CY_RSLT_SUCCESS == result);
  cyhal_wdt_free(&wdt_obj);
#endif

  printf("Initialize the device and board peripherals\r\n");
  result = cybsp_init();

  if (result != CY_RSLT_SUCCESS) {
	  printf("Board init failed. Stop program execution\r\n");
    CY_ASSERT(0);
  }

  printf("Enable global interrupts\r\n");
  __enable_irq();

  printf("Initialize retarget-io to use the debug UART port.\r\n");
  result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
  if (CYRET_SUCCESS != result) CY_ASSERT(0);

  printf("init them led, capsense tuner, capsense itself\r\n");
  initialize_led();
  initialize_capsense_tuner();
  result = initialize_capsense();

  if (CYRET_SUCCESS != result) {
    printf("Halt the CPU if CapSense initialization failed\r\n");
    CY_ASSERT(0);
  }


  printf("********************************************************\r\n"
		 "help me lord\r\n"
		 "********************************************************\r\n");

  printf("Initiate first scan\r\n");
  Cy_CapSense_ScanAllWidgets(&cy_capsense_context);

//  status = init_employee();
//  if(status == INIT_FAILURE) CY_ASSERT(CY_ASSERT_FAILED);

  // https://github.com/Infineon/mtb-example-hal-spi-slave/blob/master/main.c
//  printf("SPI has been INITIATED\r\n");
//  result = cyhal_spi_init(&sSPI,
//		  CYBSP_QSPI_D3 //CYBSP_SPI_MOSI
//		  ,CYBSP_QSPI_D2   //CYBSP_SPI_MISO
//		  ,CYBSP_QSPI_D1   //CYBSP_SPI_CLK
//		  ,CYBSP_QSPI_D0   //CYBSP_SPI_CS
//		  ,NULL,BITS_PER_FRAME,
//                              CYHAL_SPI_MODE_00_MSB,true);
//  handle_eror(result);
//  printf("the FREQUENCY !!!\r\n");
//  result = cyhal_spi_set_frequency(&sSPI, SPI_FREQ_HZ);
//  handle_eror(result);


  for (;;) {
    if (capsense_scan_complete) {
      /* Process all widgets */
      Cy_CapSense_ProcessAllWidgets(&cy_capsense_context);

      /* Process touch input */
      process_touch();

      /* Establishes synchronized operation between the CapSense
       * middleware and the CapSense Tuner tool.
       */
      Cy_CapSense_RunTuner(&cy_capsense_context);

      /* Initiate next scan */
      Cy_CapSense_ScanAllWidgets(&cy_capsense_context);

      capsense_scan_complete = false;
    }


  }
}

/**
 * Gets the details of touch position detected, processes the touch input
 * and updates the LED status.
 */
static void process_touch(void) {
  uint32_t button0_status;
  uint32_t button1_status;
  cy_stc_capsense_touch_t *slider_touch_info;
  uint16_t slider_pos;
  uint8_t slider_touch_status;
  bool led_update_req = false;

  static uint32_t button0_status_prev;
  static uint32_t button1_status_prev;
  static uint16_t slider_pos_prev;
  static uint16_t slider_status_prev;
  static led_data_t led_data = {LED_ON, LED_MAX_BRIGHTNESS};

  /* Get button 0 status */
  button0_status = Cy_CapSense_IsSensorActive(CY_CAPSENSE_BUTTON0_WDGT_ID,
                                              CY_CAPSENSE_BUTTON0_SNS0_ID,
                                              &cy_capsense_context);

  /* Get button 1 status */
  button1_status = Cy_CapSense_IsSensorActive(CY_CAPSENSE_BUTTON1_WDGT_ID,
                                              CY_CAPSENSE_BUTTON1_SNS0_ID,
                                              &cy_capsense_context);

  /* Get slider status */
  slider_touch_info = Cy_CapSense_GetTouchInfo(
      CY_CAPSENSE_LINEARSLIDER0_WDGT_ID, &cy_capsense_context);
  slider_touch_status = slider_touch_info->numPosition;
  slider_pos = slider_touch_info->ptrPosition->x;

  /* Detect new touch on Button0 */
  if ((0u != button0_status) && (0u == button0_status_prev)) {
    led_data.state = LED_ON;
    led_update_req = true;
  }

  /* Detect new touch on Button1 */
  if ((0u != button1_status) && (0u == button1_status_prev)) {
    led_data.state = LED_OFF;
    led_update_req = true;
  }

  /* Detect the new touch on slider */
  if ((0 != slider_touch_status) && (slider_pos != slider_pos_prev)) {
    led_data.brightness =
        100 - (slider_pos * 100) /
        cy_capsense_context.ptrWdConfig[CY_CAPSENSE_LINEARSLIDER0_WDGT_ID]
            .xResolution;
    led_update_req = true;
  }

  /* Update the LED state if requested */
  if (led_update_req) {
    update_led_state(&led_data);

  }

  if (button0_status != button0_status_prev || button1_status_prev != button1_status || slider_pos_prev != slider_pos
		  || slider_status_prev != slider_touch_status) {
//	  printf("\r\nbutton0_status=%lu, button1_status=%lu, slider_pos=%u, slider_touch_status=%u, led_update_req=%u\r\n",
//			  button0_status, button1_status, slider_pos, slider_touch_status, led_update_req);
//		for (int i = 0; i < slider_touch_status; i++) {
//		  printf("slider_touch_info->ptrPosition[%i].x = %u\r\n", i, slider_touch_info->ptrPosition[i].x);
//		}
	  if (slider_touch_status) {
		  int slider = slider_pos / 5;
		  for (int i = 0; i < slider; i++) {
			  printf("=");
		  }
		  for (int i = slider; i < 60; i++) {
			  printf(" ");
		  }
	  } else {
		  for (int i = 0; i < 60; i++) {
			  printf(".");
		  }
	  }
	  printf(" ");
	  if (button0_status != 0) printf("LEFT");
	  else printf("left");
	  if (button1_status != 0) printf("RIGHT");
	  else printf("right");
	  printf("\r\n");
//	  uint32_t     transmit_data = ();
//	  if (CY_RSLT_SUCCESS == cyhal_spi_send(&sSPI, transmit_data)) {
//
//	  }
  }

  /* Update previous touch status */
  button0_status_prev = button0_status;
  button1_status_prev = button1_status;
  slider_pos_prev = slider_pos;
  slider_status_prev = slider_touch_status;

}

/**
 * This function initializes the CapSense and configure the CapSense interrupt.
 */
static uint32_t initialize_capsense(void) {
  uint32_t status = CYRET_SUCCESS;

  /* CapSense interrupt configuration parameters */
  static const cy_stc_sysint_t capSense_intr_config = {
      .intrSrc = csd_interrupt_IRQn,
      .intrPriority = CAPSENSE_INTR_PRIORITY,
  };

  /* Capture the CSD HW block and initialize it to the default state. */
  status = Cy_CapSense_Init(&cy_capsense_context);
  if (CYRET_SUCCESS != status) {
    return status;
  }

  /* Initialize CapSense interrupt */
  cyhal_system_set_isr(csd_interrupt_IRQn, csd_interrupt_IRQn,
                       CAPSENSE_INTR_PRIORITY, &capsense_isr);
  NVIC_ClearPendingIRQ(capSense_intr_config.intrSrc);
  NVIC_EnableIRQ(capSense_intr_config.intrSrc);

  /* Initialize the CapSense firmware modules. */
  status = Cy_CapSense_Enable(&cy_capsense_context);
  if (CYRET_SUCCESS != status) {
    return status;
  }

  /* Assign a callback function to indicate end of CapSense scan. */
  status = Cy_CapSense_RegisterCallback(
      CY_CAPSENSE_END_OF_SCAN_E, capsense_callback, &cy_capsense_context);
  if (CYRET_SUCCESS != status) {
    return status;
  }

  return status;
}

/** Wrapper function for handling interrupts from CapSense block. */
static void capsense_isr(void) {
  Cy_CapSense_InterruptHandler(CYBSP_CSD_HW, &cy_capsense_context);
}

/**
 * This function sets a flag to indicate end of a CapSense scan.
 *
 * cy_stc_active_scan_sns_t* : pointer to active sensor details.
 */
void capsense_callback(cy_stc_active_scan_sns_t *ptrActiveScan) {
  capsense_scan_complete = true;
}

/** Initializes interface between Tuner GUI and PSoC 6 MCU. */
static void initialize_capsense_tuner(void) {
  cy_rslt_t result;

  /* Configure Capsense Tuner as EzI2C Slave */
  sEzI2C_sub_cfg.buf = (uint8 *)&cy_capsense_tuner;
  sEzI2C_sub_cfg.buf_rw_boundary = sizeof(cy_capsense_tuner);
  sEzI2C_sub_cfg.buf_size = sizeof(cy_capsense_tuner);
  sEzI2C_sub_cfg.slave_address = 8U;

  sEzI2C_cfg.data_rate = CYHAL_EZI2C_DATA_RATE_400KHZ;
  sEzI2C_cfg.enable_wake_from_sleep = false;
  sEzI2C_cfg.slave1_cfg = sEzI2C_sub_cfg;
  sEzI2C_cfg.sub_address_size = CYHAL_EZI2C_SUB_ADDR16_BITS;
  sEzI2C_cfg.two_addresses = false;

  result = cyhal_ezi2c_init(&sEzI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL,
                            &sEzI2C_cfg);
  if (result != CY_RSLT_SUCCESS) {
    handle_error();
  }
}
