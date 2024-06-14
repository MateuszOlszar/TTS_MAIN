/*
 * FIFO.h
 *
 *  Created on: 30.11.2018
 *      Author: Pawe�
 */


#ifndef CAN_FIFO_H_
#define CAN_FIFO_H_

#include <main.h>
#define CAN_FIFO_SIZE 10

typedef struct {
	uint8_t first_element;
	uint8_t last_element;
	uint8_t* data_ptr[CAN_FIFO_SIZE];
	uint8_t empty;
} can_fifo;

void CAN_FIFO_Add(can_fifo*, uint8_t*);
uint8_t* CAN_FIFO_Get(can_fifo*);
void CAN_FIFO_Flush(can_fifo*);

#endif /* FIFO_H_ */
