/*MIT License

Copyright (c) 2025 fayyaz-69

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

// File: MPU_6050 Driver Header Code
// Motion Processing Unit- MPU6050 Driver Code for ESP-IDF
// Author: Fayyaz Shaikh
// Email ID: fayyaz.shaikh24@spit.ac.in
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/

// © 2025 Fayyaz Shaikh & Team Embedded Greens
// This code and associated idea are protected under Indian copyright law.
// Unauthorized reproduction or commercial use is strictly prohibited.
// Patent pending — do not use or file without written permission.


#ifndef MY_6050_DRIVER_H
#define MY_6050_DRIVER_H

esp_err_t my_mpu6050_init(void);
uint8_t my_mpu6050_who_am_i(void);

void mpu6050_read_acceleration(float *acc_x, float *acc_y, float *acc_z);

void mpu6050_read_gyroscope(float *gyro_x, float *gyro_y, float *gyro_z);

void mpu6050_read_temperature(float *temp);

#endif
