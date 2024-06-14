#ifndef CAN_LIB_H
#define CAN_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "CAN_value_types.h"
#include "CAN_FIFO.h"

#define CAN_MAX_VALUE_NAME 		50
#define CAN_MAX_FRAME_NUM 		30
#define ACTUAL_TIME_MULTIPLIER	4


typedef struct {
	uint8_t	isExtId;
	uint32_t id;
	uint32_t dlc;
	uint8_t data[8];
} can_msg;

typedef struct {
	CAN_HandleTypeDef* hcan;
	can_msg core;
	uint16_t period;
	uint8_t values_num;
	uint32_t recieve_time_ms;
	uint32_t send_time_ms;
} can_frame;

typedef struct {
	can_frame* pFrame;
	uint8_t name[CAN_MAX_VALUE_NAME];
	uint8_t bit_ofs;
	uint16_t mult;
	uint16_t div;
	uint16_t ofs;
	can_type type;
} can_value;

typedef struct {
	uint8_t frames_num;
	can_frame* pFrames[CAN_MAX_FRAME_NUM];
	uint8_t values_num;
} can_info;

typedef struct {
	CAN_HandleTypeDef* hcan;
	can_fifo fifo;
	uint8_t retransmision_counter;
	void (*callback)(can_frame* frame);
} can_lib;


void CAN_Init(CAN_HandleTypeDef* hcan);
void CAN_InitFrame(can_frame* pFrame, CAN_HandleTypeDef* hcan, uint16_t id, uint16_t period, uint8_t dlc);
void CAN_AddExtId(can_frame* pFrame, uint32_t ext_id);
void CAN_InitValue(can_frame* pFrame, can_value* pCan_value, uint8_t bit_ofs,
				   uint16_t mult, uint16_t div, uint16_t ofs, can_type type);
void CAN_WriteValueFloat(can_value* pCan_value, float value);
void CAN_WriteValue(can_frame* pFrame, float value, uint8_t bit_ofs, can_type can_type, uint16_t mult, uint16_t div, uint16_t ofs);
float CAN_ReadValueFloat(can_value* pCan_value);
float CAN_ReadValue(can_frame* pFrame, uint8_t bit_ofs, can_type can_type, uint16_t mult, uint16_t div, uint16_t ofs);
uint8_t CAN_IsValueActual(can_value* pCan_value);
uint8_t CAN_IsFrameActual(can_frame* pFrame);
void CAN_SendFrame(can_frame* pFrame);
void CAN_SendFrame_Periodically(can_frame* pFrame);
void CAN_ReceiveCallback(CAN_HandleTypeDef* hcan, can_msg* msg);
void CAN_TxCompleteCallback(CAN_HandleTypeDef* hcan);
void CAN_SetRxCallback(CAN_HandleTypeDef* hcan, void (*callback)(can_frame* frame));

#ifdef __cplusplus
}
#endif

#endif
