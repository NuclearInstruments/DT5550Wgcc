#ifndef NI_USB3_CLASS_H
#define NI_USB3_CLASS_H
#include <stdint.h>


#include "ftdi_ft60x.h"

//-------------------------------------------------------------
// ftdi_ft60x: FT60x interface
//-------------------------------------------------------------
class ni_usb3_class 
{
public:
    ni_usb3_class(){};
    int32_t open(char * SerialNumber, int clock);
    int32_t close(void);
    int32_t write_mem(uint32_t *data, uint32_t count, uint32_t address, int bus_mode, uint32_t timeout_ms, uint32_t *written_data);
    int32_t read_mem(uint32_t *data, uint32_t count, uint32_t address, int bus_mode, uint32_t timeout_ms, uint32_t *read_data, uint32_t *valid_data);
    int32_t read_reg(uint32_t *data, uint32_t address);
    int32_t write_reg(uint32_t data, uint32_t address);
    int32_t listdevices(char *ListOfDevice, char *model,  int *count);

protected:
	ftdi_ft60x port;
	float clock_period;
};

#endif
