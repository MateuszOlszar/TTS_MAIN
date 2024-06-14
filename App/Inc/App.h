/*
 * App.h
 *
 *  Created on: Feb 19, 2024
 *      Author: matio
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#define FLASH_DATA_ADDR 0x0800F800

typedef enum
{
	Init,
	Operational,
	Calibration,

}AppState_t;

void App_Init();


#endif /* INC_APP_H_ */
