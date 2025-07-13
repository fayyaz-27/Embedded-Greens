// File: MPU_6050 Driver Code
// Motion Processing Unit- MPU6050 Driver Code for ESP-IDF
// Author: Fayyaz Shaikh
// Email ID: fayyaz.shaikh24@spit.ac.in
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/

#include<stdio.h>
#include "esp_system.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "my_6050_driver.h"

#define MPU6050_ADDRESS 0x68u
#define WHO_AM_I 0x75u
#define POWER_MANAGEMENT_1 0x6Bu

#define GYRO_CONFIG 0x1Bu
#define ACCEL_CONFIG 0x1Cu

#define ACCEL_XOUT_H 0x3Bu
#define GYRO_XOUT_H 0x43u
#define TEMP_OUT_H 0x41u


#define I2C_SCL_PIN 22
#define I2C_SDA_PIN 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQUENCY 100000


esp_err_t my_mpu6050_init(){
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQUENCY
    };

    esp_err_t err;

    err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK){
        return err;
    }
    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK){
        return err;
    }

    uint8_t data = 0;
    return i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDRESS, (uint8_t[]){POWER_MANAGEMENT_1, data}, 2, 1000/portTICK_PERIOD_MS);    
}

uint8_t my_mpu6050_who_am_i (){
    uint8_t data_who_am_i;
    i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDRESS, (uint8_t[]){WHO_AM_I}, 1, &data_who_am_i, 1, 1000/portTICK_PERIOD_MS);
    return data_who_am_i;
}

void mpu6050_read_acceleration(float *acc_x, float *acc_y, float *acc_z){
    uint8_t data[6];

    i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDRESS, (uint8_t[]){ACCEL_XOUT_H}, 1, data, 6, 1000/portTICK_PERIOD_MS);

    int16_t raw_acc_x = (int16_t)(data[0]<<8 | data[1]);
    int16_t raw_acc_y = (int16_t)(data[2]<<8 | data[3]);
    int16_t raw_acc_z = (int16_t)(data[4]<<8 | data[5]);

    float accel_sensitivity = 16384.0f;   // for range ±2g
    
    *acc_x = raw_acc_x / accel_sensitivity;
    *acc_y = raw_acc_y / accel_sensitivity;
    *acc_z = raw_acc_z / accel_sensitivity;

}

void mpu6050_read_gyroscope(float *gyro_x, float *gyro_y, float *gyro_z){
    uint8_t gyro_data[6];

    i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDRESS, (uint8_t[]){GYRO_XOUT_H}, 1, gyro_data, 6, 1000/portTICK_PERIOD_MS);

    int16_t raw_gyro_x = (int16_t)(gyro_data[0]<<8 | gyro_data[1]);
    int16_t raw_gyro_y = (int16_t)(gyro_data[2]<<8 | gyro_data[3]);
    int16_t raw_gyro_z = (int16_t)(gyro_data[4]<<8 | gyro_data[5]);

    float gyro_sensitivity = 131.0f;       // for ±250°/s

    *gyro_x = raw_gyro_x / gyro_sensitivity;
    *gyro_y = raw_gyro_y / gyro_sensitivity;
    *gyro_z = raw_gyro_z / gyro_sensitivity;
}

void mpu6050_read_temperature(float *temp){
    uint8_t temperature_data[2];

    i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDRESS, (uint8_t[]){TEMP_OUT_H}, 1, temperature_data, 2, 1000/portTICK_PERIOD_MS);

    int16_t raw_temp = (int16_t)((temperature_data[0]<<8) | temperature_data[1]);
    *temp = (raw_temp/340.0f) + 36.53;

}