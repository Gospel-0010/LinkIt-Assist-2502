#ifndef _LINKIT_APP_WIZARDTEMPLATE_
#define	_LINKIT_APP_WIZARDTEMPLATE_

#include "string_t.h"
#include "vmtype.h"
#include "vmlog.h"
#include "ResID.h"
#include "vmwlan.h"

#include "vmboard.h"
#include "vmcmd.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"
#include "vmtimer.h"
#include "vmhttps.h"

/* SSID Connection Details */
#define AP_SSID "ABC"
#define AP_PASSWORD "XYZ"
#define AP_AUTHORIZE_MODE   VM_WLAN_AUTHORIZE_MODE_WPA2_ONLY_PSK

/* HTTP Details */
#define VMHTTPS_TEST_DELAY 60000    /* 60 seconds */
#define VMHTTPS_TEST_URL "http://pettracing.com/api/test"

VMUINT8 g_channel_id;
VMINT g_read_seg_num;

void wlan_connect_callback(
            void *user_data,
            vm_wlan_connect_result_t* connect_result)
{
	vm_log_info("connect result: %d", connect_result->result);

    if(VM_WLAN_SUCCESS == connect_result->result)
    {
        vm_log_info("Connect to AP successfully\n");

    	vm_log_info("AP info shows below:\n");
    	vm_log_info("connect cause: %u", connect_result->cause);
    	vm_log_info("connect bssid: %s", connect_result->bssid);
    	vm_log_info("connect ssid: %s", connect_result->ssid);
    }
    else{
    	vm_log_info("Cannot connect to WiFi Successfully");
    }
}


void wlan_callback(void* user_data, VM_WLAN_REQUEST_RESULT result_type)
{
		vm_wlan_ap_info_t ap_info= {0};

		strcpy(ap_info.ssid, AP_SSID);
		strcpy(ap_info.password, AP_PASSWORD);
		ap_info.authorize_mode = AP_AUTHORIZE_MODE;

		vm_wlan_connect(&ap_info, wlan_connect_callback, NULL);

}

void wlan_connect(void)
{

	vm_wlan_mode_set(VM_WLAN_MODE_STA, wlan_callback, NULL);

}

/* HTTPS CODE*/

static void https_send_request_set_channel_rsp_cb(VMUINT32 req_id, VMUINT8 channel_id, VMUINT8 result)
{
    VMINT ret = -1;

    ret = vm_https_send_request(
        0,                  				/* Request ID */
        VM_HTTPS_METHOD_GET,                /* HTTP Method Constant */
        VM_HTTPS_OPTION_NO_CACHE,           /* HTTP request options */
        VM_HTTPS_DATA_TYPE_BUFFER,          /* Reply type (wps_data_type_enum) */
        100,                     	        /* bytes of data to be sent in reply at a time. If data is more that this, multiple response would be there */
        (VMUINT8 *)VMHTTPS_TEST_URL,        /* The request URL */
        strlen(VMHTTPS_TEST_URL),           /* The request URL length */
        NULL,                               /* The request header */
        0,                                  /* The request header length */
        NULL,
        0);

    if (ret != 0) {
        vm_https_unset_channel(channel_id);
    }
}

static void https_unset_channel_rsp_cb(VMUINT8 channel_id, VMUINT8 result)
{
    vm_log_debug("https_unset_channel_rsp_cb()");
}
static void https_send_release_all_req_rsp_cb(VMUINT8 result)
{
    vm_log_debug("https_send_release_all_req_rsp_cb()");
}
static void https_send_termination_ind_cb(void)
{
    vm_log_debug("https_send_termination_ind_cb()");
}
static void https_send_read_request_rsp_cb(VMUINT16 request_id, VMUINT8 result,
                                         VMUINT16 status, VMINT32 cause, VMUINT8 protocol,
                                         VMUINT32 content_length,VMBOOL more,
                                         VMUINT8 *content_type, VMUINT8 content_type_len,
                                          VMUINT8 *new_url, VMUINT32 new_url_len,
                                         VMUINT8 *reply_header, VMUINT32 reply_header_len,
                                         VMUINT8 *reply_segment, VMUINT32 reply_segment_len)
{
    VMINT ret = -1;
    vm_log_debug("https_send_request_rsp_cb()");
    if (result != 0) {
        vm_https_cancel(request_id);
        vm_https_unset_channel(g_channel_id);
    }
    else {
        vm_log_debug("reply_content:%s", reply_segment);
        ret = vm_https_read_content(request_id, ++g_read_seg_num, 100);
        if (ret != 0) {
            vm_https_cancel(request_id);
            vm_https_unset_channel(g_channel_id);
        }
    }
}
static void https_send_read_read_content_rsp_cb(VMUINT16 request_id, VMUINT8 seq_num,
                                                 VMUINT8 result, VMBOOL more,
                                                 VMUINT8 *reply_segment, VMUINT32 reply_segment_len)
{
    VMINT ret = -1;
    vm_log_debug("reply_content:%s", reply_segment);
    if (more > 0) {
        ret = vm_https_read_content(
            request_id,                         /* Request ID */
            ++g_read_seg_num,                 	/* Sequence number (for debug purpose) */
            100);                               /* The suggested segment data length of replied data in the peer buffer of
                                                   response. 0 means use reply_segment_len in MSG_ID_WPS_HTTP_REQ or
                                                   read_segment_length in previous request. */
        if (ret != 0) {
            vm_https_cancel(request_id);
            vm_https_unset_channel(g_channel_id);
        }
    }
    else {
        /* don't want to send more requests, so unset channel */
        vm_https_cancel(request_id);
        vm_https_unset_channel(g_channel_id);
        g_channel_id = 0;
        g_read_seg_num = 0;

    }
}
static void https_send_cancel_rsp_cb(VMUINT16 request_id, VMUINT8 result)
{
    vm_log_debug("https_send_cancel_rsp_cb()");
}
static void https_send_status_query_rsp_cb(VMUINT8 status)
{
    vm_log_debug("https_send_status_query_rsp_cb()");
}

static void https_send_request(VM_TIMER_ID_NON_PRECISE timer_id, void* user_data)
{
     /*----------------------------------------------------------------*/
     /* Local Variables                                                */
     /*----------------------------------------------------------------*/
     VMINT ret = -1;
     VMINT apn = VM_BEARER_DATA_ACCOUNT_TYPE_WLAN;
     vm_https_callbacks_t callbacks = {
         https_send_request_set_channel_rsp_cb,
         https_unset_channel_rsp_cb,
         https_send_release_all_req_rsp_cb,
         https_send_termination_ind_cb,
         https_send_read_request_rsp_cb,
         https_send_read_read_content_rsp_cb,
         https_send_cancel_rsp_cb,
         https_send_status_query_rsp_cb
     };
    /*----------------------------------------------------------------*/
     /* Code Body                                                      */
     /*----------------------------------------------------------------*/

    do {
        vm_timer_delete_non_precise(timer_id);
        ret = vm_https_register_context_and_callback(
            apn, &callbacks);

        if (ret != 0) {
            break;
        }

        /* set network profile information */
        ret = vm_https_set_channel(
            0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0,
            0, 0
        );
    } while (0);


}

#endif

