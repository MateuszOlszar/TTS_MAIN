/*
 * FIFO.c
 *
 *  Created on: 30.11.2018
 *      Author: Pawe�
 */

#include "CAN_FIFO.h"


void CAN_FIFO_Add(can_fifo *fifo, uint8_t *data_ptr)
{
	if(fifo->empty)
	{
		fifo->data_ptr[fifo->last_element]=data_ptr;
		fifo->empty=0;
		return;
	}
	fifo->last_element=(fifo->last_element+1)%CAN_FIFO_SIZE;

	if(fifo->last_element==fifo->first_element)
	{
		fifo->first_element=(fifo->first_element+1)%CAN_FIFO_SIZE;
	}
	fifo->data_ptr[fifo->last_element]=data_ptr;
}

/* return 0 if FIFO is empty */
uint8_t* CAN_FIFO_Get(can_fifo *fifo)
{
	uint8_t *temp;
	if(fifo->empty){
		return 0;
	}
	temp=fifo->data_ptr[fifo->first_element];

	if(fifo->first_element==fifo->last_element){
		fifo->empty=1;
		return temp;
	}
	fifo->first_element=(fifo->first_element+1)%CAN_FIFO_SIZE;

	return temp;
}

void CAN_FIFO_Flush(can_fifo *fifo)
{
	fifo->first_element=0;
	fifo->last_element=0;
	fifo->empty=1;
}
