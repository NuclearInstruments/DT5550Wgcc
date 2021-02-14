#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <chrono>
#include <thread>

#else
#include <unistd.h>

#endif


#include <assert.h>

#include <stdlib.h>

#include "ftdi_ft60x.h"
#include "ftd3xx.h"

//-----------------------------------------------------------------------------
// configure: Check and update device configuration
//-----------------------------------------------------------------------------
bool ftdi_ft60x::configure(char *device_str, uint8_t clock)
{
    FT_HANDLE handle = NULL;

#if defined(_WIN32) || defined(_WIN64)
    DWORD numDevs = 0;
    FT_CreateDeviceInfoList(&numDevs);
#endif

    // Open connection to device
    // TODO: Device index not working...
    //assert(device_idx == 0);
    FT_Create(device_str, FT_OPEN_BY_SERIAL_NUMBER, &handle);
    if (!handle)
        return false;

    // Check device type is supported
    DWORD dwType = FT_DEVICE_UNKNOWN;
    FT_GetDeviceInfoDetail(0, NULL, &dwType, NULL, NULL, NULL, NULL, NULL);
    if (dwType != FT_DEVICE_600 && dwType != FT_DEVICE_601)
    {
        fprintf(stderr, "FT60X: Incompatible device detected\n");
        return false;
    }

    // Avoid rev-a parts - too many errata to workaround
    DWORD dwVersion;
    FT_GetFirmwareVersion(handle, &dwVersion);
    if (dwVersion <= 0x105)
    {
        fprintf(stderr, "FT60X: Incompatible device (rev-A) detected\n");
        return false;
    }

    // Get current configuration
    FT_60XCONFIGURATION current_cfg;
    if (FT_OK != FT_GetChipConfiguration(handle, &current_cfg))
    {
        fprintf(stderr, "FT60X: Could not fetch current configuration\n");
        return false;
    }

    // Create new configuration
    FT_60XCONFIGURATION new_cfg;
    memcpy(&new_cfg, &current_cfg, sizeof(FT_60XCONFIGURATION));
    new_cfg.FIFOClock     = clock;
    new_cfg.FIFOMode      = CONFIGURATION_FIFO_MODE_245;
    new_cfg.ChannelConfig = CONFIGURATION_CHANNEL_CONFIG_1;

    // Disable stop on underrun
    new_cfg.OptionalFeatureSupport = CONFIGURATION_OPTIONAL_FEATURE_DISABLECANCELSESSIONUNDERRUN;

    // Detect delta in configuration and apply if there is
    if (memcmp(&new_cfg, &current_cfg, sizeof(FT_60XCONFIGURATION)))
    {
        if (FT_SetChipConfiguration(handle, &new_cfg) != FT_OK)
        {
            fprintf(stderr, "FT60X: Could not write new configuration\n");
            return false;
        }
        else
        {
            printf("FT60x: Configuration updated...\n");
            ftdi_ft60x::sleep(1000000);
        }
    }

    FT_Close(handle);

#if !defined(_WIN32) && !defined(_WIN64)
    // Enable non thread safe transfer to increase throughput
    {
        FT_TRANSFER_CONF conf;

        memset(&conf, 0, sizeof(FT_TRANSFER_CONF));
        conf.wStructSize = sizeof(FT_TRANSFER_CONF);
        conf.pipe[FT_PIPE_DIR_IN].fNonThreadSafeTransfer = true;
        conf.pipe[FT_PIPE_DIR_OUT].fNonThreadSafeTransfer = true;
        for (DWORD i = 0; i < 4; i++)
            FT_SetTransferParams(&conf, i);
    }
#endif

    return true;
}
//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
ftdi_ft60x::ftdi_ft60x()
{
    m_handle = NULL;
}
//-------------------------------------------------------------
// open: Try and open FT60x interface and configure
//-------------------------------------------------------------
bool ftdi_ft60x::open(char *device_str)
{


    // Make sure device is configured as expected
    if (!configure(device_str, CONFIGURATION_FIFO_CLK_100))
    {
        printf("FT60x: Failed to configure device\n");
        return false;
    }

   // ULONG arg = device_idx; // TODO: Not working
   // assert(device_idx == 0);

    // Create device handle
    FT_Create(device_str, FT_OPEN_BY_SERIAL_NUMBER, &m_handle);
    if (!m_handle)
    {
        printf("FT60x: Failed to create device\n");
        return false;
    }

    FT_EnableGPIO(m_handle, 0x3, 0x3);

	ftdi_ft60x::sleep(1000);
    FT_WriteGPIO(m_handle, 0x3, 0x3);
	ftdi_ft60x::sleep(1000);
	FT_WriteGPIO(m_handle, 0x3, 0x0);

    return true;
}
//-------------------------------------------------------------
// close: Close connection to FT60x
//-------------------------------------------------------------
void ftdi_ft60x::close(void)
{
    if (m_handle != NULL)
    {
        FT_Close(m_handle);
    }
}
//-------------------------------------------------------------
// read: Read a chunk of data
//-------------------------------------------------------------
int ftdi_ft60x::read(uint8_t *data, int length, int timeout_ms)
{
    DWORD count;
    FT_STATUS err;

#if !defined(_WIN32) && !defined(_WIN64) // Linux / MAC
    if ((err = FT_ReadPipeEx(m_handle, 0, data, length, &count, timeout_ms)) != FT_OK)
	{
		if (err == FT_TIMEOUT)
			return count;

		printf("FT60x: FT_ReadPipeEx err %d\n", err);
		return -1;
	}
#else // Windows
	FT_STATUS status;
	DWORD rest;
	DWORD partial;
	err = FT_SetPipeTimeout(
		m_handle,
		0x82,
		timeout_ms * 1.5
	);
	rest = length;
	count = 0;
	while (rest > 0) {
		partial = 0;
		if ((err = FT_ReadPipe(m_handle, 0x82, &data[count], rest, &partial, NULL)) != FT_OK)
		{
			if (err == FT_TIMEOUT)
				return count;

			printf("FT60x: FT_ReadPipeEx err %d\n", err);
			return -1;
		}
		rest -= partial;
		count += partial;
	}
#endif


    return (int)count;
}
//-------------------------------------------------------------
// write: Write a chunk of data
//-------------------------------------------------------------
int ftdi_ft60x::write(uint8_t *data, int length, int timeout_ms)
{
    DWORD count;

#if !defined(_WIN32) && !defined(_WIN64) // Linux / MAC
    FT_STATUS status = FT_WritePipeEx(m_handle, 0, data, length, &count, timeout_ms);
#else // Windows
	FT_STATUS status;
	status = FT_SetPipeTimeout(
		m_handle,
		0x02,
		timeout_ms * 1.5
	);
    status = FT_WritePipe(m_handle, 0x02, data, length, &count, NULL);
#endif
    if (status  != FT_OK)
    {
        printf("FT_WritePipeEx: %d\n", status);
        return -1;
    }

#if !defined(_WIN32) && !defined(_WIN64) // Linux / MAC
    DWORD queued_data = 0;
    int loops = 0;
    do
    {
        status = FT_GetWriteQueueStatus(m_handle, 0, &queued_data);
    }
    while (queued_data != 0);
#endif
    if ((int)count != length)
        return -1;

    return (int)count;
}


int ftdi_ft60x::listdevices(char *ListOfDevice, char *model,  int *count)
{
	FT_STATUS ftStatus; 
	FT_DEVICE_LIST_INFO_NODE *devInfo; 
	DWORD numDevs; 

	// 
	// create the device information list 
	// 
	ftStatus = FT_CreateDeviceInfoList(&numDevs); 
	if (ftStatus != FT_OK) { 
	   return -1;
	} 

	// 
	// allocate storage for list based on numDevs 
	// 
	devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs); 
	ListOfDevice[0] = '\0';
	// 
	// get the device information list 
	// 
	*count = 0;
	ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs); 
	if (ftStatus == FT_OK) { 
	for (int i = 0; i < numDevs; i++) {  
		bool valid=true;
		
		if (model!= NULL)
			if (strcmp(devInfo[i].Description, model) == 0)
				valid = true;
			else
				valid = false;

		if (valid == true)
			sprintf(ListOfDevice + strlen(ListOfDevice), "%s;",devInfo[i].SerialNumber); 
			(*count)++;
	   } 
	}

	
	return ftStatus;
}



//-------------------------------------------------------------
// sleep: Wait for some time
//-------------------------------------------------------------
void ftdi_ft60x::sleep(int wait_us)
{
#if defined(_WIN32) || defined(_WIN64)
	std::this_thread::sleep_for(std::chrono::microseconds(wait_us));

#else
	usleep(wait_us);

#endif
 
}