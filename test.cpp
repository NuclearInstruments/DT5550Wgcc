#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <stdlib.h>
#include "ni_usb3_wrapper.h"

int main(){
    uint32_t data[1024];
    uint32_t datar[2048];
    uint32_t written_data;
    uint32_t read_data;
    uint32_t valid_data;   
    char board_model[32];   
    int asic_count;  
    float temp;
    bool Enable;
    float voltage;
    float current;
    int SN;
    NI_HANDLE dt5550w;
    if (NI_USB3_ConnectDevice("NI120010", &dt5550w)) {
        printf("Unable to connect to the board");
        exit(0);
    }

    NI_USB3_SetIICControllerBaseAddress(2147811336, 2147811337, &dt5550w);
    NI_USB3_SetHV(true, 40, &dt5550w);
    NI_USB3_GetDT5550_DGBoardInfo(board_model, &asic_count, &SN, &dt5550w);
    printf("Board model: %s\n",board_model);
    printf("Asic Count:  %d\n",asic_count);
    printf("SN:          %d\n",SN);

    NI_USB3_GetHV(&Enable, &voltage, &current, &dt5550w);
    printf("HV enable:   %d\n",Enable);
    printf("HV voltage:  %f\n",voltage);
    printf("HV current:  %f\n",current);

    NI_USB3_GetDT5550WTempSens(0, &temp, &dt5550w);
    printf("Temp[0]:     %f\n",temp);
    NI_USB3_GetDT5550WTempSens(1, &temp, &dt5550w);
    printf("Temp[1]:     %f\n",temp);
exit(0);

    for (int i =0; i<1000;i++){
        NI_USB3_WriteData(data, 
                          1024,
                          0xFFFD0000,
                          REG_ACCESS,
                          1000, &dt5550w, 
                          &written_data);
        NI_USB3_ReadData(datar,
                          1024,
                          0xFFFD0000,
                          REG_ACCESS,
                          1000, &dt5550w,  
                          &read_data, 
                          &valid_data);


        for (int i=0;i<1024;i++)
            if (data[i]!=datar[i])
              printf("%d   :   %d\n", i, datar[i]);
        printf("."); fflush(stdout);
    }
    printf("\n\n");
}
/*ni_usb3_class dt5550w;


int main(){
	    // Open the port
   uint32_t data[1024];
   uint32_t datar[2048];
   uint32_t written_data;
   uint32_t read_data;
   uint32_t valid_data;
   int32_t devcount=0;
   char listofdevices[255];

   dt5550w.listdevices(listofdevices, NULL, &devcount);
   printf("List of devices [%d] on the USB3 bus: %s\n", devcount, listofdevices);

       if (!dt5550w.open("NI120010"))
        return -1;
    
    for (int i=0;i<1024;i++)
        data[i] = i*2;

    printf("Write!\n");
   dt5550w.write_mem(data, 1024, 0xFFFD0000, 0, 1000, &written_data);
    printf("Writen: %d!\n", written_data);
while(1){
    dt5550w.write_mem(data, 1024, 0xFFFD0000, 0, 1000, &written_data);
    dt5550w.read_mem(datar, 1024, 0xFFFD0000, 0, 1000, &read_data, &valid_data);
    for (int i=0;i<1024;i++)
        if (data[i]!=datar[i])
          printf("%d   :   %d\n", i, datar[i]);
    printf("."); fflush(stdout);
}
    printf("read_data: %d\n", read_data);
    printf("valid_data: %d\n", valid_data);


	return 0;
}*/