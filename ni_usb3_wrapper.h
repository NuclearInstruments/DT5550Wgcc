// Il seguente blocco ifdef rappresenta il modo standard di creare macro che semplificano 
// l'esportazione da una DLL. Tutti i file all'interno della DLL sono compilati con il simbolo NIUSB3_CORE_EXPORTS
// definito nella riga di comando. Questo simbolo non deve essere definito in alcun progetto
// che utilizza questa DLL. In questo modo qualsiasi altro progetto i cui file di origine includono questo file vedranno le funzioni 
// NIUSB3_CORE_API come importate da una DLL, mentre la DLL vedrà i simboli
// definiti con questa macro come esportati.
#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifdef SCIDK_API_EXPORTS
        #define NIUSB3_CORE_API extern "C" __declspec(dllexport) 
    #else
        #define NIUSB3_CORE_API __declspec(dllimport)
    #endif
#else
    #ifdef SCIDK_API_EXPORTS
        #define NIUSB3_CORE_API __attribute__((visibility("default")))
    #else
        #define NIUSB3_CORE_API
    #endif
#endif


#include <stdlib.h>
#include <stdint.h>





enum USB_CONNECTION_STATUS
{
	NOT_CONNECTED = 0,
	CONNECTED = 1
} ;


typedef struct NI_HANDLE
{
	void  *device_handle;
	USB_CONNECTION_STATUS connection_status;
} NI_HANDLE;


typedef struct NI_IIC_HANDLE
{
	NI_HANDLE *dev_con;
	uint32_t __IICBASEADDRESS;
	uint32_t __IICBASEADDRESS_STATUS;
} NI_IIC_HANDLE;


enum USB_BUS_MODE
{
	REG_ACCESS = 0,
	STREAMING = 1
} ;

NIUSB3_CORE_API int fnniusb3_core(void);
NIUSB3_CORE_API int NI_USB3_ListDevices(char *ListOfDevice, char *model,  int *Count);
NIUSB3_CORE_API int NI_USB3_ConnectDevice(char *serial_number, NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_CloseConnection(NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_Init();

NIUSB3_CORE_API int NI_USB3_WriteData(uint32_t *data, uint32_t count, 
										uint32_t address, USB_BUS_MODE bus_mode, 
										uint32_t timeout_ms, NI_HANDLE *handle, 
										uint32_t *written_data);
NIUSB3_CORE_API int NI_USB3_ReadData(uint32_t *data, uint32_t count, 
										uint32_t address, USB_BUS_MODE bus_mode, 
										uint32_t timeout_ms, NI_HANDLE *handle, 
										uint32_t *read_data, uint32_t *valid_data);
NIUSB3_CORE_API int NI_USB3_WriteReg(uint32_t data, uint32_t address, NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_ReadReg(uint32_t *data, uint32_t address, NI_HANDLE *handle);


NIUSB3_CORE_API int NI_USB3_SetHV(bool Enable, float voltage, NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_GetHV(bool *Enable, float *voltage, float *current, NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_SetOffset(bool top, uint32_t DACCODE, NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_SetImpedance(bool R50, NI_HANDLE *handle);

NIUSB3_CORE_API char *ReadFirmwareInformationApp(	NI_HANDLE *handle);


NIUSB3_CORE_API int SECReadActivationStatus(unsigned int *active, unsigned int *trial_counter, unsigned int *trial_expired, NI_HANDLE *handle);
NIUSB3_CORE_API int SECWritekey(int *key, int length, NI_HANDLE *handle);
NIUSB3_CORE_API int SECWriteSN(unsigned int SN, NI_HANDLE *handle);
NIUSB3_CORE_API int SECReadUIDSN(uint64_t *UID, uint32_t *SN, NI_HANDLE *handle);

NIUSB3_CORE_API int NI_USB3_IICUser_OpenController(uint32_t ControlAddress, uint32_t StatusAddress, NI_HANDLE *handle, NI_IIC_HANDLE *IIC_Handle);
NIUSB3_CORE_API int NI_USB3_IICUser_ReadData(uint8_t address, uint8_t *value, int len, uint8_t *value_read, int len_read, NI_IIC_HANDLE *IIC_Handle);
NIUSB3_CORE_API int NI_USB3_IICUser_WriteData(uint8_t address, uint8_t *value, int len, NI_IIC_HANDLE *IIC_Handle);

NIUSB3_CORE_API int NI_USB3_SetIICControllerBaseAddress(uint32_t ControlAddress, uint32_t StatusAddress, NI_HANDLE *handle);

NIUSB3_CORE_API int NI_USB3_GetDT5550_DGBoardInfo(char *model, int *asic_count, int *SN, NI_HANDLE *handle);
NIUSB3_CORE_API int NI_USB3_SetDT5550_DGBoardInfo(char *model, int asic_count, int SN, NI_HANDLE *handle);

NIUSB3_CORE_API int NI_USB3_GetDT5550WTempSens(int address, float *temp, NI_HANDLE *handle);


