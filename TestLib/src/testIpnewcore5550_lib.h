#include "RegisterFile.h"
#include "ni_usb3_wrapper.h"
#include "Def.h"
#include <stdint.h>
#include <stdlib.h>
#include  <stdbool.h>

#ifndef NI_SCILIB_H
#define NI_SCILIB_H


#if defined(_WIN32) || defined(_WIN64)

    #ifdef SCICOMPILER_EXPORTS
        #define SCILIB __declspec(dllexport) 
    #else
        #define SCILIB __declspec(dllimport)
    #endif
#else
    #ifdef SCICOMPILER_EXPORTS
        #define SCILIB __attribute__((visibility("default")))
    #else
        #define SCILIB
    #endif
#endif


#ifdef __cplusplus
	extern "C" {
#endif


typedef struct
{
	uint64_t Time_Code;
	uint32_t Id;
	uint32_t Pack_Id;
	uint32_t *Energy;
	uint32_t Valid;
} t_FRAME_packet;


typedef struct
{
	t_FRAME_packet *packets;
	int allocated_packets;
	int valid_packets;
} t_FRAME_packet_collection;




SCILIB int USB3_ListDevices(char *ListOfDevice, char *model,  int *Count);
SCILIB int USB3_ConnectDevice(char *serial_number, NI_HANDLE *handle);
SCILIB int USB3_CloseConnection(NI_HANDLE *handle);
SCILIB int USB3_Init();

SCILIB int __abstracted_mem_write(uint32_t *data, uint32_t count, 
										uint32_t address,  
										uint32_t timeout_ms, NI_HANDLE *handle, 
										uint32_t *written_data);
SCILIB int __abstracted_mem_read(uint32_t *data, uint32_t count, 
										uint32_t address, 
										uint32_t timeout_ms, NI_HANDLE *handle, 
										uint32_t *read_data, uint32_t *valid_data);
SCILIB int __abstracted_fifo_write(uint32_t *data, uint32_t count, 
										uint32_t address,  
										uint32_t timeout_ms, NI_HANDLE *handle, 
										uint32_t *written_data);
SCILIB int __abstracted_fifo_read(uint32_t *data, uint32_t count, 
										uint32_t address, 
										uint32_t address_status, 
										bool blocking,
										uint32_t timeout_ms, NI_HANDLE *handle, 
										uint32_t *read_data, uint32_t *valid_data);
SCILIB int __abstracted_reg_write(uint32_t data, uint32_t address, NI_HANDLE *handle);
SCILIB int __abstracted_reg_read(uint32_t *data, uint32_t address, NI_HANDLE *handle);
SCILIB int AFE_USB3_SetOffset(bool top, uint32_t DACCODE, NI_HANDLE *handle);
SCILIB int AFE_USB3_SetImpedance(bool R50, NI_HANDLE *handle);
SCILIB int AFE_USB3_SetIICControllerBaseAddress(uint32_t ControlAddress, uint32_t StatusAddress, NI_HANDLE *handle);

SCILIB int Utility_PEAK_DATA_FORM_DOWNLOAD_BUFFER(void *buffer_handle, int32_t *val);

SCILIB int Utility_ENQUEUE_DATA_IN_DOWNLOAD_BUFFER(void *buffer_handle, int32_t *val, uint32_t size, uint32_t *enqueued_data);

SCILIB int Utility_ALLOCATE_DOWNLOAD_BUFFER(void **buffer_handle, uint32_t buffer_size);

SCILIB void free_FRAME_packet_collectionvoid(t_FRAME_packet_collection *decoded_packets);
SCILIB void free_PETIROCFRAME_packet_collection(t_ASIC_packet_collection *decoded_packets);

SCILIB int IICUser_OpenControllerInit(uint32_t ControlAddress, uint32_t StatusAddress, NI_HANDLE *handle, NI_IIC_HANDLE *IIC_Handle);
SCILIB int IICUser_ReadData(uint8_t address, uint8_t *value, int len, uint8_t *value_read, int len_read, NI_IIC_HANDLE *IIC_Handle);
SCILIB int IICUser_WriteData(uint8_t address, uint8_t *value, int len, NI_IIC_HANDLE *IIC_Handle);




#ifdef __cplusplus
}
#endif


#endif