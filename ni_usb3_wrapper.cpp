#include "ni_usb3_wrapper.h"
#include "ni_usb3_class.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep(x*1000)
#endif



#define FLASH_PAGE_SIZE			512
#define FLASH_SECTOR_SIZE   0x40000

#define FIRMWARE_BL 				0
#define FIRMWARE_BA 				0x400000
#define FIRMWARE_LEN 				0x800000 - FLASH_SECTOR_SIZE
#define FIRMWARE_FIRMWARE_INFO_BA 	FIRMWARE_BA + FIRMWARE_LEN
#define FIRMWARE_FIRMWARE_INFO_LEN  FLASH_SECTOR_SIZE
#define FIRMWARE_FIRMWARE_PARAM		0xC00000
#define FIRMWARE_PARAMSIZE			0x400000

#define FLASH_CONTROLLER_BA 0xFFFE0000
#define FLASH_CONTROLLER_REG_ADDR 0xF001
#define FLASH_CONTROLLER_REG_CNTRL 0xF000
#define FLASH_CONTROLLER_REG_MAGIC 0xFFFF


//OFFSET ICAPDNA
#define RFA_ICAPDNA_BA					0xFFFA0000
#define RFA_ICAPDNA_UID1				RFA_ICAPDNA_BA + 0x04
#define RFA_ICAPDNA_UID2				RFA_ICAPDNA_BA + 0x05
#define RFA_ICAPDNA_CHECK1				RFA_ICAPDNA_BA + 0x08
#define RFA_ICAPDNA_CHECK2				RFA_ICAPDNA_BA + 0x09
#define RFA_ICAPDNA_CHECK3				RFA_ICAPDNA_BA + 0x0A
#define RFA_ICAPDNA_CHECK4				RFA_ICAPDNA_BA + 0x0B
#define RFA_ICAPDNA_CHECK5				RFA_ICAPDNA_BA + 0x0C
#define RFA_ICAPDNA_CHECK6				RFA_ICAPDNA_BA + 0x0D
#define RFA_ICAPDNA_CHECK7				RFA_ICAPDNA_BA + 0x0E
#define RFA_ICAPDNA_CHECK8				RFA_ICAPDNA_BA + 0x0F
#define RFA_ICAPDNA_WF					RFA_ICAPDNA_BA + 0x00
#define RFA_ICAPDNA_EEPROM				RFA_ICAPDNA_BA + 0x01
#define RFA_ICAPDNA_READEEPROM			RFA_ICAPDNA_BA + 0x07
#define RFA_ICAPDNA_SN					RFA_ICAPDNA_BA + 0x11
#define RFA_ICAPDNA_INFO				RFA_ICAPDNA_BA + 0x10




NIUSB3_CORE_API int NI_USB3_ConnectDevice(char *serial_number, NI_HANDLE *handle)
{

	ni_usb3_class *usbclass = new ni_usb3_class() ;
	handle->device_handle = usbclass;

    handle->connection_status = NOT_CONNECTED;
    
    if (!usbclass->open(serial_number))
        return 1;
	
	handle->connection_status = CONNECTED;
	return 0;
}

NIUSB3_CORE_API int NI_USB3_CloseConnection(NI_HANDLE *handle)
{
	if (handle->connection_status == CONNECTED)
	{
		ni_usb3_class *usbclass = (ni_usb3_class*)handle->device_handle;
		usbclass->close();
		handle->connection_status = NOT_CONNECTED;
		return 0;
	}
	else
		return 1;
}

NIUSB3_CORE_API int NI_USB3_WriteData(uint32_t *data, uint32_t count, uint32_t address, USB_BUS_MODE bus_mode, uint32_t timeout_ms, NI_HANDLE *handle, uint32_t *written_data)
{
	if (handle->connection_status != CONNECTED)
	{
		return -1;
	}
	ni_usb3_class *usbclass = (ni_usb3_class*)handle->device_handle;
	return usbclass->write_mem(data, count, address, (int) bus_mode, timeout_ms, written_data);
}


NIUSB3_CORE_API int NI_USB3_ReadData(uint32_t *data, uint32_t count, uint32_t address, USB_BUS_MODE bus_mode, uint32_t timeout_ms, NI_HANDLE *handle, uint32_t *read_data, uint32_t *valid_data)
{
	if (handle->connection_status != CONNECTED)
	{
		return -1;
	}
	ni_usb3_class *usbclass = (ni_usb3_class*)handle->device_handle;
	return usbclass->read_mem(data, count, address, (int) bus_mode, timeout_ms, read_data, valid_data);
}

NIUSB3_CORE_API int NI_USB3_WriteReg(uint32_t data, uint32_t address, NI_HANDLE *handle)
{
	if (handle->connection_status != CONNECTED)
	{
		return -1;
	}
	ni_usb3_class *usbclass = (ni_usb3_class*)handle->device_handle;
	return usbclass->write_reg(data, address);	
}

NIUSB3_CORE_API int NI_USB3_ReadReg(uint32_t *data, uint32_t address, NI_HANDLE *handle)
{
	if (handle->connection_status != CONNECTED)
	{
		return -1;
	}
	ni_usb3_class *usbclass = (ni_usb3_class*)handle->device_handle;
	return usbclass->read_reg(data, address);		
}

NIUSB3_CORE_API int NI_USB3_ListDevices(char *ListOfDevice, char *model,  int *Count)
{
	ni_usb3_class usbclass;
	usbclass.listdevices(ListOfDevice, model, Count);
}





NIUSB3_CORE_API int NI_USB3_IIC_WriteData(uint8_t address, uint8_t *value, int len, NI_HANDLE *handle)
{
	int i;
	/*
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(1<<15, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;	*/
	uint32_t data_tmp;
	if (NI_USB3_WriteReg(1 << 8, handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	if (NI_USB3_WriteReg((address << 1) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	for (i = 0; i<len; i++)
	{
		if (NI_USB3_WriteReg((value[i]) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
			return -1;
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);

	}

	if (NI_USB3_WriteReg((1 << 9), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	return 0;
}

NIUSB3_CORE_API int NI_USB3_SetIICControllerBaseAddress(uint32_t ControlAddress, uint32_t StatusAddress, NI_HANDLE *handle)
{
	handle->__IICBASEADDRESS = ControlAddress;
	handle->__IICBASEADDRESS_STATUS = StatusAddress;
	return 0;
}


NIUSB3_CORE_API int NI_USB3_IIC_ReadData(uint8_t address, uint8_t *value, int len, uint8_t *value_read, int len_read, NI_HANDLE *handle)
{
	int i;
	/*
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(1<<15, RFA_IIC_BA + 0, handle) != 0)
	return -1;
	Sleep(5);
	if (NI_USB3_WriteReg(0, RFA_IIC_BA + 0, handle) != 0)
	return -1;	*/
	uint32_t data_tmp;

	
	if (NI_USB3_WriteReg(1 << 8, handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;


	if (NI_USB3_WriteReg((address << 1) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
	
	
	do 
	{
		if (NI_USB3_ReadReg(&data_tmp, handle->__IICBASEADDRESS_STATUS, handle) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	for (i = 0; i<len; i++)
	{
		if (NI_USB3_WriteReg((value[i]) + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
			return -1;
		
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;
		} while ((data_tmp>>17)==0);


	}

	if (NI_USB3_WriteReg(1 << 8, handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;

	if (NI_USB3_WriteReg((address << 1) + 1 + (1 << 12), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;
	
	do 
	{
		if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	for (i = 0; i<len_read; i++)
	{
		if (NI_USB3_WriteReg((1 << 11) + (1 << 10), handle->__IICBASEADDRESS + 0, handle) != 0)
			return -1;
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp, handle->__IICBASEADDRESS_STATUS, handle) != 0)
				return -1;

		} while ((data_tmp>>17)==0);
		value_read[i] = data_tmp & 0xFF;
	}

	if (NI_USB3_WriteReg((1 << 9), handle->__IICBASEADDRESS + 0, handle) != 0)
		return -1;

	do 
	{
		if (NI_USB3_ReadReg(&data_tmp,handle->__IICBASEADDRESS_STATUS, handle) != 0)
			return -1;
	} while ((data_tmp>>17)==0);



	return 0;
}


NIUSB3_CORE_API int NI_USB3_IICUser_OpenController(uint32_t ControlAddress, uint32_t StatusAddress, NI_HANDLE *handle, NI_IIC_HANDLE *IIC_Handle)
{
	IIC_Handle->dev_con = handle;
	IIC_Handle->__IICBASEADDRESS = ControlAddress;
	IIC_Handle->__IICBASEADDRESS_STATUS = StatusAddress;

	return 0;
}


NIUSB3_CORE_API int NI_USB3_IICUser_ReadData(uint8_t address, uint8_t *value, int len, uint8_t *value_read, int len_read, NI_IIC_HANDLE *IIC_Handle)
{
	uint32_t data_tmp;
	int i;

	
	if (NI_USB3_WriteReg(1 << 8, IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;


	if (NI_USB3_WriteReg((address << 1) + (1 << 12), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;
	

	do 
	{
		if (NI_USB3_ReadReg(&data_tmp,IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	for (i = 0; i<len; i++)
	{
		if (NI_USB3_WriteReg((value[i]) + (1 << 12),IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
			return -1;

		do 
		{
			if (NI_USB3_ReadReg(&data_tmp, IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
				return -1;
		} while ((data_tmp>>17)==0);

	}

	if (NI_USB3_WriteReg(1 << 8, IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;

	if (NI_USB3_WriteReg((address << 1) + 1 + (1 << 12), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;
	
	do 
	{
		if (NI_USB3_ReadReg(&data_tmp, IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	for (i = 0; i<len_read; i++)
	{
		if (NI_USB3_WriteReg((1 << 11) + (1 << 10), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
			return -1;
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp, IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
				return -1;

		} while ((data_tmp>>17)==0);
		value_read[i] = data_tmp & 0xFF;
	}

	if (NI_USB3_WriteReg((1 << 9), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;

	do 
	{
		if (NI_USB3_ReadReg(&data_tmp, IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
			return -1;
	} while ((data_tmp>>17)==0);



	return 0;

}

NIUSB3_CORE_API int NI_USB3_IICUser_WriteData(uint8_t address, uint8_t *value, int len, NI_IIC_HANDLE *IIC_Handle)
{
	int i;
	uint32_t data_tmp;
	
	if (NI_USB3_WriteReg(1 << 8, IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;
	do 
	{
		if (NI_USB3_ReadReg(&data_tmp, IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
			return -1;
	} while ((data_tmp>>17)==0);


	if (NI_USB3_WriteReg((address << 1) + (1 << 12), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;
	

	do 
	{
		if (NI_USB3_ReadReg(&data_tmp,IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
			return -1;
	} while ((data_tmp>>17)==0);

	for (i = 0; i<len; i++)
	{
		if (NI_USB3_WriteReg((value[i]) + (1 << 12), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
			return -1;
		
		do 
		{
			if (NI_USB3_ReadReg(&data_tmp, IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
				return -1;
		} while ((data_tmp>>17)==0);
	}

	if (NI_USB3_WriteReg((1 << 9), IIC_Handle->__IICBASEADDRESS, IIC_Handle->dev_con) != 0)
		return -1;

	do 
	{
		if (NI_USB3_ReadReg(&data_tmp,IIC_Handle->__IICBASEADDRESS_STATUS, IIC_Handle->dev_con) != 0)
			return -1;
	} while ((data_tmp>>17)==0);
}


NIUSB3_CORE_API int NI_USB3_GetDT5550_DGBoardInfo(char *model, int *asic_count, int *SN, NI_HANDLE *handle)
{
	int i;
	uint8_t vv[16];
	vv[0] = 0;
	NI_USB3_IIC_ReadData(0x54, vv, 1,vv,16, handle);

	for(i=0;i<3;i++)
		model[i] = vv[i+4];

	*asic_count = vv[7];
	*SN = vv[8]+(vv[9]<<8)+(vv[10]<<16)+(vv[11]<<24);
	model[3]='\0';
	vv[4]='\0';
	if (strcmp((char *)vv,"NI5W") == 0)
		return 0;
	else
		return 1;
}


NIUSB3_CORE_API int NI_USB3_SetDT5550_DGBoardInfo(char *model, int asic_count, int SN, NI_HANDLE *handle)
{
	int i;
	uint8_t vv[16];
	uint8_t vv2[2];
	
	vv[0] = 'N';
	vv[1] = 'I';
	vv[2] = '5';
	vv[3] = 'W';

	vv[4] = model[0];
	vv[5] = model[1];
	vv[6] = model[2];

	vv[7] = asic_count;

	vv[8] =  (SN>>0) & 0xFF;
	vv[9] = (SN>>8) & 0xFF;
	vv[10] = (SN>>16) & 0xFF;
	vv[11] = (SN>>24) & 0xFF;

	for (i =0;i<16;i++)
	{
		vv2[0]= i;
		vv2[1]= vv[i];
		NI_USB3_IIC_WriteData(0x54, vv2,2, handle);
		Sleep(15);
	}

	return 0;
}

NIUSB3_CORE_API int NI_USB3_SetHV(bool Enable, float voltage, NI_HANDLE *handle)
{
	uint8_t vv[16];

	vv[0] = 1;
	vv[1] = 2;
	vv[2] = 0;
	vv[3] = 0;
	vv[4] = 0;
	vv[5] = 0;
	NI_USB3_IIC_WriteData(0x73, vv, 6, handle);

	vv[0] = 6;
	vv[1] = 2;
	vv[2] = 10;
	vv[3] = 0;
	vv[4] = 0;
	vv[5] = 0;
	NI_USB3_IIC_WriteData(0x73, vv, 6, handle);

	vv[0] = 2;
	vv[1] = 3;
	vv[2] = 0;
	vv[3] = 0;
	vv[4] = 0;
	vv[5] = 0;
	memcpy(&vv[2], &voltage, 4);
	NI_USB3_IIC_WriteData(0x73, vv, 6, handle);

	vv[0] = 0;
	vv[1] = 2;
	vv[2] = Enable == true ? 1 : 0;
	vv[3] = 0;
	vv[4] = 0;
	vv[5] = 0;
	NI_USB3_IIC_WriteData(0x73, vv, 6, handle);

}


NIUSB3_CORE_API int NI_USB3_GetHV(bool *Enable, float *voltage, float *current, NI_HANDLE *handle)
{
	uint8_t vv[16];
	uint32_t *data;
	data=(uint32_t*)vv;
	vv[0] = 0;
	vv[1] = 0;
	NI_USB3_IIC_ReadData(0x73, vv, 2, vv, 4, handle);
	*Enable  =  vv[0] > 0 ? true: false;
	vv[0] = 231;
	vv[1] = 1;
	NI_USB3_IIC_ReadData(0x73, vv, 2, vv, 4, handle);

	*voltage  =( (float) *data )/10000.0;

	vv[0] = 232;
	vv[1] = 1;
	NI_USB3_IIC_ReadData(0x73, vv, 2, vv, 4, handle);
	*current  =( (float) *data )/10000.0;
	if (*current>10000) *current = 0;

}



NIUSB3_CORE_API int NI_USB3_GetDT5550WTempSens(int address, float *temp, NI_HANDLE *handle)
{
	uint8_t vv[16];
	uint32_t *data;
	data=(uint32_t*)vv;
	vv[0] = 0;
	NI_USB3_IIC_ReadData(0x48 + address, vv, 1, vv, 2, handle);
	int32_t value;
	value = (vv[0] << 4) + ((vv[1]>>4) & 0xF);
	if (value > 0x800) value = -(0xFFF - value);

	*temp = ((float) value)/2048.0 * 128.0;
}

NIUSB3_CORE_API int NI_USB3_SetOffset(bool top, uint32_t DACCODE, NI_HANDLE *handle)
{
	uint8_t vv[16];
	uint8_t address;
	uint32_t voltageI = DACCODE;
	if (top == true) address = 0x61; else address = 0x60;
	vv[0] = (voltageI >> 8) & 0xFF;
	vv[1] = (voltageI >> 0) & 0xFF;
	return NI_USB3_IIC_WriteData(address, vv, 2, handle);

}


NIUSB3_CORE_API int NI_USB3_SetImpedance(bool R50, NI_HANDLE *handle)
{
	uint8_t vv[16];

	vv[0] = 0x03;
	vv[1] = 0x00;
	NI_USB3_IIC_WriteData(0x18, vv, 2, handle);

	vv[0] = 0x01;
	if (R50 == false)
		vv[1] = 0x12;
	else
		vv[1] = 0x00;

	NI_USB3_IIC_WriteData(0x18, vv, 2, handle);

}



NIUSB3_CORE_API char *ReadFirmwareInformationApp(	NI_HANDLE *handle)
{
		uint32_t datarow[512];
	uint32_t dataread[512];
	uint32_t temp =0; 
	uint32_t rw;
	uint32_t vd;
	uint32_t bytes;
	uint32_t q;
	
	int i;
	char *TEXT = (char*) malloc(0x40040 * sizeof(uint32_t));

	NI_USB3_ReadReg(&temp,FLASH_CONTROLLER_BA + FLASH_CONTROLLER_REG_MAGIC,handle);
	if (temp != 0x1234ABBA)
		return NULL;

	
	memset(TEXT,0,0x40000);
	for (i=0;i<0x40000;i+=256)
	{

		NI_USB3_WriteReg(FIRMWARE_FIRMWARE_INFO_BA+ i,FLASH_CONTROLLER_BA+FLASH_CONTROLLER_REG_ADDR,handle);
		NI_USB3_WriteReg(5,FLASH_CONTROLLER_BA+FLASH_CONTROLLER_REG_CNTRL,handle);
		do
		{
			NI_USB3_ReadReg(&temp,FLASH_CONTROLLER_BA+FLASH_CONTROLLER_REG_CNTRL,handle);
		}while(temp!=0);
		NI_USB3_ReadData((uint32_t*)(TEXT+i), 64, FLASH_CONTROLLER_BA, REG_ACCESS, 1000, handle, &rw, &vd);
		if (TEXT[i+255] == '\0')
			break;
	}

	return TEXT;

}




/*
	Write Security EEPROM
*/

NIUSB3_CORE_API int SECWriteWord(unsigned int address, unsigned int word, NI_HANDLE *handle)
{
	
	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF , handle) != 0)
		return -1;

	if(NI_USB3_WriteReg(1 << 24, RFA_ICAPDNA_EEPROM, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(1, RFA_ICAPDNA_WF, handle) != 0)
		return -1;

	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle) != 0)
		return -1;
	Sleep (10);
	if(NI_USB3_WriteReg((address << 16) + (2 << 24), RFA_ICAPDNA_EEPROM,handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(1, RFA_ICAPDNA_WF, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle ) != 0)
		return -1;

	Sleep (10);

	if(NI_USB3_WriteReg(word + (address << 16) + (4 << 24), RFA_ICAPDNA_EEPROM, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(1, RFA_ICAPDNA_WF, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle ) != 0)
		return -1;

	Sleep (10);
	
	return 0;
}




/*
	Read Security EEPROM
*/

NIUSB3_CORE_API int SECReadWord(unsigned int address, unsigned int *word, NI_HANDLE *handle)
{
	unsigned int w;
	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle) != 0)
		return -1;

	if(NI_USB3_WriteReg((address << 16) + (8 << 24), RFA_ICAPDNA_EEPROM, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(1, RFA_ICAPDNA_WF, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle ) != 0)
		return -1;

	Sleep (10);

	if(NI_USB3_ReadReg(&w, RFA_ICAPDNA_READEEPROM, handle ) != 0)
		return -1;

	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle) != 0)
		return -1;

	if(NI_USB3_WriteReg(1, RFA_ICAPDNA_WF, handle) != 0)
		return -1;

	if(NI_USB3_WriteReg(0, RFA_ICAPDNA_WF, handle) != 0)
		return -1;

	*word = w & 0xFFFF;
	
	return 0;
}



NIUSB3_CORE_API int SECReadUIDSN(uint64_t *UID, uint32_t *SN, NI_HANDLE *handle)
{
	unsigned int TTT[20];
	unsigned int v;
	uint32_t read_data;
	uint32_t valid_data;

	SECReadWord(16,&v, handle);

	NI_USB3_ReadData(TTT, 18, RFA_ICAPDNA_BA, REG_ACCESS , 100, handle, &read_data, &valid_data);


	*UID = (((uint64_t)TTT[5]) << 32) + ((uint64_t)TTT[4]); 
	*SN = v;
	return 0;
}



NIUSB3_CORE_API int SECWriteSN(unsigned int SN, NI_HANDLE *handle)
{
	return SECWriteWord(16,SN, handle);
	
}


NIUSB3_CORE_API int SECWritekey(int *key, int length, NI_HANDLE *handle)
{
	int i;
	if (length > 16)
		return -1;
	for (i=0;i<length-1;i+=2)
	{
		SECWriteWord(i,key[i+1],handle);
		SECWriteWord(i+1,key[i],handle);
		Sleep(5);
	}
	return 0;
}


NIUSB3_CORE_API int SECReadActivationStatus(unsigned int *active, unsigned int *trial_counter, unsigned int *trial_expired, NI_HANDLE *handle)
{
	unsigned int TTT[20];
	uint32_t read_data;
	uint32_t valid_data;

	NI_USB3_ReadData(TTT, 18, RFA_ICAPDNA_BA, REG_ACCESS , 100, handle, &read_data, &valid_data);


	*active = TTT[3] & 0x01;
	*trial_counter = (TTT[3] >> 3) & 0xFFF;
	*trial_expired = !( (TTT[3] >> 2) & 0x01);
	return 0;
}

