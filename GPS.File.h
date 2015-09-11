#ifndef _GPS_FILE_
#define	_GPS_FILE_

#include "string_t.h"
#include "vmgps.h"

#include "vmfs.h"

#include "vmsystem.h"
#include "vmboard.h"
#include "vmtype.h"
#include "vmlog.h"

VMCHAR filename[VM_FS_MAX_PATH_LENGTH] = {0};

VMUINT writelen;
VMINT ret;
VMUINT readlen;
VMINT ii= 0;

VM_FS_HANDLE filehandle;


void Create_data_logger(void){

	// Initialization

    VM_FS_HANDLE filehandle = -1;

    VMCHAR filename[VM_FS_MAX_PATH_LENGTH] = {0};
    VMWCHAR path[VM_FS_MAX_PATH_LENGTH] = {0};

    vm_log_info("Creating Data Logger");

    strcpy(path,"C:\\gps_data.txt");
    vm_chset_ascii_to_ucs2(filename, sizeof(filename), path);

    // create file
    if ((filehandle = vm_fs_open(filename, VM_FS_MODE_CREATE_ALWAYS_WRITE, TRUE)) < 0)
    {
        vm_log_info("Create Data Logger: Failed (%s)",path);
        return;
    }

    vm_log_info("value of filehandle in open mode: %d", filehandle);
    vm_log_info("Create Data Logger: Success (%s)", path);

}


void gps_data(VM_GPS_MESSAGE message, void* data, void* user_data){

	vm_log_info("Reading from GPS...");
	vm_log_info("NMEA Sentence is:%s", data);

	data_write (data);
	vm_log_info("%d", ii);
	if (ii == 100){

		VM_FS_HANDLE filehandle = 4;

		vm_fs_close(filehandle);

	}
	ii++;

 }

void Start_loging(void){

	vm_gps_open (VM_GPS_ONLY, gps_data, NULL);
}

void data_write (void* write_data){

	VM_FS_HANDLE filehandle = 4;

	ret = vm_fs_write(filehandle, (void*)write_data, strlen(write_data), &writelen);

	if (ret < 0)
	    {
	        vm_log_info("Failed to Log");
	        return;
	    }

	vm_log_info("Logged data: %s", write_data);

}

#endif

