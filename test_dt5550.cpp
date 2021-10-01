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

#define SCI_REG_c 0x00010007
#define SCI_REG_Oscilloscope_0_FIFOADDRESS 0x0
#define SCI_REG_Oscilloscope_0_READ_STATUS 0x10000
#define SCI_REG_Oscilloscope_0_READ_POSITION 0x10001
#define SCI_REG_Oscilloscope_0_CONFIG_TRIGGER_MODE 0x10002
#define SCI_REG_Oscilloscope_0_CONFIG_PRETRIGGER 0x10003
#define SCI_REG_Oscilloscope_0_CONFIG_TRIGGER_LEVEL 0x10004
#define SCI_REG_Oscilloscope_0_CONFIG_ARM 0x10005
#define SCI_REG_Oscilloscope_0_CONFIG_DECIMATOR 0x10006

#define N_CH 		2
#define N_SAMPLES 	16384
//0x1000



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int OSCILLOSCOPE_RECONSTRUCT(uint32_t *data_osc, uint32_t position, int32_t pre_trigger, uint32_t *read_analog, uint32_t *read_digital0, uint32_t *read_digital1, uint32_t *read_digital2, uint32_t *read_digital3)
{
  int n_ch = N_CH;
  int n_samples = N_SAMPLES;
  if (position > n_samples) return -1;
  for(int n=0; n< n_ch; n++)
    {
      int current = position - pre_trigger;
      if (current > 0)
	{
	  int k = 0;
	  for (int i = current; i < n_samples - 1; i++)
	    {
	      read_analog[k + (n_samples*n)] = data_osc[i + (n_samples*n)] & 65535;
	      read_digital0[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 16 & 1;
	      read_digital1[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 17 & 1;
	      read_digital2[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 18 & 1;
	      read_digital3[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 19 & 1;
	      k++;
	    }
	  for (int i = 0; i < current - 1; i++)
	    {
	      read_analog[k + (n_samples*n)] = data_osc[i + (n_samples*n)] & 65535;
	      read_digital0[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 16 & 1;
	      read_digital1[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 17 & 1;
	      read_digital2[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 18 & 1;
	      read_digital3[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 19 & 1;
	      k++;
	    }
	}
      else
	{
	  int k = 0;
	  for (int i = n_samples+current; i < n_samples - 1; i++)
	    {
	      read_analog[k + (n_samples*n)] = data_osc[i + (n_samples*n)] & 65535;
	      read_digital0[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 16 & 1;
	      read_digital1[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 17 & 1;
	      read_digital2[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 18 & 1;
	      read_digital3[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 19 & 1;
	      k++;
	    }
	  for (int i = 0; i < n_samples + current - 1; i++)
	    {
	      read_analog[k + (n_samples*n)] = data_osc[i + (n_samples*n)] & 65535;
	      read_digital0[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 16 & 1;
	      read_digital1[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 17 & 1;
	      read_digital2[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 18 & 1;
	      read_digital3[k + (n_samples*n)] = data_osc[i + (n_samples*n)] >> 19 & 1;
	      k++;
	    }
	}
    }
  return 0;

}


int main()
{
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
    NI_HANDLE dt5550;

    char listofdevices[1024];
    int Count;

    uint32_t data_osc[N_SAMPLES*N_CH];
    uint32_t read_data_osc;
    uint32_t valid_data_osc;
    uint32_t status_osc = 0;
    int32_t pre_trigger = 100;
    uint32_t position = 0;
    int32_t size_osc = N_SAMPLES*N_CH;
    uint32_t read_analog[N_SAMPLES*N_CH];
    uint32_t read_digital0[N_SAMPLES*N_CH];
    uint32_t read_digital1[N_SAMPLES*N_CH];
    uint32_t read_digital2[N_SAMPLES*N_CH];
    uint32_t read_digital3[N_SAMPLES*N_CH];
    
    char filename[32];   

    uint32_t single_reg;
    int ret;




    NI_USB3_ListDevices(listofdevices, "DT5550",  &Count);

    printf("Devices found: %d\n%s\n", Count, listofdevices);


    NI_USB3_SetboardModel(DT5550, &dt5550);

    if (NI_USB3_ConnectDevice("NI110022", &dt5550)) {
        printf("Unable to connect to the board");
        exit(0);
    }


    

    ret = NI_USB3_WriteReg(0, SCI_REG_Oscilloscope_0_CONFIG_DECIMATOR, &dt5550);
    printf("DECIMATOR: %d\n",ret);
    ret = NI_USB3_WriteReg(pre_trigger, SCI_REG_Oscilloscope_0_CONFIG_PRETRIGGER,  &dt5550);
    printf("PRETRIGGER: %d\n",ret);
    

    ret = NI_USB3_WriteReg(0x82, SCI_REG_Oscilloscope_0_CONFIG_TRIGGER_MODE, &dt5550);
    printf("TRIGGER_MODE: %d\n",ret);

    ret = NI_USB3_WriteReg(100, SCI_REG_Oscilloscope_0_CONFIG_TRIGGER_LEVEL, &dt5550);
    printf("TRIGGER_LEVEL: %d\n",ret);


    for(int r=0;r<100;r++) {

	    sprintf(filename, "dump/wave_%d.txt", r);
	    FILE *fp = fopen(filename, "w");

	    ret = NI_USB3_WriteReg(0, SCI_REG_Oscilloscope_0_CONFIG_ARM, &dt5550);
	    printf("CONFIG_ARM: %d\n",ret);
	    ret = NI_USB3_WriteReg(1, SCI_REG_Oscilloscope_0_CONFIG_ARM, &dt5550);
	    printf("CONFIG_ARM: %d\n",ret);


	    // READ_STATUS [0]=1 
	    while (status_osc != 1)
	      {
		ret = NI_USB3_ReadReg(&status_osc, SCI_REG_Oscilloscope_0_READ_STATUS, &dt5550);
		printf("READ_STATUS: %d\n",ret);
		printf("status_osc   %d\n",status_osc);

		if(ret!=0) {printf("loop error\n");  break;}
	      }


	    ret = NI_USB3_ReadReg(&position, SCI_REG_Oscilloscope_0_READ_POSITION, &dt5550);
	    printf("READ_POSITION: %d\n",ret);
	    printf("position   %d\n",position);

	    ret = NI_USB3_ReadData(data_osc,size_osc,SCI_REG_Oscilloscope_0_FIFOADDRESS,REG_ACCESS,2000, &dt5550, &read_data_osc, &valid_data_osc);
	    printf("FIFOADDRESS: %d    -> DATA VALID:%d\n",ret, valid_data_osc);

	    ret = OSCILLOSCOPE_RECONSTRUCT(data_osc, position, pre_trigger, read_analog, read_digital0, read_digital1, read_digital2, read_digital3);
	    printf("RECONSTRUCT: %d\n",ret);
					   



	    printf("\n\n");


		    for (int i=0;i<N_SAMPLES;i++)
			{
				    for (int c=0;c<N_CH;c++){
					fprintf(fp, "%8d" , read_analog[i + (c*N_SAMPLES)]);
				    }
				fprintf(fp, "\n");
			}
	    
	   fclose(fp);

    }

	    NI_USB3_CloseConnection(&dt5550);
    return 0;
}
