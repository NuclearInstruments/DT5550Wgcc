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
    NI_USB3_SetHV(true, 30, &dt5550w);
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
	

    for (int i =0; i<10000;i++){
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