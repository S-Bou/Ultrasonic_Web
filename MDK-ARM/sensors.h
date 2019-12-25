/*
Header for main.c file.
This file contains the common defines of the application.
*/

#ifndef __SENSORS_H
#define __SENSORS_H

#include <stdint.h>

#define usTIM TIM4

void DistanceUltrasonic(void);
void usDelay(uint32_t uSec);
void InitESP(void);
void find_str(uint8_t dataRX[]);
void UpPage(void);

#endif /* __SENSORS_H */
