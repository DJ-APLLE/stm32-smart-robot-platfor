#ifndef __SERVO_H
#define __SERVO_H

#include "includes.h"


void Servo_PB0_Init(void);
void Set_Servo_Angle(float angle);
void Set_Servo_Pulse(uint16_t pulse_us);

#endif
