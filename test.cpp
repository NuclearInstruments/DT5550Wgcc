#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>

#include <stdlib.h>
#include "ni_usb3_wrapper.h"

int main(){
    uint32_t data[1024*32];
    uint32_t datar[2048*32];
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
    char listofdevices[1024];
    int Count;
    NI_USB3_ListDevices(listofdevices, "DT5550W",  &Count);

    printf("Devices found: %d\n%s\n", Count, listofdevices);
    
    
	
    NI_USB3_SetboardModel(DT5550W, &dt5550w);

    
    if (NI_USB3_ConnectDevice("NI120010", &dt5550w)) {
        printf("Unable to connect to the board\n");
        exit(0);
    }
    
    
    
     
    printf("Connected to hardware\n");
    
    NI_USB3_GetDT5550_DGBoardInfo(board_model, &asic_count, &SN, &dt5550w);
    printf("Board model: %s\n",board_model);
    printf("Asic Count:  %d\n",asic_count);
    printf("SN:          %d\n",SN);   
    
    
    printf("Memory access reading. Running oscilloscope in free running .... ");
    uint32_t mydata[1200];
    NI_USB3_WriteReg(0, 0x805, &dt5550w);
    NI_USB3_WriteReg(0x80, 0x802, &dt5550w);
    NI_USB3_WriteReg(1, 0x805, &dt5550w);
    usleep(100000);
    NI_USB3_ReadData(mydata, 1000, 0x400, REG_ACCESS, 1000, &dt5550w, &read_data, &valid_data);
    NI_USB3_WriteReg(0, 0x805, &dt5550w);
    for (int i=0; i<valid_data; i++){
      printf("%d ",mydata[i]);
      if (i%8==0) printf("\n");
    }
      
    
    uint32_t i = 0;
    
    printf("Register access test .... ");
    for (int i=0; i<100; i++){
       uint32_t xxx;
       NI_USB3_WriteReg(i++, 1, &dt5550w);
       NI_USB3_ReadReg(&xxx, 0,  &dt5550w);      
       printf("written: %8d  read: %8d\n",i, xxx);
    }

    NI_USB3_SetHV(true, 22, &dt5550w);

	while (1){
    NI_USB3_GetHV(&Enable, &voltage, &current, &dt5550w);
    printf("HV enable:   %d\n",Enable);
    printf("HV voltage:  %f\n",voltage);
    printf("HV current:  %f\n",current);

    NI_USB3_GetDT5550WTempSens(0, &temp, &dt5550w);
    printf("Temp[0]:     %f\n",temp);
    NI_USB3_GetDT5550WTempSens(1, &temp, &dt5550w);
    printf("Temp[1]:     %f\n",temp);
	  usleep(1000000);
	}
/*
    for (int i =0; i<1000;i++){
        for (int i=0;i<1024;i++)
            	data[i] = i;

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
    */
}
