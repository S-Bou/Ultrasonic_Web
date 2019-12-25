/* 
In this file we have multiple function to manage different sensors
*/
#include "sensors.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

const float speedOfSound = 0.0342/2;
float distance = 0;
char uartComAT[100];
char page[200];
uint32_t numTicks;
extern UART_HandleTypeDef huart2;
uint8_t chanel=66;

/************************************* DISTANCE ULTRASONIC *************************************/
/*Call this function to obtain the distance of the most proxim object in  front of sensor ultrasonic*/

void DistanceUltrasonic()
{
		//Set TRIG to LOW for few uSec
		HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
		usDelay(50);
		// START Ultrasonic measure routine
		//1. Output 10 uSec TRIG
		HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
		usDelay(20);
		HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
		
		//2. Wait for ECHO pin rising edge
		while(HAL_GPIO_ReadPin(ECHO_GPIO_Port, ECHO_Pin) == GPIO_PIN_RESET);
		
		//3. Start measure ECHO pulse whidt in uSec
		numTicks = 0;
		while(HAL_GPIO_ReadPin(ECHO_GPIO_Port, ECHO_Pin) == GPIO_PIN_SET)
		{
			numTicks++;
			usDelay(2);	//2. 8 uSec
		}
		
		//4. Estimate distance in cm
		distance = (numTicks + 0.0f)*2.4*speedOfSound;
}

void InitESP(void)
{
	sprintf(uartComAT, "AT+CIPMUX=1\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT, strlen(uartComAT), 100);
	HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);	//Led green
	HAL_Delay(2000);
	sprintf(uartComAT, "AT+CIPSERVER=1,80\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT, strlen(uartComAT), 100);
	HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);	//Led green
	HAL_Delay(2000);
}

void usDelay(uint32_t uSec)
{
	if(uSec < 2) uSec = 2;
	usTIM->ARR = uSec - 1;		/*Sets the value in the auto-reload register*/
	usTIM->EGR = 1;						/*Re-initialiser the timer*/
	usTIM->SR &= ~1;					//Reset the flag
	usTIM->CR1 |= 1;					//Enables the counter
	while((usTIM->SR&0x0001) != 1);
	usTIM->SR &= ~(0x0001);
}

void find_str(uint8_t dataRX[]) 
{ 
	int i=0;
	for(i=0; i<400; i++)
	{		
		if(dataRX[i]=='+' && dataRX[i+1]=='I' && dataRX[i+2]=='P' && dataRX[i+3]=='D')
		{
			chanel = dataRX[i+5];
			HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);	//Green	
		}
  }	
	
}

void UpPage(void)
{
  sprintf(page, "<!doctype html><html><head><meta http-equiv=\"refresh\" content=\"5\"></head><body>"
	"<h1>Esto es una prueba de funcionamiento."	
	"<br>"								             
	"Distancia: %.2f"
	"</h1></body></html>", distance);             	
	
	sprintf(uartComAT, "AT+CIPSEND=0,%d\r\n", strlen(page));
	HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT, strlen(uartComAT), 100);
	HAL_Delay(2000);
	sprintf(uartComAT, "%s\r\n", page);
	HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT, strlen(uartComAT), 100);
	HAL_Delay(3000);
	sprintf(uartComAT, "AT+CIPCLOSE=%d\r\n", chanel-48);
	HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT, strlen(uartComAT), 100);
}
