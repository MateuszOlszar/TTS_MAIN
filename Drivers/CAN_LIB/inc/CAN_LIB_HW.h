#ifndef CAN_LIB_HW_H
#define CAN_LIB_HW_H

#include "main.h"
#include "CAN_LIB.h"

#define NUMBER_OF_CANS		3
#define CAN_ARB_LOST_RETR 	40

static can_lib cans[NUMBER_OF_CANS];

typedef enum {
	CAN_FAILURE,
	CAN_SUCCESS
} CAN_status;

CAN_status CAN_HW_SendFrame(can_frame* pFrame);
void CAN_HW_Init(CAN_HandleTypeDef* hcan);


#endif

