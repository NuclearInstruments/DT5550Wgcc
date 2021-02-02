#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <stdlib.h>
#include "ni_usb3_class.h"

ni_usb3_class dt5550w;
/*
#include "ftdi_ft60x.h"


ftdi_ft60x port;

bool write_mem(uint32_t *data, uint32_t count, uint32_t address, int bus_mode, uint32_t timeout_ms, uint32_t *written_data)
{

    uint8_t acWriteBuf[255];
    uint8_t *data_pointer;
    uint32_t ulBytesWritten = 0;
    int32_t pTotalData = count;
    int32_t timeoutns = timeout_ms * 100000;
    int32_t wr_len;
    bool ok = true;

    acWriteBuf[0] = 0xFF;   acWriteBuf[1] = 0x00;   acWriteBuf[2] = 0xAB;   acWriteBuf[3] = (0xF0 + (bus_mode << 1));
    acWriteBuf[4] = 0xCD;   acWriteBuf[5] = 0xAB;   acWriteBuf[6] = 0xCD;   acWriteBuf[7] = 0xAB;
    acWriteBuf[8] = 0xF1;   acWriteBuf[9] = 0xCA;   acWriteBuf[10] = 0x60;  acWriteBuf[11] = 0x0D;

    //timeout
    acWriteBuf[12] = ((timeoutns >> 24)&0xFF);; acWriteBuf[13] = ((timeoutns >> 16)&0xFF);; acWriteBuf[14] = ((timeoutns >> 8)&0xFF);;  acWriteBuf[15] = ((timeoutns)&0xFF);;

    //address
    acWriteBuf[16] = ((address >> 24)&0xFF);    acWriteBuf[17] = ((address >> 16)&0xFF);    acWriteBuf[18] = ((address >> 8)&0xFF); acWriteBuf[19] = ((address)&0xFF);

    //patload len
    acWriteBuf[20] = ((pTotalData >> 24)&0xFF); acWriteBuf[21] = ((pTotalData >> 16)&0xFF); acWriteBuf[22] = ((pTotalData >> 8)&0xFF);  acWriteBuf[23] = ((pTotalData)&0xFF);

   

    wr_len = 24;
    int sent = port.write(acWriteBuf, wr_len, timeout_ms);
  

    ok = true;
    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        ok = false;
        return ok;
    }

    data_pointer = (uint8_t *) data;
    wr_len= count*4;
    sent = port.write(data_pointer, wr_len, timeout_ms);
    *written_data = sent;
    ok = true;
    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        ok = false;
        return ok;
    }


    return ok;
}

bool read_mem(uint32_t *data, uint32_t count, uint32_t address, int bus_mode, uint32_t timeout_ms, uint32_t *read_data, uint32_t *valid_data)
{
   uint8_t acWriteBuf[255];
    uint8_t *data_pointer;
    uint32_t ulBytesWritten = 0;
    int32_t pTotalData = count;
    int32_t timeoutns = timeout_ms * 100000;
    int32_t wr_len;
    bool ok = true;

 
    //header
    acWriteBuf[0] = 0xFF;   acWriteBuf[1] = 0x00;   acWriteBuf[2] = 0xAB;   acWriteBuf[3] = (0xF1 + (bus_mode << 1));
    acWriteBuf[4] = 0xCD;   acWriteBuf[5] = 0xAB;   acWriteBuf[6] = 0xCD;   acWriteBuf[7] = 0xAB;
    acWriteBuf[8] = 0xF1;   acWriteBuf[9] = 0xCA;   acWriteBuf[10] = 0x60;  acWriteBuf[11] = 0x0D;

    //timeout
    acWriteBuf[12] = ((timeoutns >> 24)&0xFF);; acWriteBuf[13] = ((timeoutns >> 16)&0xFF);; acWriteBuf[14] = ((timeoutns >> 8)&0xFF);;  acWriteBuf[15] = ((timeoutns)&0xFF);;

    //address
    acWriteBuf[16] = ((address >> 24)&0xFF);    acWriteBuf[17] = ((address >> 16)&0xFF);    acWriteBuf[18] = ((address >> 8)&0xFF); acWriteBuf[19] = ((address)&0xFF);

    //patload len
    acWriteBuf[20] = ((pTotalData >> 24)&0xFF); acWriteBuf[21] = ((pTotalData >> 16)&0xFF); acWriteBuf[22] = ((pTotalData >> 8)&0xFF);  acWriteBuf[23] = ((pTotalData)&0xFF);


    wr_len = 24;
    int sent = port.write(acWriteBuf, wr_len, timeout_ms);
  

    ok = true;
    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        ok = false;
        return ok;
    }

    data_pointer = (uint8_t *) data;
    wr_len= (count+2)*4;
    sent = port.read(data_pointer, wr_len, timeout_ms);
  
    ok = true;
    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        ok = false;
        return ok;
    }

    uint32_t  *preal;
    uint32_t  *keyword;
    keyword = (uint32_t *) &data_pointer[count*4];
    preal = (uint32_t *) &data_pointer[count*4+4];
    if (*preal == 0xF1CA600D)
        preal = (uint32_t *) &data_pointer[count*4+8];

    if (read_data!=NULL)  *read_data = sent;
    if (valid_data!=NULL) *valid_data = *preal;
    
    if (*keyword == 0xF1CA600D)
        return true;
    else
        return false;

}*/

/*bool NI_USB3_ReadReg(uint32_t *data, uint32_t address){
    uint32_t read_data;
    uint32_t dummy_vector[4];
    read_mem(dummy_vector, 1, address, 0, 1000, &read_data, &valid_data);
}*/

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
}