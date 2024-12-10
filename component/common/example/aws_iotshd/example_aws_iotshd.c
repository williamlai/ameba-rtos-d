#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "wifi_conf.h"
#include "wifi_ind.h"

#include <lwip/sockets.h>
#include <lwip_netconf.h>
#include <lwip/netif.h>

#include <sntp/sntp.h>

#include <iotshd_main.h>

static void aws_iotshd_thread(void *param)
{
    while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
    {
        printf("Waiting for WIFI connection...\n");
        vTaskDelay(10000);
    }

    sntp_init();

    aws_iotshd_main();

    vTaskDelete(NULL);
}

void example_aws_iotshd(void)
{
    if (xTaskCreate(aws_iotshd_thread, ((const char *)"aws_iotshd_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(aws_iotshd_thread) failed", __FUNCTION__);
}