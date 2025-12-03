#ifndef MPU6050_H
#define MPU6050_H

#include "main.h"
#include "i2c.h"

// Добавляем extern объявление
extern I2C_HandleTypeDef hi2c2;

#define MPU6050_ADDR 0xD0  // Адрес датчика (0x68 << 1)

// Регистры MPU6050
#define MPU6050_REG_PWR_MGMT_1   0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_GYRO_XOUT_H  0x43
#define MPU6050_REG_WHO_AM_I     0x75

// Структура для данных с датчика
typedef struct {
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t temp;
    float accel_x_g, accel_y_g, accel_z_g;
    float gyro_x_dps, gyro_y_dps, gyro_z_dps;
    float temp_c;
    float pitch, roll;
} MPU6050_Data;

// Прототипы функций
uint8_t MPU6050_Init(void);
void MPU6050_Read_All(MPU6050_Data* data);
void MPU6050_Calculate_Angles(MPU6050_Data* data);

#endif
