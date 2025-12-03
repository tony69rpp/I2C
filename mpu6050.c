#include "mpu6050.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>  // Добавляем для abs()

uint8_t MPU6050_Init(void) {
    uint8_t check_data;

    // Проверка подключения датчика
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, MPU6050_REG_WHO_AM_I, 1, &check_data, 1, 1000);

    if (status != HAL_OK) {
        printf("I2C Read Error: %d\r\n", status);
        return 0;
    }

    printf("MPU6050 WHO_AM_I: 0x%02X\r\n", check_data);

    if (check_data != 0x68) {
        printf("MPU6050 not found, expected 0x68, got 0x%02X\r\n", check_data);
        return 0;
    }

    // Выход из sleep режима
    uint8_t pwr_mgmt_1 = 0x00;
    if (HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, 1, &pwr_mgmt_1, 1, 1000) != HAL_OK) {
        printf("Failed to wake up MPU6050\r\n");
        return 0;
    }

    HAL_Delay(100);
    printf("MPU6050 Initialized Successfully\r\n");
    return 1;
}

void MPU6050_Read_All(MPU6050_Data* data) {
    uint8_t buffer[14];

    // Чтение всех данных (ускорение, температура, гироскоп)
    if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, 1, buffer, 14, 1000) != HAL_OK) {
        printf("Failed to read MPU6050 data\r\n");
        return;
    }

    // Преобразование данных ускорения
    data->accel_x = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->accel_y = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->accel_z = (int16_t)((buffer[4] << 8) | buffer[5]);

    // Преобразование температуры
    data->temp = (int16_t)((buffer[6] << 8) | buffer[7]);

    // Преобразование данных гироскопа
    data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]);
    data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]);

    // Конвертация в физические величины
    // Ускорение (g): ±2g = 16384 LSB/g
    data->accel_x_g = data->accel_x / 16384.0;
    data->accel_y_g = data->accel_y / 16384.0;
    data->accel_z_g = data->accel_z / 16384.0;

    // Гироскоп (°/s): ±250°/s = 131 LSB/°/s
    data->gyro_x_dps = data->gyro_x / 131.0;
    data->gyro_y_dps = data->gyro_y / 131.0;
    data->gyro_z_dps = data->gyro_z / 131.0;

    // Температура (°C)
    data->temp_c = data->temp / 340.0 + 36.53;

    // Расчет углов наклона
    MPU6050_Calculate_Angles(data);
}

void MPU6050_Calculate_Angles(MPU6050_Data* data) {
    // Расчет углов наклона по акселерометру
    data->roll = atan2(data->accel_y_g, data->accel_z_g) * 180.0 / M_PI;
    data->pitch = atan2(-data->accel_x_g, sqrt(data->accel_y_g * data->accel_y_g + data->accel_z_g * data->accel_z_g)) * 180.0 / M_PI;
}
