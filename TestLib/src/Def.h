#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#else
	#include <unistd.h>

#endif
#include <stdlib.h>
#include <stdint.h>
#pragma once



typedef struct
{
	uint64_t Time_Code;
	uint64_t RunTime_Code;
	uint32_t AsicID;
	uint32_t Pack_Id;
	uint32_t *Charge;
	uint32_t *CoarseTime;
	uint32_t *FineTime;
	uint32_t *Hit;
} t_ASIC_packet;


typedef struct
{
	t_ASIC_packet *packets;
	int allocated_packets;
	int valid_packets;
} t_ASIC_packet_collection;
