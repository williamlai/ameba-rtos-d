/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <platform_opts_bt.h>
#if defined(CONFIG_BT_OTA_CLIENT) && CONFIG_BT_OTA_CLIENT
#include <stdio.h>
#include <basic_types.h>
#include <os_msg.h>
#include <os_task.h>
#include <gap.h>
#include <gap_le.h>
#include <gap_msg.h>
#include <trace_app.h>
#include <app_msg.h>
#include <bt_ota_client_app_task.h>
#include <bt_ota_client_app.h>


/** @defgroup  CENTRAL_CLIENT_APP_TASK Central Client App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define BT_OTA_CLIENT_APP_TASK_PRIORITY             1         //!< Task priorities
#define BT_OTA_CLIENT_APP_TASK_STACK_SIZE           256 * 6   //!<  Task stack size
#define BT_OTA_CLIENT_MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
#define BT_OTA_CLIENT_MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
#define BT_OTA_CLIENT_MAX_NUMBER_OF_EVENT_MESSAGE   (BT_OTA_CLIENT_MAX_NUMBER_OF_GAP_MESSAGE + BT_OTA_CLIENT_MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *bt_ota_client_app_task_handle;   //!< APP Task handle
void *bt_ota_client_evt_queue_handle;  //!< Event queue handle
void *bt_ota_client_io_queue_handle;   //!< IO queue handle

/*============================================================================*
 *                              Functions
 *============================================================================*/
void bt_ota_client_app_send_msg(uint16_t subtype, void *buf)
{
	uint8_t event = EVENT_IO_TO_APP;
	T_IO_MSG io_msg;

	io_msg.type = IO_MSG_TYPE_QDECODE;
	io_msg.subtype = subtype;
	io_msg.u.buf = buf;

	if (bt_ota_client_evt_queue_handle != NULL && bt_ota_client_io_queue_handle != NULL) {
		if (os_msg_send(bt_ota_client_io_queue_handle, &io_msg, 0) == false) {
			printf("bt ota client app send msg fail: subtype 0x%x\r\n", io_msg.subtype);
		} else if (os_msg_send(bt_ota_client_evt_queue_handle, &event, 0) == false) {
			printf("bt ota client app send event fail: subtype 0x%x\r\n", io_msg.subtype);
		}
	}
}

/**
 * @brief  Initialize App task
 * @return void
 */
void bt_ota_client_app_task_init()
{
    os_task_create(&bt_ota_client_app_task_handle, "bt_ota_client_app", bt_ota_client_app_main_task, 0,
				   BT_OTA_CLIENT_APP_TASK_STACK_SIZE, BT_OTA_CLIENT_APP_TASK_PRIORITY);
}

/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void bt_ota_client_app_main_task(void *p_param)
{
    (void) p_param;
    uint8_t event;

    os_msg_queue_create(&bt_ota_client_io_queue_handle, BT_OTA_CLIENT_MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&bt_ota_client_evt_queue_handle, BT_OTA_CLIENT_MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));

    gap_start_bt_stack(bt_ota_client_evt_queue_handle, bt_ota_client_io_queue_handle, BT_OTA_CLIENT_MAX_NUMBER_OF_GAP_MESSAGE);

    while (true)
    {
        if (os_msg_recv(bt_ota_client_evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG io_msg;
                if (os_msg_recv(bt_ota_client_io_queue_handle, &io_msg, 0) == true)
                {
                    bt_ota_client_app_handle_io_msg(io_msg);
                }
            }
            else
            {
                gap_handle_msg(event);
            }
        }
    }
}

void bt_ota_client_app_task_deinit(void)
{
	if (bt_ota_client_app_task_handle) {
		os_task_delete(bt_ota_client_app_task_handle);
	}
	if (bt_ota_client_io_queue_handle) {
		os_msg_queue_delete(bt_ota_client_io_queue_handle);
	}
	if (bt_ota_client_evt_queue_handle) {
		os_msg_queue_delete(bt_ota_client_evt_queue_handle);
	}
	bt_ota_client_io_queue_handle = NULL;
	bt_ota_client_evt_queue_handle = NULL;
	bt_ota_client_app_task_handle = NULL;

	bt_ota_client_gap_dev_state.gap_init_state = 0;
	bt_ota_client_gap_dev_state.gap_adv_sub_state = 0;
	bt_ota_client_gap_dev_state.gap_adv_state = 0;
	bt_ota_client_gap_dev_state.gap_scan_state = 0;
	bt_ota_client_gap_dev_state.gap_conn_state = 0;

	bt_ota_client_app_max_links = 0;

}

/** @} */ /* End of group CENTRAL_CLIENT_APP_TASK */
#endif

