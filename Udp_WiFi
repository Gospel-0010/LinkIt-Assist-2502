#ifndef _LINKIT_APP_WIZARDTEMPLATE_
#define	_LINKIT_APP_WIZARDTEMPLATE_

#include <string_t.h>
#include "vmtype.h"
#include "vmboard.h"
#include "vmsystem.h"
#include "vmlog.h"
#include "vmcmd.h"
#include "vmdcl.h"
#include "vmdcl_gpio.h"
#include "vmthread.h"

#include "ResID.h"
#include "vmsock.h"
#include "vmbearer.h"
#include "vmudp.h"
#include "vmtimer.h"

#include "vmwlan.h"

#define AP_SSID "HOME-C722-2.4"
#define AP_PASSWORD "3F7FLNALFFDH93H7"
#define AP_AUTHORIZE_MODE   VM_WLAN_AUTHORIZE_MODE_WPA2_ONLY_PSK

#define TCP_SYNC_START_TIMER    60000   /* Start connection 60 seconds later after bootup */


#define NTP_PACKET_SIZE 48
static VM_UDP_HANDLE g_udp;
vm_soc_address_t g_address = {4, 123, {132, 163, 4, 101}};
VMUINT8 g_packetBuffer[NTP_PACKET_SIZE];

static void wlan_connect_callback(
            void *user_data,
            vm_wlan_connect_result_t* connect_result)
{
	vm_log_info("connect result:%d", connect_result->result);

    if(VM_WLAN_SUCCESS == connect_result->result)
    {
        vm_log_info("Connect to AP successfully");

    	vm_log_info("AP info shows below:");
    	vm_log_info("connect result:%d", connect_result->result);
    	vm_log_info("connect cause:%d", connect_result->cause);
    	vm_log_info("connect bssid:%s", connect_result->bssid);
    	vm_log_info("connect ssid:%s", connect_result->ssid);
    }
}


static void wlan_callback(void* user_data, VM_WLAN_REQUEST_RESULT result_type)
{
		vm_wlan_ap_info_t ap_info= {0};

		strcpy(ap_info.ssid, AP_SSID);
		strcpy(ap_info.password, AP_PASSWORD);
		ap_info.authorize_mode = AP_AUTHORIZE_MODE;

		vm_wlan_connect(&ap_info, wlan_connect_callback, NULL);

}

static void wlan_connect(void)
{

	vm_wlan_mode_set(VM_WLAN_MODE_STA, wlan_callback, NULL);

}

static void udp_callback(VM_UDP_HANDLE handle, VM_UDP_EVENT event)
{
    VMCHAR buf[100] = {0};
    VMINT ret = 0;
    VMINT nwrite;

    vm_log_debug("event:%d", event);
    switch (event)
    {
        case VM_UDP_EVENT_READ:
            ret = vm_udp_receive(g_udp, buf, 100, &g_address);
            if(ret > 0)
            {
                vm_log_debug("received data: %s", buf);
            }
            vm_udp_close(g_udp);
            break;
        case VM_UDP_EVENT_WRITE:
            nwrite = vm_udp_send(g_udp, g_packetBuffer, NTP_PACKET_SIZE, &g_address);
            break;
        default:
            break;
    }

}

static void send_udp(VM_TIMER_ID_NON_PRECISE timer_id, void* user_data)
{
    VMINT nwrite;

    vm_timer_delete_non_precise(timer_id);
    g_udp = vm_udp_create(1000, VM_BEARER_DATA_ACCOUNT_TYPE_WLAN, udp_callback, 0);
    memset(g_packetBuffer, 0, NTP_PACKET_SIZE);
    g_packetBuffer[0] = 0x11100011;   // LI, Version, Mode
    g_packetBuffer[1] = 0;     // Stratum, or type of clock
    g_packetBuffer[2] = 6;     // Polling Interval
    g_packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    g_packetBuffer[12]  = 49;
    g_packetBuffer[13]  = 0x4E;
    g_packetBuffer[14]  = 49;
    g_packetBuffer[15]  = 52;
    nwrite = vm_udp_send(g_udp, g_packetBuffer, NTP_PACKET_SIZE, &g_address);
}




#endif

