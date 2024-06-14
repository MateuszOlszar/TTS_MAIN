/*
 * App.c
 *
 *  Created on: Feb 19, 2024
 *      Author: matio
 */
#include "App.h"
#include "main.h"
#include "MLX90621_API.h"
#include "CAN_LIB.h"
#include "FLASH_Handler.h"

#define EMISSIVITY 0.98f
#define ID_BASE 0x069
extern CAN_HandleTypeDef hcan;

AppState_t _state;

uint8_t DEBUG_Preview = 0;
uint8_t inner_idx[14];
uint8_t middle_idx[14];
uint8_t outer_idx[14];
uint8_t* regions[4];

int16_t inner_temp = 0;
int16_t outer_temp = 0;
int16_t middle_temp = 0;

float tambient = 0.0f;
float result[64];

int16_t matrix[16][4];
int16_t trimmatr[10][4];
int16_t temp[64];
uint16_t trim = 0;
uint16_t cfgReg = 0;
uint16_t frame[66] = {0};
uint8_t mlxee[256]={0};
paramsMLX90621 params;

can_frame ramka[16];
can_frame tire_data_frame;
can_frame calib_data_frame;

uint32_t lastCalibFrameTime = 0;
int curRR;
int curRes;

void InitFunc();
void OpFunc();
void CalibFunc();
void ParseCalibData();
int16_t AvreageTemp(uint8_t* tyre_region, int16_t* temps);

void App_Init()
{
	_state = Init;

	while(1)
	{
		switch(_state)
		{
		case Init:
			InitFunc();
			break;
		case Operational:
			OpFunc();
			break;
		case Calibration:
			CalibFunc();
			break;
		default:
			InitFunc();
			break;
		}
	}
}

void InitFunc()
{
	CAN_Init(&hcan);
	for(uint8_t i = 0; i<16; i++)
	{
		CAN_InitFrame(&ramka[i], &hcan, ID_BASE+i, 100, 8);
	}
	CAN_InitFrame(&tire_data_frame, &hcan, ID_BASE, 100, 8);
	CAN_InitFrame(&calib_data_frame, &hcan, 0x729, 1000, 8);
	MLX90621_DumpEE(mlxee);
	MLX90621_ExtractParameters(mlxee, &params);
	MLX90621_GetConfiguration(&cfgReg);
	MLX90621_Configure(mlxee);
	MLX90621_SetRefreshRate(0x08);

	curRR = MLX90621_GetRefreshRate();
	curRes = MLX90621_GetCurResolution();

	regions[0] = NULL;
	regions[1] = inner_idx;
	regions[2] = middle_idx;
	regions[3] = outer_idx;

	uint32_t buf[11] = {0};

	Flash_Read_Data(FLASH_DATA_ADDR, buf, 11);
	memcpy(inner_idx, (uint8_t*)buf, 14);
	memcpy(middle_idx, (uint8_t*)buf + 14, 14);
	memcpy(outer_idx, (uint8_t*)buf + 28, 14);

	_state = Operational;
}

void OpFunc()
{
	MLX90621_GetFrameData(frame);
	tambient = MLX90621_GetTa(frame, &params);
	MLX90621_CalculateTo(frame, &params, EMISSIVITY, tambient, result);

	for(uint8_t i = 0; i<64; i++)
	{
		temp[i] = (int16_t)(result[i]*10);
	}

	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			matrix[i][j] = temp[i * 4 + j];
		}
	}

	if(DEBUG_Preview)
	{
		for(uint8_t i = 0; i < 16; i++)
		{
			memcpy(ramka[i].core.data, &matrix[i], 8);
			HAL_Delay(5);
			CAN_SendFrame(&ramka[i]);
		}
	}

	inner_temp = AvreageTemp(inner_idx, temp);
	outer_temp = AvreageTemp(outer_idx, temp);
	middle_temp = AvreageTemp(middle_idx, temp);



	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	if(calib_data_frame.recieve_time_ms != lastCalibFrameTime)
	{
		lastCalibFrameTime = calib_data_frame.recieve_time_ms;
		_state = Calibration;
	}

}


void CalibFunc()
{
	for(uint8_t i = 0; i<64; i++)
		{
			temp[i] = (int16_t)(result[i]*10);
		}

		for(int i = 0; i < 16; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				matrix[i][j] = temp[i * 4 + j];
			}
		}
		for(uint8_t i = 0; i < 16; i++)
		{

			memcpy(ramka[i].core.data, &matrix[i], 8);
			HAL_Delay(5);
			CAN_SendFrame(&ramka[i]);
		}
		MLX90621_GetFrameData(frame);
		tambient = MLX90621_GetTa(frame, &params);
		MLX90621_CalculateTo(frame, &params, EMISSIVITY, tambient, result);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		if(calib_data_frame.recieve_time_ms != lastCalibFrameTime)
		{
			lastCalibFrameTime = calib_data_frame.recieve_time_ms;
			if(calib_data_frame.core.data[0] == 0)
			{
				_state = Operational;
				uint32_t buf[11] = {0};
				memcpy((uint8_t*)buf, inner_idx, 14);
				memcpy((uint8_t*)buf + 14, middle_idx, 14);
				memcpy((uint8_t*)buf + 28, outer_idx, 14);
				Flash_Write_Data(FLASH_DATA_ADDR, buf, 11);
				return;
			}
			ParseCalibData();
		}
}

void ParseCalibData()
{
	uint8_t tyre_region = calib_data_frame.core.data[0];
	for(uint8_t i = 0; i< 7; i++)
	{
		uint8_t temp_idx = calib_data_frame.core.data[i+1];
		for(uint8_t j = 0; j< 14;j++)
		{
			if(regions[tyre_region]!=NULL)
			{
				if(regions[tyre_region][j] != temp_idx && regions[tyre_region][j] == 0)
				{
					regions[tyre_region][j] = temp_idx;
					break;
				}
			}
		}
	}
}

int16_t AvreageTemp(uint8_t* tyre_region, int16_t* pixels)
{
	int16_t temp = 0;
	uint8_t hit = 0;
	for(uint8_t i = 0; i< 14;i++)
	{
		if(tyre_region[i] != 0)
		{
			temp += pixels[tyre_region[i]];
			hit++;
		}
		else continue;
	}
	if(hit == 0) return 0;
	return temp/hit;
}
