#ifndef MY_6050_DRIVER_H
#define MY_6050_DRIVER_H

esp_err_t my_mpu6050_init(void);
uint8_t my_mpu6050_who_am_i(void);

void mpu6050_read_acceleration(float *acc_x, float *acc_y, float *acc_z);

void mpu6050_read_gyroscope(float *gyro_x, float *gyro_y, float *gyro_z);

void mpu6050_read_temperature(float *temp);

#endif