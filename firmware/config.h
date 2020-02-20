/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#ifndef CONFIG_H
#define CONFIG_H

// TEST
#define DEVICE_TEST_TOGGLE_TIMEOUT_MS               5000

// =============================== PINs =======================================
#define DEVICE_POWER_PIN                            A3

#define DEVICE_MIN_DM_DP_DELAY_MS                   10
#define DEVICE_MIN_PERIOD_DELAY_MS                  1000

#define DEVICE_DEAFULT_DELAY_MS                     200
#define DEVICE_TEST_TIMEOUT_MS                      300

#define MULTIPLEXOR_TARGET_ENABLE_PIN               A4
#define MULTIPLEXOR_TARGET_SWITCH_PIN               A5

#define ADC_VOLTAGE_NORMAL_PIN                      A1
#define ADC_VOLTAGE_GLITCH_PIN                      A2

// ============================= ADC ===========================================
#define ADC_MEASURE_TIMEOUT_MS                      1000
#define ADC_REFERENCE_mV                            3300
#define ADC_RESOLUTION_BIT                          12
#define ADC_SAMPLE_RATE_160_5_CLK                   3

#define ADC_RESISTOR_DIVIDER                        (3.2)

// =============================== LEDS ========================================
#define LED_COUNT                                   4
#define LED_RED_PIN                                 A1
#define LED_BLUE_PIN                                A2
#define LED_ORANGE_PIN                              A15

#define LED_MAX_BRIGHT                              0xFF
#define LED_BLINK_FREQ_HZ                           25
#define LED_BLINK_TIMES                             3

#define LED_TIM_REG                                 TIM2
#define LED_TIM                                     TIM_2
#define LED_RED_CHANNEL                             TIM_CHANNEL2
#define LED_BLUE_CHANNEL                            TIM_CHANNEL3
#define LED_ORANGE_CHANNEL                          TIM_CHANNEL4

#define LED_BLINK_TIM_REG                           TIM3
#define LED_BLINK_TIM                               TIM_3
#define LED_BLINK_TIM_CHANNEL                       TIM_CHANNEL1
#define LED_BLINK_IRQ_VECTOR                        TIM3_IRQn

// ================================ LCD ========================================
#define LCD_TITLE_ROW                               0
#define LCD_ADC_NORMAL_MEAS_ROW                     1
#define LCD_ADC_GLITCH_MEAS_ROW                     2
#define LCD_ADC_VALUE_COLUMN                        7
#define LCD_ADC_MEAS_SCALE_COLUMN                   12

#define LCD_DEVICE_SWITCH_MODE_ROW                  4
#define LCD_DEVICE_SWITCH_MODE_COLUMN               5
#define LCD_DEVICE_STATE_ROW                        5


//#define LCD_FONT_WIDTH                              4
#define LCD_FONT_WIDTH                              5

#define LCD_DOT_FULL                                0x07
#define LCD_DOT_EMPTY                               0x09
#define LCD_BITBYTE                                 1
#define LCD_SPI                                     0

#define LCD_UPDATE_MS                               250
#define LCD_WIDTH                                   84
#define LCD_HEIGHT                                  48

#define LCD_STR_HEIGHT                              8
#define LCD_STR_WIDTH                               16

/* PINS */
#define LCD_BCKLT_PIN                               B0
#define LCD_RESET_PIN                               B1
#define LCD_CE_PIN                                  B2
#define LCD_SCLK_PIN                                B3
#define LCD_DC_PIN                                  B6
#define LCD_DIN_PIN                                 B7

// =============================== USB =========================================
#define USB_PORT_NUM                                USB_0
#define USBD_PROCESS_SIZE                           1200
#define USBD_PROCESS_PRIORITY                       150

// ============================== CCID  ========================================
#define USB_CCID_INTERFACE                          0
#define USB_HID_INTERFACE                           0

// =============================== COMM ========================================
#define COMM_COMMAND_END                            0x0D

// ================================= DEBUG =====================================

#define DBG_CONSOLE                                 UART_1
#define DBG_CONSOLE_TX_PIN                          A9
#define DBG_CONSOLE_BAUD                            115200

#define TEST_ROUNDS                                 10000

#define APP_DEBUG                                   1
#define APP_DEBUG_ERRORS                            1

#define APP_DEBUG_USB                               0
#define APP_DEBUG_HID                               0
#define APP_DEBUG_DEVICE                            0

#endif // CONFIG_H
