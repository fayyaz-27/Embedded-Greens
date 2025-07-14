// main.c
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "dht11.h"
#include "my_6050_driver.h"
#include "e32_lora_driver.h"

#define TAG "MAIN"
#define DHT11_GPIO GPIO_NUM_13

// Fire risk label
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
    DHT11_init(DHT11_GPIO);
    e32_init();

    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for sensor stabilization

    while (1) {
        float acc_x, acc_y, acc_z;
        float wind_speed = 0.0;

        // Read DHT11 data
        struct dht11_reading dht_data = DHT11_read();
        if (dht_data.status != DHT11_OK) {
            ESP_LOGW(TAG, "❌ Failed to read DHT11 sensor!");
            vTaskDelay(pdMS_TO_TICKS(3000));
            continue;
        }

        float temp = (float)dht_data.temperature;
        float humidity = (float)dht_data.humidity;

        // Read MPU6050 data
        mpu6050_read_acceleration(&acc_x, &acc_y, &acc_z);

        // Estimate wind speed (demo logic using acceleration)
        wind_speed = fabs(acc_x) + fabs(acc_y) + fabs(acc_z);
        wind_speed *= 10.0f;  // scale for demo

        // Fire Risk Scoring
        int score = 0;

        // 🔥 Temperature scoring
        if (temp >= 45.0) score += 3;
        else if (temp >= 40.0) score += 2;
        else if (temp >= 35.0) score += 1;

        // 💧 Humidity scoring (low humidity = higher risk)
        if (humidity <= 15.0) score += 3;
        else if (humidity <= 30.0) score += 2;
        else if (humidity <= 45.0) score += 1;

        // 🌬️ Wind speed scoring
        if (wind_speed >= 25.0) score += 3;
        else if (wind_speed >= 15.0) score += 2;
        else if (wind_speed >= 5.0) score += 1;

        const char* fire_risk = get_fire_risk_level(score);

        // Output results
        printf("\n📊 Wildfire Risk Evaluation\n");
        printf("-----------------------------\n");
        printf("🌡️  Temp       : %.2f °C\n", temp);
        printf("💧 Humidity   : %.2f %%\n", humidity);
        printf("🌬️  Wind Speed : %.2f (est.)\n", wind_speed);
        printf("🔥 Fire Risk  : %s (Score = %d)\n", fire_risk, score);

        // Send LoRa alert for HIGH or CRITICAL
        if (strcmp(fire_risk, "HIGH") == 0 || strcmp(fire_risk, "CRITICAL") == 0) {
            char alert_msg[128];
            snprintf(alert_msg, sizeof(alert_msg),
                     "🔥 ALERT: %s Fire Risk | 🌡️ %.2f°C | 💧 %.2f%% | 🌬️ %.2f",
                     fire_risk, temp, humidity, wind_speed);
            e32_transmit(alert_msg);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // Delay for 5 seconds
    }
}
