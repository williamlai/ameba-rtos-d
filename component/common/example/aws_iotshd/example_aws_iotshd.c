#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "osdep_service.h"

#include "wifi_conf.h"
#include "wifi_ind.h"

#include <lwip/sockets.h>
#include <lwip_netconf.h>
#include <lwip/netif.h>

#include <sntp/sntp.h>

#include <iotshd_main.h>

#include <mbedtls/threading.h>
#include <mbedtls/entropy.h>

static void mbedtls_threading_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    mutex->is_valid = 1;
    mutex->mutex = xSemaphoreCreateMutex();
}

static void mbedtls_threading_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
    {
        return;
    }

    vSemaphoreDelete(mutex->mutex);
    mutex->is_valid = 0;
}

static int mbedtls_threading_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
    {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (xSemaphoreTake(mutex->mutex, portMAX_DELAY) != pdTRUE)
    {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

static int mbedtls_threading_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid)
    {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (xSemaphoreGive(mutex->mutex) != pdTRUE)
    {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    ((void)data);

    printf("%s:%d(): len(%d)\n", __FUNCTION__, __LINE__, len);
    rtw_get_random_bytes(output, len);

    *olen = len;

    return 0;
}

static void aws_iotshd_thread(void *param)
{
    while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
    {
        printf("Waiting for WIFI connection...\n");
        vTaskDelay(10000);
    }

    sntp_init();

    mbedtls_threading_set_alt(mbedtls_threading_mutex_init,
                              mbedtls_threading_mutex_free,
                              mbedtls_threading_mutex_lock,
                              mbedtls_threading_mutex_unlock);

    aws_iotshd_main();

    vTaskDelete(NULL);
}

void example_aws_iotshd(void)
{
    if (xTaskCreate(aws_iotshd_thread, ((const char *)"aws_iotshd_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(aws_iotshd_thread) failed", __FUNCTION__);
}