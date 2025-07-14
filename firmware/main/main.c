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

// File: Embedded Green Main Program Code
// Made for ESP-IDF
// Author: Fayyaz Shaikh
// Email ID: fayyaz.shaikh24@spit.ac.in
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/

// © 2025 Fayyaz Shaikh & Team Embedded Greens
// This code and associated idea are protected under Indian copyright law.
// Unauthorized reproduction or commercial use is strictly prohibited.
// Patent pending — do not use or file without written permission.

/*
    Required Tech and Hardware stack:
    1. ESP32
    2. LoRa module
    3. MPU6050/Anemometer
    4. DHT11
    5. ESP-IDF
*/


#include <stdio.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "dht11.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "my_6050_driver.h"
#include "e32_lora_driver.h"

#define TAG "EMBEDDED GREENS"
#define DHT11_GPIO GPIO_NUM_13

const char* get_fire_risk_level(int score) {
    if (score >= 8) return "CRITICAL";
    else if (score >= 5) return "HIGH";
    else if (score >= 3) return "MODERATE";
    else return "LOW";
}

void app_main() {
    ESP_LOGI(TAG, "🌲 Initializing Wildfire Detection System...");

    // Initialize sensors and LoRa
    my_mpu6050_init();
    DHT11_init(13);
    e32_init();
    gpio_set_direction(2, GPIO_MODE_OUTPUT);

    vTaskDelay(pdMS_TO_TICKS(2000));  
    while (1) {
        float temp, acc_x, acc_y, acc_z, wind_speed;
        float humidity;

        // Temperature from MPU6050 
        mpu6050_read_temperature(&temp);
        humidity = (rand() % 10) + 30; 

        // Read MPU6050 Acceleration
        mpu6050_read_acceleration(&acc_x, &acc_y, &acc_z);

        // Calculating wind speed using magnitude formula
        wind_speed = sqrtf(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z);
        wind_speed *= 10.0f;  

        // Fire Risk Score Calculation
        int score = 0;

        if (temp >= 45.0) score += 3;
        else if (temp >= 40.0) score += 2;
        else if (temp >= 35.0) score += 1;

        if (humidity <= 15.0) score += 3;
        else if (humidity <= 30.0) score += 2;
        else if (humidity <= 45.0) score += 1;

        if (wind_speed >= 25.0) score += 3;
        else if (wind_speed >= 15.0) score += 2;
        else if (wind_speed >= 5.0) score += 1;

        const char* level = get_fire_risk_level(score);

        // Print to Serial Monitor
        printf("\n📊 Wildfire Risk Evaluation\n");
        printf("-----------------------------\n");
        printf("🌡️  Temp       : %.2f °C\n", temp);
        printf("💧 Humidity   : %.2f %%\n", humidity);
        printf("🌬️  Wind Speed : %.2f (est.)\n", wind_speed);
        printf("🔥 Fire Risk  : %s (Score = %d)\n", level, score);

        // Alert via LoRa for MODERATE and above
        if (strcmp(level, "MODERATE") == 0 || strcmp(level, "HIGH") == 0 || strcmp(level, "CRITICAL") == 0) {
            char alert_msg[128];
            snprintf(alert_msg, sizeof(alert_msg),
                     "🔥 ALERT: Fire Risk %s | Temp: %.2f°C | Humidity: %.2f%% | Wind: %.2f",
                     level, temp, humidity, wind_speed);
            e32_transmit(alert_msg);
            gpio_set_level(2, 1);
            vTaskDelay(pdMS_TO_TICKS(300));
            gpio_set_level(2, 0);
            printf("📡 Data sent to nearest base station using LoRa.\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); 
    }
}
