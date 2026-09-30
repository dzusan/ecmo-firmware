/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DBG1_Pin GPIO_PIN_2
#define DBG1_GPIO_Port GPIOE
#define DBG2_Pin GPIO_PIN_3
#define DBG2_GPIO_Port GPIOE
#define PWR_9V_EN_Pin GPIO_PIN_13
#define PWR_9V_EN_GPIO_Port GPIOC
#define PWR_GOOD_Pin GPIO_PIN_14
#define PWR_GOOD_GPIO_Port GPIOC
#define PS_ON_Pin GPIO_PIN_15
#define PS_ON_GPIO_Port GPIOC
#define SEN_FLOW_F_Pin GPIO_PIN_0
#define SEN_FLOW_F_GPIO_Port GPIOF
#define SEN_FLOW_R_Pin GPIO_PIN_1
#define SEN_FLOW_R_GPIO_Port GPIOF
#define SEN_FLOW_C_Pin GPIO_PIN_2
#define SEN_FLOW_C_GPIO_Port GPIOF
#define HALL_U_Pin GPIO_PIN_2
#define HALL_U_GPIO_Port GPIOB
#define HALL_V_Pin GPIO_PIN_11
#define HALL_V_GPIO_Port GPIOF
#define HALL_W_Pin GPIO_PIN_12
#define HALL_W_GPIO_Port GPIOF
#define ENC_A_Pin GPIO_PIN_13
#define ENC_A_GPIO_Port GPIOF
#define ENC_B_Pin GPIO_PIN_14
#define ENC_B_GPIO_Port GPIOF
#define ENC_BTN_Pin GPIO_PIN_15
#define ENC_BTN_GPIO_Port GPIOF
#define LTDC_U_LR_Pin GPIO_PIN_0
#define LTDC_U_LR_GPIO_Port GPIOG
#define LTDC_U_UD_Pin GPIO_PIN_1
#define LTDC_U_UD_GPIO_Port GPIOG
#define LTDC_U_MODE_Pin GPIO_PIN_7
#define LTDC_U_MODE_GPIO_Port GPIOE
#define GATE_BAT_Pin GPIO_PIN_8
#define GATE_BAT_GPIO_Port GPIOE
#define GATE_SUP_Pin GPIO_PIN_9
#define GATE_SUP_GPIO_Port GPIOE
#define LTDC_U_RST_Pin GPIO_PIN_10
#define LTDC_U_RST_GPIO_Port GPIOE
#define LTDC_U_DITHB_Pin GPIO_PIN_9
#define LTDC_U_DITHB_GPIO_Port GPIOD
#define PWM_BACKLIGHT_Pin GPIO_PIN_11
#define PWM_BACKLIGHT_GPIO_Port GPIOD
#define KEY7_Pin GPIO_PIN_2
#define KEY7_GPIO_Port GPIOG
#define KEY6_Pin GPIO_PIN_3
#define KEY6_GPIO_Port GPIOG
#define KEY5_Pin GPIO_PIN_4
#define KEY5_GPIO_Port GPIOG
#define KEY4_Pin GPIO_PIN_5
#define KEY4_GPIO_Port GPIOG
#define KEY3_Pin GPIO_PIN_6
#define KEY3_GPIO_Port GPIOG
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOG
#define KEY1_Pin GPIO_PIN_8
#define KEY1_GPIO_Port GPIOG
#define LED_Pin GPIO_PIN_4
#define LED_GPIO_Port GPIOD
#define DISP_SD_CS_Pin GPIO_PIN_6
#define DISP_SD_CS_GPIO_Port GPIOD
#define DISP_DC_Pin GPIO_PIN_15
#define DISP_DC_GPIO_Port GPIOG
#define DISP_LCD_CS_Pin GPIO_PIN_6
#define DISP_LCD_CS_GPIO_Port GPIOB
#define DISP_RST_Pin GPIO_PIN_7
#define DISP_RST_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
