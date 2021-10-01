#include "Def.h"
#include "ni_usb3_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include  "testIpnewcore5550_lib.h"

#define BOARD_SERIAL_NUMBER "NI120012"




int main(int argc, char* argv[])
{
	NI_HANDLE handle;
	int ret;
	uint32_t    val;


	USB3_Init();


	if(USB3_ConnectDevice(BOARD_SERIAL_NUMBER, &handle) != 0) { printf("Unable to connect to the board!\n"); return (-1); };
#ifndef CUSTOM_EXAMPLE		
	
	

	
#else

#endif

	return 0;
}

 