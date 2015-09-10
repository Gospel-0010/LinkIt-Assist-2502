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

#include "Http_Wifi.h"

/*
* entry
*/
void vm_main(void) 
{
	wlan_connect();
	vm_timer_create_non_precise(60000, https_send_request, NULL);
}
