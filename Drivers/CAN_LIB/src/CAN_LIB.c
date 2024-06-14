#include "CAN_LIB.h"
#include "CAN_LIB_HW.h"
#include "string.h"
#include "math.h"


uint64_t CAN_ConvertIntToFrameBits(int32_t value, uint8_t bit_ofs, uint8_t length);
uint64_t CAN_ConvertUintToFrameBits(uint32_t value, uint8_t bit_ofs, uint8_t length);
int32_t CAN_ConvertFrameBitsToInt(uint8_t* ptr, uint8_t bit_ofs, uint8_t length);
uint32_t CAN_ConvertFrameBitsToUint(uint8_t* ptr, uint8_t bit_ofs, uint8_t length);

uint8_t FilterFrame(CAN_HandleTypeDef* hcan, uint16_t id);

can_info _can_info;

void CAN_Init(CAN_HandleTypeDef* hcan){
	static uint8_t can_number;
	if(can_number >= NUMBER_OF_CANS)
		Error_Handler();

	cans[can_number].hcan = hcan;
	cans[can_number].retransmision_counter = 0;

	CAN_HW_Init(hcan);
	CAN_FIFO_Flush(&cans[can_number].fifo);

	can_number++;
}

void CAN_InitFrame(can_frame* pFrame, CAN_HandleTypeDef* hcan, uint16_t id, uint16_t period, uint8_t dlc)
{
	pFrame->hcan = hcan;
	pFrame->core.id = id;
	pFrame->period = period;
	pFrame->core.dlc = dlc;
	_can_info.pFrames[_can_info.frames_num] = pFrame;
	_can_info.frames_num++;

}

void CAN_AddExtId(can_frame* pFrame, uint32_t ext_id)
{
	pFrame->core.isExtId = 1;
	pFrame->core.id |= ((ext_id<<14)>>14)<<18;
}

void CAN_InitValue(can_frame* pFrame, can_value* pCan_value, uint8_t bit_ofs,
					uint16_t mult, uint16_t div, uint16_t ofs, can_type type)
{
	pCan_value->pFrame = pFrame;
	pCan_value->bit_ofs = bit_ofs;
	pCan_value->mult = mult;
	pCan_value->div = div;
	pCan_value->ofs = ofs;
	pCan_value->type = type;
	pFrame->values_num++;
	_can_info.values_num++;
}

/* returns index of frame in _can_info or
   returns 255 if didn't found matching frame */
uint8_t FilterFrame(CAN_HandleTypeDef* hcan, uint16_t id)
{
	for(uint8_t i = 0; i < _can_info.frames_num; i++){
		if(_can_info.pFrames[i]->core.id == id && _can_info.pFrames[i]->hcan == hcan){
			return i;
		}
	}
	return 255;
}
/**********************************************/


uint32_t CAN_ConvertFrameBitsToUint(uint8_t* ptr, uint8_t bit_ofs, uint8_t length)
{
	return (uint32_t)((((*(uint64_t*)ptr)>>bit_ofs)<<(64-length))>>(64-length));
}

int32_t CAN_ConvertFrameBitsToInt(uint8_t* ptr, uint8_t bit_ofs, uint8_t length)
{
	return (int32_t)((((*(int64_t*)ptr)>>bit_ofs)<<(64-length))>>(64-length));
}

uint64_t CAN_ConvertUintToFrameBits(uint32_t value, uint8_t bit_ofs, uint8_t length)
{
	return ((((uint64_t)value)<<bit_ofs)<<(64-length-bit_ofs))>>(64-length-bit_ofs);
}

uint64_t CAN_ConvertIntToFrameBits(int32_t value, uint8_t bit_ofs, uint8_t length)
{
	int64_t int_temp = ((((int64_t)value)<<bit_ofs)<<(64-length-bit_ofs))>>(64-length-bit_ofs);
	return *(uint64_t*)&int_temp;
}



void CAN_WriteValueFloat(can_value* pCan_value, float value)
{
	uint64_t mask;
	uint64_t* ptr = (uint64_t*)pCan_value->pFrame->core.data;
	uint8_t type = pCan_value->type;

	value =  (value - pCan_value->ofs)/pCan_value->mult*pCan_value->div;

	if(type <= 32){
		mask = CAN_ConvertUintToFrameBits(0xffffffff, pCan_value->bit_ofs, type);
		//clear value bits in frame
		*ptr &= ~mask;
		//copy value bits to frame
		*ptr |= CAN_ConvertUintToFrameBits((uint32_t)roundf(value), pCan_value->bit_ofs, type);
	}
	else{
		mask = CAN_ConvertUintToFrameBits(0xffffffff, pCan_value->bit_ofs, type-32);
		//clear value bits in frame
		*ptr &= ~mask;
		//copy value bits to frame
		*ptr |= CAN_ConvertIntToFrameBits((int32_t)roundf(value), pCan_value->bit_ofs, type-32);
	}
}


void CAN_WriteValue(can_frame* pFrame, float value, uint8_t bit_ofs, can_type can_type, uint16_t mult, uint16_t div, uint16_t ofs)
{
	uint64_t* ptr = (uint64_t*)pFrame->core.data;
	uint64_t mask;
	uint8_t type = can_type;

	value =  (value-ofs)/mult*div;

	if(type <= 32){
		mask = CAN_ConvertUintToFrameBits(0xffffffff, bit_ofs, type);
		//clear value bits in frame
		*ptr &= ~mask;
		//copy value bits to frame
		*ptr |= CAN_ConvertUintToFrameBits((uint32_t)roundf(value), bit_ofs, type);
	}
	else{
		mask = CAN_ConvertUintToFrameBits(0xffffffff, bit_ofs, type-32);
		//clear value bits in frame
		*ptr &= ~mask;
		//copy value bits to frame
		*ptr |= CAN_ConvertIntToFrameBits((int32_t)roundf(value), bit_ofs, type-32);
	}
}



float CAN_ReadValueFloat(can_value* pCan_value)
{
	uint8_t type = pCan_value->type;
	float value;

	if(type <= 32){
		value = (float)CAN_ConvertFrameBitsToUint(pCan_value->pFrame->core.data, pCan_value->bit_ofs, type);
	}
	else{
		value = (float)CAN_ConvertFrameBitsToInt(pCan_value->pFrame->core.data, pCan_value->bit_ofs, type-32);
	}

	value = (value + pCan_value->ofs)*pCan_value->mult/pCan_value->div;
	return value;
}


float CAN_ReadValue(can_frame* pFrame, uint8_t bit_ofs, can_type can_type, uint16_t mult, uint16_t div, uint16_t ofs)
{
	uint8_t type = can_type;
	float value;

	if(type <= 32){
		value = (float)CAN_ConvertFrameBitsToUint(pFrame->core.data, bit_ofs, type);
	}
	else{
		value = (float)CAN_ConvertFrameBitsToInt(pFrame->core.data, bit_ofs, type-32);
	}

	value = (value + ofs)*mult/div;
	return value;
}

uint8_t CAN_IsValueActual(can_value* pCan_value)
{
	return (pCan_value->pFrame->recieve_time_ms != 0) || (HAL_GetTick() - pCan_value->pFrame->recieve_time_ms < pCan_value->pFrame->period*ACTUAL_TIME_MULTIPLIER);
}

uint8_t CAN_IsFrameActual(can_frame* pFrame)
{
	return (pFrame->recieve_time_ms != 0) || (HAL_GetTick() - pFrame->recieve_time_ms < pFrame->period*ACTUAL_TIME_MULTIPLIER);
}

void CAN_SendFrame(can_frame* pFrame)
{
	if(CAN_HW_SendFrame(pFrame) != CAN_SUCCESS){
		for(uint8_t i=0; i<NUMBER_OF_CANS; i++)
		{
			if(pFrame->hcan == cans[i].hcan)
			{
				CAN_FIFO_Add(&cans[i].fifo, (uint8_t*)pFrame);
			}
		}
	}
}

void CAN_SendFrame_Periodically(can_frame* pFrame)
{
	if(HAL_GetTick() - pFrame->send_time_ms >= pFrame->period){

		if(CAN_HW_SendFrame(pFrame) != CAN_SUCCESS){
			for(uint8_t i=0; i<NUMBER_OF_CANS; i++)
			{
				if(pFrame->hcan == cans[i].hcan)
				{
					CAN_FIFO_Add(&cans[i].fifo, (uint8_t*)pFrame);
				}
			}
		}
		pFrame->send_time_ms = HAL_GetTick();
	}
}

void CAN_SetRxCallback(CAN_HandleTypeDef* hcan, void (*callback)(can_frame* frame))
{
	for(uint8_t i=0; i<NUMBER_OF_CANS; i++){
		if(hcan == cans[i].hcan){
			cans[i].callback = callback;
		}
	}
}

void CAN_ReceiveCallback(CAN_HandleTypeDef* hcan, can_msg* msg)
{
	uint8_t index = FilterFrame(hcan, msg->id);
	if(index==255) return;
	memcpy(_can_info.pFrames[index]->core.data, msg->data, 8);
	_can_info.pFrames[index]->recieve_time_ms = HAL_GetTick();
	for(uint8_t i=0; i<NUMBER_OF_CANS; i++){
		if(hcan == cans[i].hcan && cans[i].callback != NULL){
			cans[i].callback(_can_info.pFrames[index]);
		}
	}
}

void CAN_TxCompleteCallback(CAN_HandleTypeDef* hcan)
{
	for(uint8_t i=0; i<NUMBER_OF_CANS; i++){
		if(hcan == cans[i].hcan){
			uint8_t* pFrame = CAN_FIFO_Get(&cans[i].fifo);
			if(pFrame){
				CAN_SendFrame((can_frame*)pFrame);
			}
		}
	}
}


