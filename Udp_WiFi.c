/* 
  Gets the time data from NTP server by UDP.

  You can change the NTP server address by modify g_address.
*/
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
#include "Udp_WFi.h"
#include "vmsock.h"
#include "vmbearer.h"
#include "vmudp.h"
#include "vmtimer.h"

#include "vmwlan.h"



void vm_main(void) 
{
	wlan_connect();
	vm_timer_create_non_precise(60000, send_udp, NULL);
}

