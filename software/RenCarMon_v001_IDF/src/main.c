/**
 * @file main.c
 * @author D.T.Mustvanga (dtmutsvanga@yahoo.com)
 * @brief RenCarMon v001 Car monitor firmware
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright Rentuku Automation(c) 2020
 * 
 */

//#include "pinout.h"

#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"

uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void task_blink(void* ignore)
{
    gpio16_output_conf();
    while(true) {
    	gpio16_output_set(0);
        vTaskDelay(1000/portTICK_RATE_MS);
    	gpio16_output_set(1);
        vTaskDelay(1000/portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    xTaskCreate(&task_blink, "startup", 2048, NULL, 1, NULL);
}
