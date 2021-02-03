#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <stdlib.h>
#include "ni_usb3_class.h"

#define REG_ACCESS 		0
#define FIFO_ACCESS 	1

int32_t ni_usb3_class::open(char * SerialNumber){
	return port.open(SerialNumber) ? 1:0;
}

int32_t ni_usb3_class::close(void){
	port.close();
	return 0;
}

int32_t ni_usb3_class::write_mem(uint32_t *data, uint32_t count, uint32_t address, int bus_mode, uint32_t timeout_ms, uint32_t *written_data){
	uint8_t acWriteBuf[255];
    uint8_t *data_pointer;
    uint32_t ulBytesWritten = 0;
    int32_t pTotalData = count;
    int32_t timeoutns = timeout_ms * 100000;
    int32_t wr_len;

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
  

    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        return 1;
    }

    data_pointer = (uint8_t *) data;
    wr_len= count*4;
    sent = port.write(data_pointer, wr_len, timeout_ms);
    *written_data = sent;
    
    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        return 2;
    }


    return 0;
}

int32_t ni_usb3_class::read_mem(uint32_t *data, uint32_t count, uint32_t address, int bus_mode, uint32_t timeout_ms, uint32_t *read_data, uint32_t *valid_data){
	uint8_t acWriteBuf[255];
    uint8_t *data_pointer;
    uint32_t ulBytesWritten = 0;
    int32_t pTotalData = count;
    int32_t timeoutns = timeout_ms * 100000;
    int32_t wr_len;

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
  

    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        return 1;
    }

    data_pointer = (uint8_t *) data;
    wr_len= (count+2)*4;
    sent = port.read(data_pointer, wr_len, timeout_ms);
 
    if (sent != wr_len)
    {
        fprintf(stderr, "ERROR: Failed to send command\n");
        return 2;
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
        return 0;
    else
        return 1;
}

int32_t ni_usb3_class::read_reg(uint32_t *data, uint32_t address){
	uint32_t read_data;
	uint32_t dummy_vector[4];
	int32_t ret = read_mem(dummy_vector, 1, address, REG_ACCESS , 1000,  NULL, &read_data);
	*data = dummy_vector[0];
	if (ret)
		return -1;

	if (read_data>0)
		return 0;
	else 
		return -1;
}

int32_t ni_usb3_class::write_reg(uint32_t data, uint32_t address){
	uint32_t written_data;
	int32_t ret = write_mem(&data, 1, address, REG_ACCESS , 1000,  &written_data);
	
	if (ret)
		return -1;

	if (written_data>0)
		return 0;
	else 
		return -1;
}

int32_t ni_usb3_class::listdevices(char *ListOfDevice, char *model,  int *count){
	return port.listdevices(ListOfDevice, model, count);	
}

